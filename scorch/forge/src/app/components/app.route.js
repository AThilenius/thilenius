var forgeApp = angular.module('forgeApp');

// configure our routes
forgeApp.config(function($routeProvider) {
  $routeProvider

      .when('/',
            {
              templateUrl: 'app/components/home/home.html',
              controller: 'homeController'
            })

      .when('/login',
            {
              templateUrl: 'app/components/account/login.html',
              controller: 'loginController'
            })

      .when('/list_repos',
            {
              templateUrl: 'app/components/repo/list_repos.html',
              controller: 'repoController'
            })

      .when('/contact', {
        templateUrl: 'app/components/contact/contact.html',
        controller: 'contactController'
      });
});

forgeApp.controller('contactController', function($scope) {
  $scope.message = 'Contact us! JK. This is just a demo.';
});
