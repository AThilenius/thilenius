var forgeApp = angular.module('forgeApp');

// Fires the events:
// fiber.error  (String)
// error        (String)
var FiberService = function($rootScope, sentinel) {
  this.$rootScope = $rootScope;
  this.sentinel = sentinel;
  var transport = new Thrift.Transport("/api/fiber/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new FiberClient(protocol);

  // Error redirect
  $rootScope.$on('fiber.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

// Simple helper to return a CordStream from an existing cord
FiberService.prototype.fromCord = function(existingCord) {
  return new CordStream(this.client, existingCord);
};

// Returns a new CordStream
FiberService.prototype.createCord = function(name, done, fail) {
  var that = this;
  that.client.CreateCord(that.sentinel.token, name, null)
      .fail(function(jqXhr, stat, error) {
        if (fail) {
          fail(error);
        }
      })
      .done(function(result) {
        done(new CordStream(that.client, result));
      });
};

// Returns a new CordStream
FiberService.prototype.getCord = function(cordUuid, done, fail) {
  var that = this;
  that.client.GetCord(cordUuid, null)
      .fail(function(jqXhr, stat, error) {
        if (fail) {
          fail(error);
        }
      })
      .done(function(result) {
        done(new CordStream(that.client, result));
      });
};

forgeApp.factory('fiber', [
  '$rootScope',
  'sentinel',
  function($rootScope, sentinel) {
    return new FiberService($rootScope, sentinel);
  }
]);
