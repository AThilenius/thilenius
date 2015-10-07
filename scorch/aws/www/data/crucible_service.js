// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp');

// Fires the events:
// crucible.repoAdded   (CrucibleRepo)
// crucible.repoRemoved (CrucibleRepo)
// crucible.preCommit   (ChangeList)
// crucible.postCommit  (ChangeList)
// crucible.error       (String)
// error                (String)
var CrucibleService = function($rootScope, sentinel) {
  this.$rootScope = $rootScope;
  this.sentinel = sentinel;
  var transport = new Thrift.Transport("/api/crucible/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new CrucibleClient(protocol);
  this.repos = [];
  this.errorState = false;

  // private
  this.executionPipline = [];

  // Error redirect
  $rootScope.$on('crucible.error', function(event, message) {
    $rootScope.$broadcast('error', message);
  });
};

CrucibleService.prototype.enqueueCommit = function(
    repoProtoHeader, changeListProto, doneCallback) {
  if (this.errorState) {
    return;
  }
  var that = this;
  that.$rootScope.$broadcast('crucibe.preCommit', changeListProto);
  this.executionPipline.push(function() {
    that.client.CommitAndDownstream(that.sentinel.token, repoProtoHeader,
                                    changeListProto, null)
        .fail(that.firejqXhrErrorFactory())
        .done(function(result) {
          doneCallback(result);
          console.log("CL commited: " + result.change_list_uuid + ". At: " +
                      repoProtoHeader.latest_change_list_uuid +
                      ". Pipline at " + that.executionPipline.length +
                      " items.");
          that.$rootScope.$broadcast('crucibe.postCommit', result);
          // Remove ourself from the execution pipline
          that.executionPipline.shift();
          // Execute the next handler if there is one
          if (that.executionPipline.length > 0) {
            that.executionPipline[0]();
          }
        });
  });
  if (that.executionPipline.length == 1) {
    that.executionPipline[0]();
  }
};

// private
CrucibleService.prototype.loadAllRepos = function() {
  if (!this.sentinel.token) {
    this.$rootScope.$broadcast('crucible.error',
                               'Crucible requires a sentinel token');
    return;
  }
  var that = this;
  that.client.GetRepoHeadersByUser(that.sentinel.token, null)
      .fail(that.firejqXhrErrorFactory())
      .done(function(result) {
        var errorHandler = function(error) { that.error = error; };
        for (var i = 0; i < result.length; i++) {
          that.loadRepo(result[i]);
        }
      });
};

// private
CrucibleService.prototype.unloadAllRepos = function() {
  for (var i = 0; i < this.repos.length; i++) {
    this.$rootScope.$broadcast('repoRemoved', this.repos[i]);
  }
  repos = [];
};

// private
CrucibleService.prototype.loadRepo = function(repoProtoHeader) {
  var that = this;
  this.client.GetRepoById(this.sentinel.token, repoProtoHeader.repo_uuid, null)
      .fail(that.firejqXhrErrorFactory())
      .done(function(result) {
        var crucibleRepo = new CrucibleRepo(that, result);
        that.repos.push(crucibleRepo);
        that.$rootScope.$broadcast('crucible.repoAdded', crucibleRepo);
      });
};

// private
CrucibleService.prototype.firejqXhrErrorFactory = function() {
  var that = this;
  return function(jqXhr, stat, error) {
    if (jqXhr && jqXhr.status === 0) {
      that.$rootScope.$broadcast('crucible.error',
                                 'Status 0 | cannot connect to Crucible');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.$rootScope.$broadcast('crucible.error',
                                 'Status ' + stat + ' | ' + error.user_message);
    } else {
      that.$rootScope.$broadcast('crucible.error',
                                 'Status ' + stat +
                                     ' | Something isnt right here...');
    }
  };
};

forgeApp.factory('crucible', [
  '$rootScope',
  'sentinel',
  function($rootScope, sentinel) {
    return new CrucibleService($rootScope, sentinel);
  }
]);
