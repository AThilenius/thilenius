var forgeApp = angular.module('forgeApp');

var FORGE_COOKIE_KEY = 'forge_session_token';

// Fires the events:
// sentinel.login  (::sentinel::Token, ::sentinel::User)
// sentinel.logout ()
// sentinel.error  (String)
// error           (String)
var SentinelService = function($rootScope, $cookies, $location) {
  this.$rootScope = $rootScope;
  this.$cookies = $cookies;
  this.$location = $location;
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
  that.client.CreateToken(login, password, null)
      .fail(that.firejqXhrErrorFactory())
      .done(function(result) {
        that.token = result;
        that.$cookies.put(FORGE_COOKIE_KEY, JSON.stringify(that.token));
        that.loadUserData();
      });
};

SentinelService.prototype.logout = function() {
  if (!this.lastUrl) {
    this.lastUrl = this.$location.path();
  }
  this.token = null;
  this.user = null;
  this.$cookies.put(FORGE_COOKIE_KEY, null);
  this.$rootScope.$broadcast('sentinel.logout');
  this.$location.path('/login');
};

SentinelService.prototype.createAccount = function(firstName, lastName, email,
                                                   password) {
  // Create a user partial
  var user = new User();
  user.first_name = firstName;
  user.last_name = lastName;
  user.email_address = email;
  var that = this;
  this.client.CreateUser(user, password, null)
      .fail(that.firejqXhrErrorFactory())
      .done(function(user) { that.login(email, password); });
};

SentinelService.prototype.tryLoadingFromCookie = function() {
  if (this.token && this.user) {
    // Already logged in
    return;
  }
  if (this.$cookies.get(FORGE_COOKIE_KEY) &&
      JSON.parse(this.$cookies.get(FORGE_COOKIE_KEY))) {
    this.token = jQuery.extend(new Token(),
                               JSON.parse(this.$cookies.get(FORGE_COOKIE_KEY)));
    // Validate the token
    var that = this;
    this.client.CheckToken(this.token, null)
        .fail(function(jqXhr, stat, error) {
          // Jump to login screen
          that.logout();
        })
        .done(function(isValid) {
          if (isValid) {
            that.loadUserData();
          } else {
            that.token = null;
          }
        });
  } else {
    this.logout();
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
                                 'Cannot connect to Sentinel');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.$rootScope.$broadcast('sentinel.error',
                                 error.user_message);
    } else {
      that.$rootScope.$broadcast(
          'sentinel.error', 'Something isnt right here...');
    }
  };
};

forgeApp.factory('sentinel', [
  '$rootScope',
  '$cookies',
  '$location',
  function($rootScope, $cookies, $location) {
    return new SentinelService($rootScope, $cookies, $location);
  }
]);
