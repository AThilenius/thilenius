var app = angular.module('app');

app.controller('createClubController', [
  '$rootScope',
  '$scope',
  '$location',
  'Person',
  'Club',
  function($rootScope, $scope, $location, Person, Club) {

    $scope.createClub = function(specs) {
      Person.clubs.create({
        id: Person.getCurrentId()
      }, specs, function(result) {
        $location.path('/list_clubs');
      });
    };

  }
]);
