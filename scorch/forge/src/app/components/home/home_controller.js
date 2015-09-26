// create the controller and inject Angular's $scope
forgeApp.controller('homeController', [
  '$scope',
  'session',
  function($scope, session) {
    $scope.session = session;
  }
]);
