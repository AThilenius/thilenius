// Copyright 2015 Alec Thilenius
// All rights reserved.
var app = angular.module('app', [
  'lbServices',
  'ngCookies',
  'ui.router',
  'xeditable',
  'thilenius.club_dashboard',
  'thilenius.navbar',
  'thilenius.world_dashboard'
]);

app.run(function(editableOptions) { editableOptions.theme = 'bs3'; });

app.config([
  '$httpProvider',
  function($httpProvider) {
    $httpProvider.interceptors.push(function($q, $location, LoopBackAuth) {
      return {
        responseError: function(rejection) {
          if (rejection.status === 401) {
            // Now clearing the loopback values from client browser for safe
            // logout...
            LoopBackAuth.clearUser();
            LoopBackAuth.clearStorage();
            $location.nextAfterLogin = $location.path();
            $location.path('/login');
          }
          return $q.reject(rejection);
        }
      };
    });
  }
]);
