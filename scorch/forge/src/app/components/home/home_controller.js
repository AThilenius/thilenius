// create the controller and inject Angular's $scope
forgeApp.controller('homeController', [
  '$scope',
  'user',
  function($scope, user) {
    $scope.user = user;
  }
]);
