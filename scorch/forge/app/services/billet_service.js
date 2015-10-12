// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp');

// Fires the events:
// billet.sessionCreated (::billet::proto::Session)
// billet.begin          ()
// billet.done           ()
// billet.compileBegin   ()
// billet.compileEnd     (::billet::proto::ApplicationOutput)
// billet.compileOutput  (::billet::proto::ApplicationOutput)
// billet.runBegin       ()
// billet.runEnd         (::billet::proto::ApplicationOutput)
// billet.runOutput      (::billet::proto::ApplicationOutput)
// billet.error          (String)
// error                 (String)
var billetService = function($rootScope) {
  this.$rootScope = $rootScope;
  var transport = new Thrift.Transport("/api/billet/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new BilletClient(protocol);
  this.billetSessionProto = null;

  // Error redirect
  $rootScope.$on('billet.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

billetService.prototype.createSession = function(sentinelToken) {
  if (!this.billetSessionProto) {
    var that = this;
    this.client.CreateSession(sentinelToken, null)
        .fail(this.firejqXhrErrorFactory())
        .done(function(result) {
          that.billetSessionProto = result;
          that.$rootScope.$broadcast('billet.sessionCreated', result);
        });
  }
};

billetService.prototype.runCMakeRepo = function(repoHeaderProto,
                                                stagedChangeListProtos) {
  if (!this.billetSessionProto) {
    this.$rootScope.$broadcast('billet.error', "No active sessions");
    return;
  }
  this.compileCMakeRepo(repoHeaderProto, stagedChangeListProtos);
};

billetService.prototype.clangFormat = function(source, callback, error) {
  var that = this;
  that.$rootScope.$broadcast('billet.begin');
  this.client.ClangFormat(source, null)
      .fail(function(jqXhr, stat, err) { error(err); })
      .done(function(result) {
        that.$rootScope.$broadcast('billet.done');
        callback(result);
      });
};

// private
billetService.prototype.compileCMakeRepo = function(repoHeaderProto,
                                                    stagedChangeListProtos) {
  var that = this;
  this.client.BuildCMakeRepo(this.billetSessionProto, repoHeaderProto,
                             stagedChangeListProtos, [/* App Args*/], null)
      .fail(this.firejqXhrErrorFactory())
      .done(function() {
        that.$rootScope.$broadcast('billet.begin');
        that.$rootScope.$broadcast('billet.compileBegin');
        var queryOutput = function(after_line) {
          that.client.QueryCompilerOutputAfterLine(that.billetSessionProto,
                                                   after_line, null)
              .fail(that.firejqXhrErrorFactory())
              .done(function(result) {
                if (result.did_terminate) {
                  // If compilation finished, then kick off running the app
                  that.$rootScope.$broadcast('billet.compileEnd', result);
                  // If compiling didn't fail
                  if (result.termination_code === 0) {
                    that.runExecutable();
                  } else {
                    that.$rootScope.$broadcast('billet.done', result);
                  }
                } else {
                  that.$rootScope.$broadcast('billet.compileOutput', result);
                  queryOutput(after_line + result.output_tokens.length);
                }
              });
        };
        queryOutput(0);
      });
};

// private
billetService.prototype.runExecutable = function() {
  var that = this;
  this.client.RunRepo(this.billetSessionProto, null)
      .fail(this.firejqXhrErrorFactory())
      .done(function() {
        that.$rootScope.$broadcast('billet.runBegin');
        var queryOutput = function(after_line) {
          that.client.QueryApplicationOutputAfterLine(that.billetSessionProto,
                                                      after_line, null)
              .fail(that.firejqXhrErrorFactory())
              .done(function(result) {
                if (result.did_terminate) {
                  that.$rootScope.$broadcast('billet.runEnd', result);
                  that.$rootScope.$broadcast('billet.done', result);
                } else {
                  that.$rootScope.$broadcast('billet.runOutput', result);
                  queryOutput(after_line + result.output_tokens.length);
                }
              });
        };
        queryOutput(0);
      });
};

// private
billetService.prototype.firejqXhrErrorFactory = function() {
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
  function($rootScope) { return new billetService($rootScope); }
]);
