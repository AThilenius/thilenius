var forgeApp = angular.module('forgeApp');

// configure our routes
forgeApp.config(function($routeProvider, $locationProvider) {
  $routeProvider

      // route for the home page
      .when('/',
            {templateUrl : 'app/components/home/home.html', controller : 'homeController'})

      // route for the about page
      .when('/about',
            {templateUrl : 'app/components/about/about.html', controller : 'aboutController'})

      // route for the contact page
      .when('/contact', {
        templateUrl : 'app/components/contact/contact.html',
        controller : 'contactController'
      });

      // use the HTML5 History API
      $locationProvider.html5Mode(true);
});

forgeApp.controller('aboutController', function($scope) {
  $scope.message = 'Look! I am an about page.';
});

forgeApp.controller('contactController', function($scope) {
  $scope.message = 'Contact us! JK. This is just a demo.';
});
