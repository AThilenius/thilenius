// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp');

// Fires the events:
// billet.oldCord        (::fiber::proto::Cord)
// billet.activeCord     (::fiber::proto::Cord)
// billet.error          (String)
// error                 (String)
var BilletService = function($rootScope, fiber) {
  this.$rootScope = $rootScope;
  this.fiber = fiber;
  var transport = new Thrift.Transport("/api/billet/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new BilletClient(protocol);
  this.isOldCordRunning = false;
  this.currentCord = null;

  // DO NOT COMMIT
  document.billet = this;

  // Error redirect
  $rootScope.$on('billet.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

BilletService.prototype.init = function(token) {
  this.token = token;
  var that = this;
  this.client.GetSessionStatus(token, null)
      .fail(function(jqXhr, stat, err) {
        // Ignore errors because this isn't mission critical
        console.log("Billet error: " + JSON.stringify(err));
      })
      .done(function(result) {
        that.isOldCordRunning = result.is_running;
        if (result.current_or_last_cord &&
            result.current_or_last_cord.uuid !== '') {
          console.log("Streaming old Fiber Cord: " +
                      result.current_or_last_cord.uuid);
          var cordStream = that.fiber.fromCord(result.current_or_last_cord);
          that.currentCord = cordStream;
          that.$rootScope.$broadcast(
              result.is_running ? 'billet.activeCord' : 'billet.oldCord',
              cordStream);
          // TODO(athilenius): Watch Fiber and wait for exit
        }
      });
};

BilletService.prototype.runCMakeRepo = function(repoHeaderProto) {
  var url = window.location.origin + "#/crucible/" + repoHeaderProto.repo_uuid +
            "/" + repoHeaderProto.latest_change_list_uuid;
  // Wow, that's fuck ugly
  var echo_repo_command = 'echo \"Synced to Crucible CL: <a href=\'' + url +
                          '\' target=\'_blank\'>' + url + '</a>\"';
  var command = echo_repo_command +
      '&& mkdir --parents build && cd build && cmake .. && make && ./runnable';
  if (!this.token) {
    this.$rootScope.$broadcast('billet.error', "No active token");
    return;
  }
  var that = this;
  this.client.SyncAndExec(this.token, repoHeaderProto, command, null)
      .fail(function(jqXhr, stat, err) {
        console.log("Billet error: " + JSON.stringify(err));
      })
      .done(function(result) {
        console.log("Billet result: " + JSON.stringify(result));
        var cordStream = that.fiber.fromCord(result);
        that.currentCord = cordStream;
        that.$rootScope.$broadcast('billet.activeCord', cordStream);
      });
};

BilletService.prototype.terminateSession = function() {
  var that = this;
  this.client.TerminateSession(this.token, null)
      .fail(function(jqXhr, stat, err) {
        // Ignore termination errors
        console.log("Billet TerminateSession error: " + JSON.stringify(err));
      })
      .done(function() {
        // Nothing to do. We just wait for the Fiber Cord to close
      });
};

BilletService.prototype.clean = function(repoHeaderProto) {
  var command = 'rm -rf build/';
  if (!this.token) {
    this.$rootScope.$broadcast('billet.error', "No active token");
    return;
  }
  var that = this;
  this.client.SyncAndExec(this.token, repoHeaderProto, command, null)
      .fail(function(jqXhr, stat, err) {
        console.log("Billet error: " + JSON.stringify(err));
      })
      .done(function(result) {
        console.log("Billet result: " + JSON.stringify(result));
        var cordStream = that.fiber.fromCord(result);
        that.currentCord = cordStream;
        that.$rootScope.$broadcast('billet.activeCord', cordStream);
      });
};

BilletService.prototype.clangFormat = function(source, callback, error) {
  // TODO(athilenius): Super race condtion here, bro.
  var that = this;
  that.$rootScope.$broadcast('billet.begin');
  this.client.ClangFormat(source, null)
      .fail(function(jqXhr, stat, err) { error(err); })
      .done(function(result) { callback(result); });
};

// private
BilletService.prototype.firejqXhrErrorFactory = function() {
  var that = this;
  return function(jqXhr, stat, error) {
    if (jqXhr && jqXhr.status === 0) {
      that.$rootScope.$broadcast('billet.error',
                                 'Status 0 | cannot connect to billet');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.$rootScope.$broadcast('billet.error',
                                 'Status ' + stat + ' | ' + error.user_message);
    } else {
      that.$rootScope.$broadcast(
          'billet.error', 'Status ' + stat + ' | Something isnt right here...');
    }
  };
};

forgeApp.factory('billet', [
  '$rootScope',
  'fiber',
  function($rootScope, fiber) { return new BilletService($rootScope, fiber); }
]);
