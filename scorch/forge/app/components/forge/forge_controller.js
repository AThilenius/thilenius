var forgeApp = angular.module('forgeApp');

forgeApp.controller('forgeController', [
  '$rootScope',
  '$scope',
  '$location',
  'sentinel',
  'crucible',
  'billet',
  function($rootScope, $scope, $location, sentinel, crucible, billet) {
    // Forward the version on
    if (typeof FORGE_VERSION !== 'undefined') {
      $scope.forgeVersion = FORGE_VERSION;
    } else {
      $scope.forgeVersion = 'unknown';
    }

    // Will redirect to login is failed
    sentinel.tryLoadingFromCookie();

    // Sentinel / Crucible
    $scope.sentinel = sentinel;
    $scope.crucible = crucible;
    $scope.billet = billet;

    // Sidebar
    $scope.activeSidebarTab = 'file';

    $scope.fileExplorerControl = {};
    $scope.contentWindowControl = {};
    $scope.historyExplorerControl = {};
    $scope.anvilWindowControl = {};
    $scope.settingsWindowControl = {};

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
      if ($scope.fileExplorerControl.selected) {
        $scope.contentWindowControl.bindRepoFileForEdit(
            $scope.fileExplorerControl.selected.repo,
            $scope.fileExplorerControl.selected.relativePath);
      }
    };

    $rootScope.$on('error', function(event, message) {
      $scope.$apply(function() { $scope.error = message; });
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

    $rootScope.$on(
        'historyExplorer.snapshotSelected', function(event, repo, relativePath,
                                                     changeList) {
          if ($scope.activeSidebarTab === 'history') {
            $scope.contentWindowControl.bindRepoFileForView(repo, relativePath,
                                                            changeList);
          }
        });

    $rootScope.$on(
        'sentinel.logout', function(event) { crucible.unloadAllRepos(); });

    // Load up Crucible and Billet (Should be logged in by the time we get here)
    if (sentinel.token) {
      crucible.loadAllRepos();
      billet.init(sentinel.token);
    }
  }
]);
