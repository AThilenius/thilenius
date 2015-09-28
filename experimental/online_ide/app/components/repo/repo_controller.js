forgeApp.controller('repoController', [
  '$scope',
  'session',
  'crucible',
  function($scope, session, crucible) {
    $scope.session = session;
  }
]);
