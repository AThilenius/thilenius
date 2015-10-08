// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.content_window', ['thilenius.console_window'])
    .directive('atContentWindow', [
      // TODO(athilenius): Think more if I want to use services directly in
      // directives. Might want to seperate that out.
      'billet',
      function(billet) {
        return {
          restrict : 'AE',
          scope : {control : '='},
          templateUrl : 'app/directives/content_window/content_window.htm',
          link : function(scope, iElement, iAttrs) {
            scope.editor = ace.edit("editor");
            scope.editor.setTheme("ace/theme/twilight");
            scope.editor.getSession().setMode("ace/mode/c_cpp");
            scope.editor.$blockScrolling = Infinity;

            scope.editorVisible = false;
            scope.activeRepo = null;
            scope.relativePath = null;

            scope.consoleWindowControl = {};

            // Expose a control object
            scope.internalControl = scope.control || {};

            scope.internalControl.commitPending = function() {
              if (scope.activeRepo && !scope.editor.getReadOnly()) {
                // Commit any pending code
                scope.activeRepo.commit(scope.relativePath,
                                        scope.editor.getValue());
              }
            };

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
              var stashedCl = scope.activeRepo.getPendingChangeList(
                  scope.relativePath, scope.editor.getValue());
              console.log("Running repo with stashed CL: " +
                          JSON.stringify(stashedCl, null, 2));
              if (stashedCl) {
                billet.runCMakeRepo(scope.activeRepo.repoProto.repo_header,
                                    [stashedCl]);
              } else {
                billet.runCMakeRepo(scope.activeRepo.repoProto.repo_header, []);
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
