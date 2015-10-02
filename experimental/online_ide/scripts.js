// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp', [
  'ngCookies',
  'thilenius.tree_view',
  'thilenius.content_window',
  'thilenius.file_explorer'
]);

forgeApp.controller('forgeController', [
  '$rootScope',
  '$scope',
  'sentinel',
  'crucible',
  function($rootScope, $scope, sentinel, crucible) {
    // Sentinel / Crucible
    $scope.sentinel = sentinel;
    $scope.crucible = crucible;

    // Sidebar
    $scope.activeSidebarTab = 'file';

    $scope.fileExplorerControl = {};
    $scope.contentWindowControl = {};
    $scope.historyExplorerControl = {};

    $scope.error = null;

    $rootScope.$on(
        'error', function(event, message) { $scope.error = message; });

    $rootScope.$on('crucible.repoAdded', function(event, repo) {
      $scope.fileExplorerControl.addRepo(repo);
    });

    $rootScope.$on(
        'fileExplorer.fileSelected', function(event, repo, relativePath) {
          $scope.contentWindowControl.bindRepoFileForEdit(repo, relativePath);
          $scope.historyExplorerControl.setRepoFile(repo, relativePath);
        });

    // Watch for sentinel loads
    $rootScope.$on('sentinel.login', function(event, token, user) {
      crucible.loadAllRepos();
    });
    $rootScope.$on(
        'sentinel.logout', function(event) { crucible.unloadAllRepos(); });

    // Finally let Sentinel load from cookie (if we have one)
    sentinel.tryLoadingFromCookie();
  }
]);
