var forgeApp = angular.module('forgeApp');

var FORGE_COOKIE_KEY = 'forge_session_token';

var Session = function($cookies, client) {
  this.session_token = $cookies.get(FORGE_COOKIE_KEY);
  this.$cookies = $cookies;
  this.client = client;
};

Session.prototype.login = function(login, password, err, cb) {
  var that = this;
  that.client.CreateToken(login, password, function(){})
      .fail(function(jqXHR, status, error) {
        that.session_token = null;
        err(error);
      })
      .done(function(result) {
        console.log("Got token: " + JSON.stringify(result, null, 2));
        that.session_token = result;
        that.$cookies.put(FORGE_COOKIE_KEY, that.session_token);
        var user_partial = new User();
        user_partial.uuid = that.session_token.user_uuid;
        that.client.FindUser(that.session_token, user_partial, function(){})
            .fail(function(jqXHR, status, error) {
              that.session_token = null;
              that.user = null;
              err(error);
            })
            .done(function(user) {
              console.log("Got user: " + JSON.stringify(user, null, 2));
              that.user = user;
              cb(that.session_token, that.user);
            });
      });
};

Session.prototype.logout = function() {
  this.session_token = null;
  this.$cookies.put(FORGE_COOKIE_KEY, null);
  $scope.$apply();
};

forgeApp.factory('session', [
  '$cookies',
  function($cookies) {
    var transport = new Thrift.Transport("/api/sentinel/");
    var protocol = new Thrift.Protocol(transport);
    var client = new SentinelClient(protocol);
    return new Session($cookies, client);
  }
]);
