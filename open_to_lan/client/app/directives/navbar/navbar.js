// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.navbar', [])
    .directive('atNavbar', [
      '$rootScope',
      'Person',
      function($rootScope, Person) {
        return {
          templateUrl: 'app/directives/navbar/navbar.htm',
          link: function(scope, iElement, iAttrs) {
            scope.clubs = Person.clubs({id: Person.getCurrentId()});
            scope.selectClub = function(club) {
              scope.activeClub = club;
            };
          }
        };
      }
    ]);
