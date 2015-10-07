var forgeApp = angular.module('forgeApp');

var Crucible = function(client, session) {
  this.client = client;
  this.session = session;
};

forgeApp.factory('crucible', [
  'session',
  function(session) {
    var transport = new Thrift.Transport("/api/crucible/");
    var protocol = new Thrift.Protocol(transport);
    var client = new CrucibleClient(protocol);
    return new Crucible(client, session);
  }
]);
