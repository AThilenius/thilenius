// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module(
    'forgeApp',
    [ 'ngCookies', 'thilenius.tree_view', 'thilenius.content_window' ]);

forgeApp.controller('forgeController', [
  '$scope',
  'sentinel',
  'crucible',
  function($scope, sentinel, crucible) {
    // Editor
    //$scope.editor = ace.edit("editor");
    //$scope.editor.setTheme("ace/theme/twilight");
    //$scope.editor.getSession().setMode("ace/mode/javascript");

    // Sentinel / Crucible
    $scope.sentinel = sentinel;
    $scope.crucible = crucible;

    // Sidebar
    $scope.activeSidebarTab = 'file';
    $scope.fileTree = {
      selected : null,
      parents : []
    };
    $scope.historyTree = {};

    $scope.contentWindowControl = {};

    $scope.error = null;

    // Watch for Crucible errors
    crucible.on('error', function(error) {
      $scope.$apply(function() {
        $scope.error = error;
      });
    });

    // Watch for changes to sentinel, load a repo when it changes
    $scope.$watch(function(scope) { return sentinel.token; },
                  function(newValue, oldValue) {
                    crucible.on('repoAdded', function(repo) {
                      // Create a FileTree parent for it
                      var fileTreeParent = {
                        label : repo.repoProto.repo_header.repo_name,
                        children : []
                      };
                      for (var relativePath in repo.headState) {
                        fileTreeParent.children.push({
                          label : relativePath,
                          repo : repo,
                          source : repo.headState[relativePath]
                        });
                      }
                      $scope.$apply(function() {
                        $scope.fileTree.parents.push(fileTreeParent);
                      });
                    });
                    crucible.loadAllRepos();
                  });

    // File selected
    $scope.$watch('fileTree.selected', function(newVal, oldVal) {
      if (newVal) {
        $scope.contentWindowControl.bindRepoFileForEdit(newVal.repo,
                                                        newVal.label);
      }
    });

    // Login Method
    $scope.login = function(email, password) {
      sentinel.login(email,
                     password, function() {}, function() { $scope.$apply(); });
    };
  }
]);
