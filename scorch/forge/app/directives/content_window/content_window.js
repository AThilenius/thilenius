// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.content_window', ['thilenius.console_window'])
    .directive('atContentWindow', [
      '$rootScope',
      '$timeout',
      'billet',
      function($rootScope, $timeout, billet) {
        return {
          restrict : 'AE',
          scope : {control : '=', editor : '='},
          templateUrl : 'app/directives/content_window/content_window.htm',
          link : function(scope, iElement, iAttrs) {
            scope.editor = ace.edit("editor");
            scope.editor.setTheme("ace/theme/twilight");
            scope.editor.getSession().setMode("ace/mode/c_cpp");
            scope.editor.getSession().setTabSize(2);
            scope.editor.$blockScrolling = Infinity;
            scope.editor.setOptions({
              enableBasicAutocompletion : true,
              enableSnippets : true,
              enableLiveAutocompletion : true
            });

            scope.editorVisible = false;
            scope.activeRepo = null;
            scope.relativePath = null;
            scope.showConsole = false;
            scope.canRun = true;

            scope.consoleWindowControl = {};

            scope.internalControl = scope.control || {};

            $rootScope.$watchCollection(
                'editorSettings', function(settings, oldVal) {
                  if (settings) {
                    scope.editor.setOption("foldStyle", settings.foldStyle);
                    scope.editor.setOption("fontSize", settings.fontSize);
                    scope.editor.setOption("selectionStyle",
                                           settings.fullLineSelection ? 'line'
                                                                      : 'text');
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

            scope.internalControl.commitPending = function() {
              if (scope.activeRepo && !scope.editor.getReadOnly()) {
                // Commit any pending code
                scope.activeRepo.commit(scope.relativePath,
                                        scope.editor.getValue());
              }
            };

            // Watch for changes to console size, so we can refresh the editor
            // size
            scope.$watch('showConsole', function(newVal, oldVal) {
              $timeout(function() { scope.editor.resize(); }, 0);
            });

            // Watch for billet begin-done for run flag
            $rootScope.$on(
                'billet.begin', function(event) { scope.canRun = false; });

            $rootScope.$on('billet.done', function(event) {
              if (!scope.$$phase) {
                scope.$apply(function() { scope.canRun = true; });
              }
            });

            // Binds a file from a Repo for edit. Also stashes and commits
            // changes
            scope.internalControl.bindRepoFileForEdit = function(repo,
                                                                 relativePath) {
              scope.unload();
              scope.lastStash = new Date();
              scope.activeRepo = repo;
              scope.relativePath = relativePath;
              scope.editor.setValue(repo.headState[relativePath].source, 1);
              scope.editorVisible = true;
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
            };

            // private
            scope.run = function() {
              if (scope.activeRepo) {
                var stashedCl = scope.activeRepo.getPendingChangeList(
                    scope.relativePath, scope.editor.getValue());
                if (stashedCl) {
                  billet.runCMakeRepo(scope.activeRepo.repoProto.repo_header,
                                      [stashedCl]);
                } else {
                  billet.runCMakeRepo(scope.activeRepo.repoProto.repo_header,
                                      []);
                }
              }
            };

            // private
            scope.autoFormat = function() {
              if (scope.activeRepo) {
                if (scope.editor.getReadOnly()) {
                  // Don't format read only
                  return;
                }
                scope.editor.setReadOnly(true);
                billet.clangFormat(
                    scope.editor.getValue(),
                    function(newSource) {
                      scope.editor.setValue(newSource,
                                            scope.editor.getCursorPosition());
                      scope.editor.setReadOnly(false);
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
                // Download it
                saveAs(zipBlob,
                       scope.activeRepo.repoProto.repo_header.repo_name);
              }
            };

            // private
            // Unloads and loaded repo, commiting any changes to Crucible
            scope.unload = function() {
              scope.changeHandler = function(e) {};
              if (scope.activeRepo && !scope.editor.getReadOnly()) {
                // Commit any pending code
                scope.activeRepo.commit(scope.relativePath,
                                        scope.editor.getValue());
              }
              scope.activeRepo = null;
              scope.relativePath = null;
            };

          }
        };
      }
    ]);
