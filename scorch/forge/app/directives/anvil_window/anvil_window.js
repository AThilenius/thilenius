// Copyright 2015 Alec Thilenius
// All rights reserved.

// Fires the events:
angular.module('thilenius.anvil_window', [])
    .directive('atAnvilWindow', [
      '$rootScope',
      function($rootScope) {
        return {
          restrict: 'AE',
          scope: {control: '='},
          templateUrl: 'app/directives/anvil_window/anvil_window.htm',
          link: function(scope, iElement, iAttrs) {
            // Expose a control object
            scope.internalControl = scope.control || {};

          }
        };
      }
    ]);
