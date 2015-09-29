var forgeApp = angular.module('forgeApp');

var CrucibleService = function(sentinel) {
  this.sentinel = sentinel;
  var transport = new Thrift.Transport("/api/crucible/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new CrucibleClient(protocol);
  this.error = null;
  this.repos = [];

  // Set to true while waiting for a commit to come back from Crucible
  this.isCommitPending = false;
};

// TODO(athilenius): Repos should be pulled lazily
// Callback will be called every time a new repo is computed form diffs with all
// repos as the argument
CrucibleService.prototype.loadAllRepos = function(callback) {
  if (!this.sentinel.token) {
    this.error = "";
    this.repos = [];
    return;
  }
  var that = this;
  that.client.GetRepoHeadersByUser(that.sentinel.token, null)
      .fail(function(jqXHR, status, error) {
        that.error = error;
        console.log("Crucible server error: " + JSON.stringify(error, null, 2));
      })
      .done(function(result) {
        console.log("Got repo headers: " + JSON.stringify(result, null, 2));
        // Now load each repo (Not ideal)
        var errorHandler = function(error) { that.error = error; };
        for (var i = 0; i < result.length; i++) {
          that.repos.push(new CrucibleRepo(that, that.sentinel, that.client,
                                           result[i], callback, errorHandler));
        }
      });
};

forgeApp.factory('crucible', [
  'sentinel',
  function(sentinel) { return new CrucibleService(sentinel); }
]);
