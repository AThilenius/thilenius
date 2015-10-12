var forgeApp = angular.module('forgeApp');

forgeApp.controller('loginController', [
  '$location',
  '$rootScope',
  '$scope',
  'sentinel',
  function($location, $rootScope, $scope, sentinel) {
    $scope.sentinel = sentinel;

    $rootScope.$on('sentinel.login', function(event, token, user) {
      $scope.$apply(function() { $location.path("/forge"); });
    });

    $rootScope.$on(
        'sentinel.logout', function(event) { $location.path('/login'); });

    $rootScope.$on('sentinel.error', function(event, message) {
      $scope.error = message;
      $scope.password = '';
      console.log("Sentinel failure: " + message);
    });

    $scope.createAccount = function() {
      sentinel.createAccount($scope.create.firstName, $scope.create.lastName,
                             $scope.create.email, $scope.create.password);
    };

    // Finally let Sentinel load from cookie (if we have one)
    sentinel.tryLoadingFromCookie();
  }
]);
