var app = angular.module('app');

var DefaultsService = function($rootScope) {
  this.$rootScope = $rootScope;
};

DefaultsService.prototype.fromCord = function(existingCord) {
  return new CordStream(this.client, existingCord);
};

app.factory('defaults', [
  '$rootScope',
  function($rootScope) {
    return new DefaultsService($rootScope);
  }
]);
