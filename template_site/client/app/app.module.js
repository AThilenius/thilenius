// Copyright 2015 Alec Thilenius
// All rights reserved.

var app = angular.module('app', [
  'angular-google-analytics',
  'angular-md5',
  'lbServices',
  'ngAnimate',
  'ngCookies',
  'ngMaterial',
  'ngMessages',
  'ui.router'
]);

var redirectReady = false;

/** @ngInject */
function appRun($rootScope, Analytics, $location, Person) {
  // Load current user if there is one
  $rootScope.currentUser = Person.getCurrent();
  $rootScope.currentUser.$promise.then(person => {
    redirectReady = true;
  });
  // Manually track Google Analytics pages
  $rootScope.$on('$stateChangeSuccess',
      (event, toState, toParams, fromState, fromParams) => {
        Analytics.trackPage($location.path() + toState.name.replace(/\./g, '/'),
            toState.name);
      });
};

app.run(appRun);

//==============================================================================
//  DEBUG
//==============================================================================
/** @ngInject */
function debugRun($q, Person) {
  window.q = $q;
  window.Person = Person;
};
app.run(debugRun);
//==============================================================================
//  /DEBUG
//==============================================================================

/** @ngInject */
function logoutHttpInterceptor($q, $rootScope, $location, LoopBackAuth) {
  return {
    responseError: rejection => {
      if (rejection.status === 401 && redirectReady) {
        console.log('401 Logout: ', rejection);
        // Clear the Loopback values from client browser for safe logout
        LoopBackAuth.clearUser();
        LoopBackAuth.clearStorage();
        $rootScope.currentUser = null;
        $location.nextAfterLogin = $location.path();
        $location.path('/login');
      }
      return $q.reject(rejection);
    }
  };
};

/** @ngInject */
function appConfigure($compileProvider, $httpProvider, AnalyticsProvider) {
  // Setup Google Anylitics
  AnalyticsProvider
  // Manually track pages (see appRun)
      .trackPages(false)
      .setAccount('UA-80572414-1')
      .setDomainName('none');

  // Allow 'tel' in href tags
  $compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|file|tel):/);
  // Register the logout on Invalid Authentication handler
  $httpProvider.interceptors.push(logoutHttpInterceptor);
};

app.config(appConfigure);
