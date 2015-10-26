// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp');

// Fires the events:
// blaze.account_set     (String)
// blaze.action_result   (Object)
// blaze.error           (String)
// error                 (String)
var blazeService = function($rootScope) {
  this.$rootScope = $rootScope;
  var transport = new Thrift.Transport("/api/blaze/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new BlazeClient(protocol);
  // Error redirect
  $rootScope.$on('blaze.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

blazeService.prototype.setMinecraftAccount = function(sentinelToken,
                                                      minecraftUsername) {
  var that = this;
  this.client.SetMinecraftAccount(sentinelToken, minecraftUsername, null)
      .fail(this.firejqXhrErrorFactory())
      .done(function(result) {
        that.$rootScope.$broadcast('blaze.account_set', minecraftUsername);
      });
};

blazeService.prototype.actionTargetInvoke = function(sentinelToken, worldId,
                                                     command, argsJsonArray) {
  var args = [worldId, command];
  args = args.concat(argsJsonArray);
  // Stringy any objects
  for (var i = 0; i < args.length; i++) {
    var arg = args[i];
    if (typeof arg !== "string") {
      args[i] = JSON.stringify(arg);
    }
  }
  var that = this;
  this.client.ProcessBlazeRequest(sentinelToken, 'action_target_invoke', args,
                                  null)
      .fail(this.firejqXhrErrorFactory())
      .done(function(result) {
        that.$rootScope.$broadcast('blaze.action_result', result);
      });
};

// private
blazeService.prototype.firejqXhrErrorFactory = function() {
  var that = this;
  return function(jqXhr, stat, error) {
    if (jqXhr && jqXhr.status === 0) {
      that.$rootScope.$broadcast('blaze.error',
                                 'Status 0 | cannot connect to blaze');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.$rootScope.$broadcast('blaze.error',
                                 'Status ' + stat + ' | ' + error.user_message);
    } else {
      that.$rootScope.$broadcast(
          'blaze.error', 'Status ' + stat + ' | Something isnt right here...');
    }
  };
};

forgeApp.factory('blaze', [
  '$rootScope',
  function($rootScope) { return new blazeService($rootScope); }
]);
