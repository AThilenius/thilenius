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
    .state('club', {
      url: '/club',
      templateUrl: 'app/components/club/club.html',
      controller: 'clubController'
    });
  $urlRouterProvider.otherwise('club');
}]);
