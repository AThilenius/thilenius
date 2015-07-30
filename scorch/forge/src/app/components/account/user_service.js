var forgeApp = angular.module('forgeApp');

var User = function(client, session) {
  this.has_data = false;
  this.user_data = null;
  this._client = client;
  this._session = session;

  // Load data if we already have a session (probably from a cookie)
  if (this._session.session_token && this._session.session_token.length > 0) {
    this.load(function(){}, function(){
      console.log('User data loaded from cookie session token');
    });
  }
};

User.prototype.load = function(err, cb) {
  if (this.has_data) {
    cb(this.user_data);
  } else {
    if (this._session.session_token) {
      var that = this;
      this._client.read_user_data(this._session.session_token, function() {})
          .fail(function(jqXHR, status, error) { err(error); })
          .done(function(result) {
            that.has_data = true;
            that.user_data = result;
            cb(that.user_data);
          });
    } else {
      err({failure_message : "No session token present"});
    }
  }
};

User.prototype.save = function(user_data) {

};

User.prototype.logout = function() {
  this._session.logout();
  this.has_data = false;
  this.user_data = null;
};

forgeApp.factory('user', [
  'session',
  function(session) {
    var transport = new Thrift.Transport("/services/user_data/");
    var protocol = new Thrift.Protocol(transport);
    var client = new UserDataClient(protocol);
    return new User(client, session);
  }
]);
