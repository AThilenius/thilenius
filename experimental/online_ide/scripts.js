// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp', [
  'ngCookies',
  'ngRoute',
  'thilenius.tree_view',
  'thilenius.content_window',
  'thilenius.file_explorer',
  'thilenius.history_explorer'
]);

forgeApp.config(function($routeProvider) {
  $routeProvider

      .when('/',
            {
              templateUrl: 'forge.html',
              controller: 'forgeController'
            })

      .when('/login',
            {
              templateUrl: 'login.html',
              controller: 'loginController'
            });

      //.when('/list_repos',
            //{
              //templateUrl: 'app/components/repo/list_repos.html',
              //controller: 'repoController'
            //})

      //.when('/contact', {
        //templateUrl: 'app/components/contact/contact.html',
        //controller: 'contactController'
      //});
});

forgeApp.controller('loginControll', [function(){}]);

forgeApp.controller('forgeController', [
  '$rootScope',
  '$scope',
  'sentinel',
  'crucible',
  'billet',
  function($rootScope, $scope, sentinel, crucible, billet) {
    // Sentinel / Crucible
    $scope.sentinel = sentinel;
    $scope.crucible = crucible;
    $scope.billet = billet;

    // Sidebar
    $scope.activeSidebarTab = 'file';

    $scope.fileExplorerControl = {};
    $scope.contentWindowControl = {};
    $scope.historyExplorerControl = {};

    $scope.error = null;

    $scope.buildHistory = function() {
      $scope.contentWindowControl.commitPending();
      if ($scope.fileExplorerControl.selected) {
        $scope.historyExplorerControl.setRepoFile(
            $scope.fileExplorerControl.selected.repo,
            $scope.fileExplorerControl.selected.relativePath);
      }
    };

    $scope.editOn = function() {
      $scope.contentWindowControl.bindRepoFileForEdit(
          $scope.fileExplorerControl.selected.repo,
          $scope.fileExplorerControl.selected.relativePath);
    };

    $scope.run = function() {
      console.log("Running repo: " +
                  $scope.fileExplorerControl.selected.repo.repoProto.repo_header
                      .repo_uuid);
      billet.runCMakeRepo(
          $scope.fileExplorerControl.selected.repo.repoProto.repo_header, []);
    };

    $rootScope.$on('error', function(event, message) {
      $scope.$apply(function() {
        $scope.error = message;
      });
    });

    $rootScope.$on('crucible.repoAdded', function(event, repo) {
      $scope.fileExplorerControl.addRepo(repo);
    });

    $rootScope.$on(
        'fileExplorer.fileSelected', function(event, repo, relativePath) {
          $scope.contentWindowControl.bindRepoFileForEdit(repo, relativePath);
          $scope.contentWindowControl.billet = billet;
          $scope.autoFormat = function() {
            $scope.contentWindowControl.formatCode();
          };
        });

    $rootScope.$on('historyExplorer.snapshotSelected', function(event, repo,
                                                                relativePath,
                                                                changeList) {
      if ($scope.activeSidebarTab === 'history') {
        $scope.contentWindowControl.bindRepoFileForView(repo, relativePath,
                                                        changeList);
      }
    });

    // Watch for sentinel loads
    $rootScope.$on('sentinel.login', function(event, token, user) {
      crucible.loadAllRepos();
      billet.createSession(token);
    });
    $rootScope.$on(
        'sentinel.logout', function(event) { crucible.unloadAllRepos(); });

    // Finally let Sentinel load from cookie (if we have one)
    sentinel.tryLoadingFromCookie();
  }
]);
