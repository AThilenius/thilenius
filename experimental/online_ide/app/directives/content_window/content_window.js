// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.content_window', [])
    .directive('atContentWindow', function() {
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

          // Expose a control object
          scope.internalControl = scope.control || {};

          // Binds a file from a Repo for edit. Also stashes and commits changes
          scope.internalControl.bindRepoFileForEdit = function(repo,
                                                               relativePath) {
            scope.lastStash = new Date();
            scope.editor.on("change", function(e) {
              // Commit at max once every 1000ms
              //if (new Date() - scope.lastStash >= 1000) {
                scope.lastStash = new Date();
                repo.commit(relativePath,
                            scope.editor.getValue());
              //}
            });
            scope.activeRepo = repo;
            scope.relativePath = relativePath;
            scope.editor.setValue(repo.headState[relativePath].source, 1);
            scope.editorVisible = true;
          };

          // Binds a file from a Repo for view (read only).
          scope.internalControl.bindRepoFileForView = function(repo,
                                                               relativePath) {

          };

          // private
          // Unloads and loaded repo, commiting any changes to Crucible
          scope.unload = function() {
            if (scope.activeRepo) {
              // Unattach editor hooks
              scope.editor.on("change", function(e) {});
            }
          };

        }
      };
    });