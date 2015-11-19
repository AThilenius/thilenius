var app = angular.module('app');

app.controller('loginController', [
  '$location',
  '$rootScope',
  '$scope',
  'Person',
  function($location, $rootScope, $scope, Person) {

    $scope.login = function() {
      $scope.loginCredentials.email =
          $scope.loginCredentials.email.toLowerCase();
      Person.login($scope.loginCredentials, function(result) {
        var next = $location.nextAfterLogin || '/club';
        location.nextAfterLogin = null;
        $location.path(next);
      });
    };

  }
]);
