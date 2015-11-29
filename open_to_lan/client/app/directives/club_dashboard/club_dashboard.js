// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.club_dashboard', [])
    .directive('atClubDashboard', [
      '$rootScope',
      'Club',
      function($rootScope, Club) {
        return {
          templateUrl: 'app/directives/club_dashboard/club_dashboard.htm',
          link: function($scope, iElement, iAttrs) {
            $scope.updateClub = function() { $scope.activeClub.$save(); };
          }
        };
      }
    ]);
