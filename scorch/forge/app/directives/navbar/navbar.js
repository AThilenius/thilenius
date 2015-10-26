// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.navbar', [])
    .directive('atNavbar', [
      '$rootScope',
      'sentinel',
      function($rootScope, sentinel) {
        return {
          restrict: 'AE',
          scope: {control: '='},
          templateUrl: 'app/directives/navbar/navbar.htm',
          link: function(scope, iElement, iAttrs) {
            scope.sentinel = sentinel;
          }
        };
      }
    ]);
