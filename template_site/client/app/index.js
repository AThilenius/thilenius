'use strict';

angular
  .module('app')
  .controller('indexController', indexController);

/** @ngInject */
function indexController($scope, $location, $state, $rootScope, Person,
  baseData) {

  $scope.navigate = function(link) {
    $location.path(link);
  };

  $scope.goToState = function(state) {
    $state.go(state);
  };

  $scope.goToHome = function() {
    $state.go('/');
  };

  $scope.logout = function() {
    Person.logout().$promise.finally(() => {
      $rootScope.currentUser = null;
      $scope.goToHome();
    });
  };

};
