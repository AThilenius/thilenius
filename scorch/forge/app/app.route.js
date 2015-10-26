var forgeApp = angular.module('forgeApp');

forgeApp.config([
  '$routeProvider',
  function($routeProvider) {
    $routeProvider
        .when('/',
              {
                templateUrl: 'app/components/forge/forge.html',
                controller: 'forgeController'
              })
        .when('/login',
              {
                templateUrl: 'app/components/login/login.html',
                controller: 'loginController'
              })
        .when('/fiber/:uuid',
              {
                templateUrl: 'app/components/fiber/fiber.html',
                controller: 'fiberController'
              })
        .when('/crucible/:repoUuid/:clUuid',
              {
                templateUrl: 'app/components/crucible/crucible.html',
                controller: 'crucibleController'
              })
        .otherwise({redirectTo: '/'});
  }
]);

//forgeApp.run([
  //'$location',
  //'$rootScope',
  //'sentinel',
  //function($location, $rootScope, sentinel) {
    //$rootScope.$on(
        //'$routeChangeStart', function(event, nextRoute, currentRoute) {
          //$rootScope.previousState = $location.path();
          //// if login required and you're logged out, capture the current path
          ////if (nextRoute.loginRequired && !sentinel.token) {
            ////$rootScope.postLogInRoute = $location.path();
            ////console.log("Saving route: " + $location.path());
            ////$location.path('/login').replace();
          ////} else if ($rootScope.postLogInRoute && sentinel.token) {
            ////// once logged in, redirect to the last route and reset it
            ////console.log("Restoring route: " + $rootScope.postLogInRoute);
            ////$location.path($rootScope.postLogInRoute).replace();
            ////$rootScope.postLogInRoute = null;
          ////}
        //});
  //}
//]);
