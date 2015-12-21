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
    .state('forge', {
      url: '/forge',
      templateUrl: 'app/components/forge/forge.html',
      controller: 'forgeController'
    });
  $urlRouterProvider.otherwise('forge');
}]);
