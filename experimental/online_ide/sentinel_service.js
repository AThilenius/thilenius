var forgeApp = angular.module('forgeApp');

var FORGE_COOKIE_KEY = 'forge_session_token';

// Fires the events:
// sentinel.login  (::sentinel::Token, ::sentinel::User)
// sentinel.logout ()
// sentinel.error  (String)
// error           (String)
var SentinelService = function($rootScope, $cookies) {
  this.$rootScope = $rootScope;
  this.$cookies = $cookies;
  var transport = new Thrift.Transport("/api/sentinel/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new SentinelClient(protocol);
  this.token = null;
  this.user = null;
  // Error redirect
  $rootScope.$on('sentinel.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

SentinelService.prototype.login = function(login, password) {
  var that = this;
  that.client.CreateToken(login, password, function() {})
      .fail(that.firejqXhrErrorFactory())
      .done(function(result) {
        that.token = result;
        that.$cookies.put(FORGE_COOKIE_KEY, JSON.stringify(that.token));
        that.loadUserData();
      });
};

SentinelService.prototype.logout = function() {
  this.token = null;
  this.user = null;
  this.$cookies.put(FORGE_COOKIE_KEY, null);
  this.$rootScope.$broadcast('sentinel.logout');
};

SentinelService.prototype.tryLoadingFromCookie = function() {
  if (!this.token && !this.user && this.$cookies.get(FORGE_COOKIE_KEY) &&
      JSON.parse(this.$cookies.get(FORGE_COOKIE_KEY))) {
    this.token = jQuery.extend(new Token(),
                               JSON.parse(this.$cookies.get(FORGE_COOKIE_KEY)));
    // Validate the token
    var that = this;
    this.client.CheckToken(this.token, function() {})
        .fail(that.firejqXhrErrorFactory())
        .done(function(isValid) {
          if (isValid) {
            that.loadUserData();
          } else {
            that.token = null;
          }
        });
  }
};

// private
SentinelService.prototype.loadUserData = function() {
  // Don't load user data without a token or if it's already loaded
  if (!this.token || this.user) {
    return;
  }
  var user_partial = new User();
  user_partial.uuid = this.token.user_uuid;
  var that = this;
  this.client.FindUser(this.token, user_partial, function() {})
      .fail(that.firejqXhrErrorFactory())
      .done(function(user) {
        that.user = user;
        that.$rootScope.$broadcast('sentinel.login', that.token, that.user);
      });
};

// private
SentinelService.prototype.firejqXhrErrorFactory = function() {
  var that = this;
  return function(jqXhr, stat, error) {
    if (jqXhr && jqXhr.status === 0) {
      that.$rootScope.$broadcast('sentinel.error',
                                 'Status 0 | cannot connect to Sentinel');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.$rootScope.$broadcast('sentinel.error',
                                 'Status ' + stat + ' | ' + error.user_message);
    } else {
      that.$rootScope.$broadcast('sentinel.error',
                                 'Status ' + stat +
                                     ' | Something isnt right here...');
    }
  };
};

forgeApp.factory('sentinel', [
  '$rootScope',
  '$cookies',
  function($rootScope, $cookies) {
    return new SentinelService($rootScope, $cookies);
  }
]);
