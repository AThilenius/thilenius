// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.content_window', ['thilenius.console_window'])
    .directive('atContentWindow', [
      '$rootScope',
      '$timeout',
      'billet',
      function($rootScope, $timeout, billet) {
        return {
          restrict: 'AE',
          scope: {control: '=', editor: '='},
          templateUrl: 'app/directives/content_window/content_window.htm',
          link: function(scope, iElement, iAttrs) {
            scope.editor = ace.edit("editor");
            scope.editor.setTheme("ace/theme/twilight");
            scope.editor.getSession().setMode("ace/mode/c_cpp");
            scope.editor.getSession().setTabSize(2);
            scope.editor.$blockScrolling = Infinity;
            scope.editor.setOptions({
              enableBasicAutocompletion: true,
              enableSnippets: true,
              enableLiveAutocompletion: true
            });
            scope.editorVisible = false;
            scope.activeRepo = null;
            scope.relativePath = null;
            scope.showConsole = false;
            scope.canRun = true;
            scope.fileSessions = {};
            scope.consoleWindowControl = {};
            scope.internalControl = scope.control || {};

            $rootScope.$watchCollection(
                'editorSettings', function(settings, oldVal) {
                  if (settings) {
                    scope.editor.setOption("foldStyle", settings.foldStyle);
                    scope.editor.setOption("fontSize", settings.fontSize);
                    scope.editor.setOption(
                        "selectionStyle",
                        settings.fullLineSelection ? 'line' : 'text');
                    scope.editor.setOption("highlightActiveLine",
                                           settings.highlightActiveLine);
                    scope.editor.setOption("highlightSelectedWord",
                                           settings.highlightSelectedWord);
                    if (settings.keyBinding === 'ace') {
                      scope.editor.setKeyboardHandler("");
                    } else {
                      scope.editor.setKeyboardHandler("ace/keyboard/" +
                                                      settings.keyBinding);
                    }
                    scope.editor.setOption("scrollPastEnd",
                                           settings.scrollPastEnd);
                    scope.editor.setOption("showGutter", settings.showGutter);
                    scope.editor.setOption("theme", settings.theme);
                    scope.editor.setOption("displayIndentGuides",
                                           settings.displayIndentGuides);
                    scope.editor.setOption("showInvisibles",
                                           settings.showInvisibles);
                    scope.editor.setOption("showPrintMargin",
                                           settings.showPrintMargin);
                    scope.editor.setOption("fadeFoldWidgets",
                                           settings.fadeFoldWidgets);
              }
            }, true);

            scope.internalControl.commitPending = function(callback) {
              if (scope.activeRepo && !scope.editor.getReadOnly()) {
                // Commit any pending code
                scope.activeRepo.commit(scope.relativePath,
                                        scope.editor.getValue(),
                                        callback);
              }
            };

            // Watch for changes to console size, so we can refresh the editor
            // size
            scope.$watch('showConsole', function(newVal, oldVal) {
              $timeout(function() {
                scope.editor.resize();
                if (!newVal) {
                  // Also focus the editor if the console is hidden
                  scope.editor.focus();
                }
              }, 0);
            });

            // Watch for billet begin-done for run flag
            $rootScope.$on('billet.activeCord', function(event, cordStream) {
              scope.canRun = false;
              scope.$apply();
              cordStream.addHandler('close', function() {
                scope.canRun = true;
                scope.$apply();
              });
            });

            // Also check Billet directly for old/active cords
            if (billet.currentCord && billet.isOldCordRunning) {
              scope.canRun = false;
              billet.currentCord.addHandler('close', function() {
                scope.canRun = true;
                scope.$apply();
              });
            }

            // Binds a file from a Repo for edit. Also stashes and commits
            // changes. If the file is locked, it cannot be changed
            scope.internalControl.bindRepoFileForEdit = function(repo,
                                                                 relativePath) {
              scope.unload();
              scope.lastStash = new Date();
              scope.activeRepo = repo;
              scope.relativePath = relativePath;
              scope.editor.setValue(repo.headState[relativePath].source, 1);
              scope.editorVisible = true;
              scope.restoreFileSession();
              if (repo.headState[relativePath].file_info.is_locked &&
                  repo.repoProto.repo_header.base_repo_uuid !== '') {
                scope.editor.setReadOnly(true);
                scope.isLockedFile = true;
              } else {
                scope.changeHandler = function(e) {
                  if (!scope.lastStash) {
                    scope.lastStash = new Date();
                  }
                  // Commit at max once every 15 seconds
                  if (new Date() - scope.lastStash >= 15000) {
                    scope.lastStash = new Date();
                    repo.commit(relativePath, scope.editor.getValue());
                  }
                };
                scope.editor.setReadOnly(false);
                scope.editor.on(
                    "change", function(e) { scope.changeHandler(e); });
              }
            };

            // Binds a file from a Repo for view (read only).
            scope.internalControl.bindRepoFileForView = function(
                repo, relativePath, changeList) {
              scope.unload();
              scope.activeRepo = repo;
              scope.relativePath = relativePath;
              scope.editor.setValue(
                  repo.reconstructFilesForCL(
                           changeList.change_list_uuid)[relativePath].source,
                  1);
              scope.editor.setReadOnly(true);
              scope.editorVisible = true;
              scope.restoreFileSession();
            };

            // private
            scope.run = function() {
              if (scope.activeRepo) {
                scope.internalControl.commitPending(function() {
                  billet.runCMakeRepo(scope.activeRepo.repoProto.repo_header);
                });
              }
            };

            // private
            scope.stop = function() {
              if (scope.activeRepo) {
                billet.terminateSession();
              }
            };

            scope.clean = function() {
              if (scope.activeRepo) {
                billet.clean(scope.activeRepo.repoProto.repo_header);
              }
            }

            // private
            scope.autoFormat = function() {
              if (scope.activeRepo) {
                if (scope.editor.getReadOnly()) {
                  // Don't format read only
                  return;
                }
                scope.saveFileSession();
                scope.editor.setReadOnly(true);
                billet.clangFormat(
                    scope.editor.getValue(),
                    function(newSource) {
                      scope.editor.setValue(newSource,
                                            scope.editor.getCursorPosition());
                      scope.editor.setReadOnly(false);
                      scope.restoreFileSession();
                    },
                    function(err) { scope.editor.setReadOnly(false); });
              }
            };

            // private
            scope.downloadRepoSnapshot = function() {
              if (scope.activeRepo) {
                // Force commit first
                scope.internalControl.commitPending();
                // Generate the zip
                var zipBlob = scope.activeRepo.toZipFile();
                var repoShortName =
                    scope.activeRepo.repoProto.repo_header.repo_name.split(
                        "/")[0];
                var date = new Date();
                var zipName = repoShortName + "-" +
                              date.getFullYear().toString().slice(-2) +
                              ("0" + date.getDate()).slice(-2) +
                              ("0" + (date.getMonth() + 1)).slice(-2) + '-' +
                              ("0" + date.getHours()).slice(-2) + '-' +
                              ("0" + date.getMinutes()).slice(-2) + '-' +
                              ("0" + date.getSeconds()).slice(-2) + '.zip';
                // Download it
                saveAs(zipBlob, zipName);
              }
            };

            // private
            // Unloads and loaded repo, commiting any changes to Crucible
            scope.unload = function() {
              scope.changeHandler = function(e) {};
              scope.saveFileSession();
              if (scope.activeRepo && !scope.editor.getReadOnly()) {
                // Commit any pending code
                scope.activeRepo.commit(scope.relativePath,
                                        scope.editor.getValue());
              }
              scope.activeRepo = null;
              scope.relativePath = null;
              scope.isLockedFile = false;
            };

            // private
            scope.saveFileSession = function() {
              if (!scope.activeRepo || !scope.relativePath) {
                return;
              }
              var fileSessionKey =
                  scope.activeRepo.repoProto.repo_header.repo_uuid +
                  scope.relativePath;
              scope.fileSessions[fileSessionKey] = {
                vScroll: scope.editor.session.getScrollTop(),
                hScroll: scope.editor.session.getScrollLeft(),
                cursorRow: scope.editor.selection.getCursor().row,
                cursorColumn: scope.editor.selection.getCursor().column
              };
            };

            scope.restoreFileSession = function() {
              scope.editor.focus();
              if (!scope.activeRepo || !scope.relativePath) {
                return;
              }
              var fileSessionKey =
                  scope.activeRepo.repoProto.repo_header.repo_uuid +
                  scope.relativePath;
              var session = scope.fileSessions[fileSessionKey];
              if (session) {
                scope.editor.session.setScrollTop(session.vScroll);
                scope.editor.session.setScrollLeft(session.hScroll);
                scope.editor.selection.moveCursorToPosition(
                    {row: session.cursorRow, column: session.cursorColumn});
              }
            };

          }
        };
      }
    ]);
