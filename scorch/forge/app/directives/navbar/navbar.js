// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.navbar', [])
    .directive('atNavbar', [
      '$rootScope',
      function($rootScope) {
        return {
          restrict: 'AE',
          scope: {control: '='},
          templateUrl: 'app/directives/navbar/navbar.htm',
          link: function(scope, iElement, iAttrs) {}
        };
      }
    ]);
