var forgeApp = angular.module('forgeApp');

var CrucibleService = function(sentinel) {
  this.sentinel = sentinel;
  var transport = new Thrift.Transport("/api/sentinel/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new CrucibleClient(protocol);
  this.error = null;
  this.repos = [];
  var that = this;
  $scope.$watch(function(scope) { return this.sentinel.token; },
                function(newValue, oldValue) {
                  if (newValue) {
                    that.loadAllRepos();
                  } else {
                    that.error = null;
                    that.repos = [];
                  }
                });
};

CrucibleService.prototype.getFile = function(repo, filePath) {
  // Need to use diff-match-path to patch file up to head
};

// TODO(athilenius): Repos should be pulled lazily
CrucibleService.prototype.loadAllRepos = function() {
  var that = this;
  that.client.GetRepoHeadersByUser(that.sentinel.token, function() {})
      .fail(function(jqXHR, status, error) {
        that.error = error;
        console.log("Crucible server error: " + JSON.stringify(error, null, 2));
      })
      .done(function(result) {
        console.log("Got repo headers: " + JSON.stringify(result, null, 2));
        // Now load each repo (Not ideal)
        that.loadRepo(result);
      });
};

CrucibleService.prototype.loadRepo = function(repoHeader) {
  var that = this;
  that.client.GetRepoById(that.sentinel.token,
                          repoHeader.repo_uuid, function() {})
      .fail(function(jqXHR, status, error) {
        that.error = error;
        console.log("Crucible server error: " + JSON.stringify(error, null, 2));
      })
      .done(function(result) {
        that.repos.append(result);
      });
};

CrucibleService.prototype.saveFileChanges = function() {
  // Hmmm, how?
};

forgeApp.factory('crucible', [
  'sentinel',
  function(sentinel) { return new CrucibleService(sentinel); }
]);
