var forgeApp = angular.module('forgeApp');

var FORGE_COOKIE_KEY = 'forge_session_token';

var Session = function($cookies, client) {
  this.session_token = $cookies.get(FORGE_COOKIE_KEY);
  this.$cookies = $cookies;
  this._client = client;
};

Session.prototype.login = function(login, password, err, cb) {
  var that = this;
  that._client.create_ptoken(login, password, function() {})
      .fail(function(jqXHR, status, error) {
        that.session_token = null;
        err(error);
      })
      .done(function(result) {
        that.session_token = result;
        that.$cookies.put(FORGE_COOKIE_KEY, that.session_token);
        cb(that.session_token);
      });
};

forgeApp.factory('session', [
  '$cookies',
  function($cookies) {
    var transport = new Thrift.Transport("/services/authentication/");
    var protocol = new Thrift.Protocol(transport);
    var client = new AuthenticationClient(protocol);
    return new Session($cookies, client);
  }
]);
