'use strict';

// NOTE!! If you change routes you also need to update /server/middleware
angular
    .module('app')
    .config(angularAppConfig);

/** @ngInject */
function angularAppConfig(
    $stateProvider, $urlRouterProvider, $locationProvider) {
  $stateProvider.state('/', {
        url: '/',
        templateUrl: 'app/components/home/home.html',
        controller: 'homeController'
      })
      .state('login', {
        url: '/login',
        templateUrl: 'app/components/login/login.html',
        controller: 'loginController'
      })
      .state('createLogin', {
        url: '/create_login',
        templateUrl: 'app/components/create_login/create_login.html',
        controller: 'createLoginController'
      })
      .state('verify', {
        url: '/verify',
        templateUrl: 'app/components/verify/verify.html',
        controller: 'verifyController'
      })
      .state('policy', {
        url: '/policy',
        templateUrl: 'app/components/policy/policy.html',
        controller: 'policyController'
      });
  $urlRouterProvider.otherwise('/');

  $locationProvider.html5Mode({
    enabled: true,
    requireBase: false
  });
};
