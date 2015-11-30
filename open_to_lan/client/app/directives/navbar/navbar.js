// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.navbar', [])
    .directive('atNavbar', [
      '$rootScope',
      '$location',
      'Person',
      function($rootScope, $location, Person) {
        return {
          templateUrl: 'app/directives/navbar/navbar.htm',
          link: function($scope, iElement, iAttrs) {
            $scope.clubs = Person.clubs({id: Person.getCurrentId()});

            $scope.selectClub = function(club) { $scope.activeClub = club; };

            $scope.createClub = function() {
              $scope.activeClub = Person.clubs.create(
                  {id: Person.getCurrentId()}, {name: 'Unnamed', members: []});
              $scope.clubs.push($scope.activeClub);
            };

            $scope.logout = function() {
              Person.logout();
              $location.path('/login');
            };

          }
        };
      }
    ]);
