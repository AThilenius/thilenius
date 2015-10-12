var forgeApp = angular.module('forgeApp');

forgeApp.config([
  '$routeProvider',
  function($routeProvider) {
    $routeProvider

        .when('/',
              {
                templateUrl: 'app/components/login/login.html',
                controller: 'loginController'
              })

        .when('/login',
              {
                templateUrl: 'app/components/login/login.html',
                controller: 'loginController'
              })

        .when('/forge', {
          templateUrl: 'app/components/forge/forge.html',
          controller: 'forgeController'
        });

  }
]);

// Set up automatic forwarding to login for all routes
forgeApp.run([
  '$rootScope',
  '$location',
  'sentinel',
  function($rootScope, $location, sentinel) {
    $rootScope.$on("$routeChangeStart", function(event, next, current) {
      if (!sentinel.user) {
        if (next.templateUrl == "app/components/login/login.html") {
          // already going to #login, no redirect needed
        } else {
          $location.path("/login");
        }
      }
    });
  }
]);
