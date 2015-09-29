var forgeApp = angular.module('forgeApp');

var FORGE_COOKIE_KEY = 'forge_session_token';

var SentinelService = function($cookies) {
  this.$cookies = $cookies;
  var transport = new Thrift.Transport("/api/sentinel/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new SentinelClient(protocol);
  this.token = null;
  if ($cookies.get(FORGE_COOKIE_KEY) &&
      JSON.parse($cookies.get(FORGE_COOKIE_KEY))) {
    this.token =
        jQuery.extend(new Token(), JSON.parse($cookies.get(FORGE_COOKIE_KEY)));
  }
  // Load user if we alrady have a token
  if (this.token) {
    console.log("Loaded token from cookies: " +
                JSON.stringify(this.token, null, 2));
    this.loadUserData(function() {}, function() {});
  }
};

SentinelService.prototype.login = function(login, password, err, cb) {
  var that = this;
  that.client.CreateToken(login, password, function() {})
      .fail(function(jqXHR, status, error) {
        that.token = null;
        err(error);
      })
      .done(function(result) {
        console.log("Got token: " + JSON.stringify(result, null, 2));
        that.token = result;
        that.$cookies.put(FORGE_COOKIE_KEY, JSON.stringify(that.token));
        that.loadUserData(err, cb);
      });
};

SentinelService.prototype.logout = function() {
  this.token = null;
  this.$cookies.put(FORGE_COOKIE_KEY, null);
};

SentinelService.prototype.loadUserData = function(err, cb) {
  var that = this;
  var user_partial = new User();
  user_partial.uuid = that.token.user_uuid;
  that.client.FindUser(that.token, user_partial, function() {})
      .fail(function(jqXHR, status, error) {
        that.token = null;
        that.user = null;
        err(error);
      })
      .done(function(user) {
        console.log("Got user: " + JSON.stringify(user, null, 2));
        that.user = user;
        cb(that.token, that.user);
      });
};

forgeApp.factory('sentinel', [
  '$cookies',
  function($cookies) { return new SentinelService($cookies); }
]);
