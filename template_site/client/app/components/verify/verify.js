'use strict';

angular
  .module('app')
  .controller('verifyController', verifyController);

/** @ngInject */
function verifyController($scope, $rootScope, $location, Person) {

  $scope.error = null;

  Person.confirm({
    uid: $location.search().uid,
    token: $location.search().token
  }).$promise.then(response => {
    $location.path('/login');
  }, err => $scope.error = err.data.error.message);

};
