var forgeApp = angular.module('forgeApp');

forgeApp.controller('loginController', [
  '$location',
  '$rootScope',
  '$scope',
  'sentinel',
  function($location, $rootScope, $scope, sentinel) {
    $rootScope.htmlClass = 'forge-body';
    $rootScope.bodyClass = 'forge-body';

    $scope.sentinel = sentinel;

    $rootScope.$on('sentinel.login', function(event, token, user) {
      if ($location.path() === '/login') {
        if ($scope.sentinel.lastUrl) {
          $scope.$apply(function() {
            $location.path($scope.sentinel.lastUrl);
            $scope.sentinel.lastUrl = null;
            $scope.error = null;
          });
        } else {
          $scope.$apply(function() { $location.path('/'); });
          $scope.error = null;
        }
      }
    });

    $rootScope.$on('sentinel.error', function(event, message) {
      $scope.$apply(function() {
        $scope.error = message;
        $scope.password = '';
      });
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
