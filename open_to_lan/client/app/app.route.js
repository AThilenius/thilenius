// Copyright 2015 Alec Thilenius
// All rights reserved.
var app = angular.module('app');

app.config(['$stateProvider', '$urlRouterProvider', function($stateProvider,
      $urlRouterProvider) {
  $stateProvider
    .state('login', {
      url: '/login',
      templateUrl: 'app/components/login/login.html',
      controller: 'loginController'
    })
    .state('create_user', {
      url: '/create_user',
      templateUrl: 'app/components/create_user/create_user.html',
      controller: 'createUserController'
    })
    .state('create_club', {
      url: '/create_club',
      templateUrl: 'app/components/create_club/create_club.html',
      controller: 'createClubController'
    })
    .state('console', {
      url: '/console',
      templateUrl: 'app/components/console/console.html',
      controller: 'consoleController'
    });
  $urlRouterProvider.otherwise('console');
}]);
