var forgeApp = angular.module('app');

forgeApp.controller('forgeController', [
  '$rootScope',
  '$scope',
  '$location',
  'Person',
  'Project',
  'SourceFile',
  function($rootScope, $scope, $location, Person, Project, SourceFile) {

    window.Person = Person;
    window.Project = Project;
    window.SourceFile = SourceFile;

    // Global state object (not intended for serialization)
    $scope.state = {
      viewingAsRole: 'student'
    };

    // Forward the version on
    if (typeof FORGE_VERSION !== 'undefined') {
      $scope.forgeVersion = FORGE_VERSION;
    } else {
      $scope.forgeVersion = 'unknown';
    }

    $scope.projects = Person.projects({
      id: Person.getCurrentId(),
      filter: {include: {relation: 'sourceFiles'}}
    });

    $scope.person = Person.getCurrent(function(person) {
      $scope.state.viewingAsRole = person.role;
    });

    //// Will redirect to login is failed
    // sentinel.tryLoadingFromCookie();

    //// Sentinel / Crucible
    //$scope.sentinel = sentinel;
    //$scope.crucible = crucible;
    //$scope.billet = billet;

    //// Sidebar
    //$scope.activeSidebarTab = 'file';

    //$scope.fileExplorerControl = {};
    //$scope.contentWindowControl = {};
    //$scope.historyExplorerControl = {};
    //$scope.anvilWindowControl = {};
    //$scope.settingsWindowControl = {};
    //$scope.alertWindowControl = {};

    //$scope.displayError = null;

    //$scope.buildHistory = function() {
    //$scope.contentWindowControl.commitPending();
    // if ($scope.fileExplorerControl.selected) {
    //$scope.historyExplorerControl.setRepoFile(
    //$scope.fileExplorerControl.selected.repo,
    //$scope.fileExplorerControl.selected.relativePath);
    //}
    //};

    //$scope.editOn = function() {
    // if ($scope.fileExplorerControl.selected) {
    //$scope.contentWindowControl.bindFile(
    //$scope.fileExplorerControl.selected.repo,
    //$scope.fileExplorerControl.selected.relativePath, null, false);
    //}
    //};

    //// Watch for alerts and set alert-eplorer active if there are any
    //$rootScope.$on('billet.alerts', function(event, alerts) {
    // if (alerts.length > 0) {
    //$scope.$apply(function() { $scope.activeSidebarTab = 'alerts'; });
    //}
    //});


    //$rootScope.$on('error', function(event, message) {
    //$scope.$apply(function() { $scope.displayError = message; });
    //});

    //$rootScope.$on('crucible.repoAdded', function(event, repo) {
    //$scope.fileExplorerControl.addRepo(repo);
    //});

    //$rootScope.$on(
    //'fileExplorer.fileSelected', function(event, repo, relativePath) {
    //$scope.contentWindowControl.bindFile(repo, relativePath, null, false);
    //$scope.contentWindowControl.billet = billet;
    //$scope.autoFormat = function() {
    //$scope.contentWindowControl.formatCode();
    //};
    //});

    //$rootScope.$on(
    //'historyExplorer.snapshotSelected', function(event, repo, relativePath,
    // changeList) {
    // if ($scope.activeSidebarTab === 'history') {
    //$scope.contentWindowControl.bindFile(
    // repo, relativePath, changeList.change_list_uuid, true);
    //}
    //});

    //$rootScope.$on('alertExplorer.jumpToAlert', function(event, alert) {
    //// Try to find the repo in crucible
    // for (var i = 0; crucible.repos && i < crucible.repos.length; i++) {
    // var repo = crucible.repos[i];
    // if (repo.repoProto.repo_header.repo_uuid === alert.repoUuid) {
    //// Found it
    //$scope.contentWindowControl.bindFile(repo, alert.file, null, false);
    //$scope.contentWindowControl.jumpTo(alert.row, alert.column);
    //}
    //}
    //});

    //$rootScope.$on(
    //'sentinel.logout', function(event) { crucible.unloadAllRepos(); });

    //// Load up Crucible and Billet (Should be logged in by the time we get
    /// here)
    // if (sentinel.token) {
    // crucible.loadAllRepos();
    // billet.init(sentinel.token);
    //}
  }
]);
