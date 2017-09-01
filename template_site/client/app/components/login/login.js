'use strict';

angular
  .module('app')
  .controller('loginController', loginController);

/** @ngInject */
function loginController($rootScope, $location, $scope) {

  $scope.login = function() {
    $scope.error = null;
    $scope.loginCredentials.email =
      $scope.loginCredentials.email.toLowerCase();
    Person.login($scope.loginCredentials).$promise.then(response => {
      $rootScope.currentUser = Person.getCurrent();
      if ($location.nextAfterLogin) {
        $location.path($location.nextAfterLogin);
        $location.nextAfterLogin = null;
      } else {
        $scope.goToHome();
      }
    }, error => {
      $scope.error = error.data.error.code = 'LOGIN_FAILED' ?
        'Woops, wrong email or password' : error.data.error.message;
      $scope.loginCredentials.password = '';
    });
  };

};
