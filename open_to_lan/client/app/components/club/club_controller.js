var app = angular.module('app');

app.controller('clubController', [
  '$rootScope',
  '$scope',
  'Person',
  'Club',
  function($rootScope, $scope, Person, Club) {
    $scope.clubs = Person.clubs({
      id: Person.getCurrentId()
    });
  }
]);
