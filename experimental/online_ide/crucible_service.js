// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('forgeApp');

var CrucibleService = function(sentinel) {
  this.sentinel = sentinel;
  var transport = new Thrift.Transport("/api/crucible/");
  var protocol = new Thrift.Protocol(transport);
  this.client = new CrucibleClient(protocol);
  this.repos = [];

  this.eventHandlers = {
    // void (CrucibleRepo)
    'repoAdded' : [],

    // void (CrucibleRepo)
    'repoRemoved' : [],

    // void (ChangeList)
    // Called when a commit is added to the execution pipline
    'preCommit' : [],

    // void (ChangeList)
    // Called when a commit comes back successful from Crucible
    'postCommit' : [],

    // void (String)
    'error' : []
  };

  this.errorState = false;

  // private
  this.executionPipline = [];
};

// Registers an event handler
CrucibleService.prototype.on = function(eventName, newHandler) {
  this.eventHandlers[eventName].push(newHandler);
};

CrucibleService.prototype.loadAllRepos = function() {
  if (!this.sentinel.token) {
    this.fireEvent('error', 'Crucible requires a sentinel token');
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

CrucibleService.prototype.enqueueCommit = function(
    repoProtoHeader, changeListProto, doneCallback) {
  if (this.errorState) {
    return;
  }
  that.fireEvent('preCommit', changeList);
  var that = this;
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
          that.fireEvent('postCommit', changeList);
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
CrucibleService.prototype.fireEvent = function(eventName, eventArgs) {
  var handlersList = this.eventHandlers[eventName];
  for (var i = 0; i < handlersList.length; i++) {
    handlersList[i](eventArgs);
  }
  if (eventName == 'error') {
    console.error(eventArgs);
    this.errorState = true;
  }
};

// private
CrucibleService.prototype.loadRepo = function(repoProtoHeader) {
  var that = this;
  this.client.GetRepoById(this.sentinel.token, repoProtoHeader.repo_uuid, null)
      .fail(that.firejqXhrErrorFactory())
      .done(function(result) {
        var crucibleRepo = new CrucibleRepo(that, result);
        that.repos.push(crucibleRepo);
        that.fireEvent('repoAdded', crucibleRepo);
      });
};

// private
CrucibleService.prototype.firejqXhrErrorFactory = function() {
  var that = this;
  return function(jqXhr, stat, error) {
    if (jqXhr && jqXhr.status === 0) {
      that.fireEvent('error', 'Status 0 | cannot connect to Crucible');
    } else if (error && error.user_message && error.user_message.length > 0) {
      that.fireEvent('error', 'Status ' + stat + ' | ' + error.user_message);
    } else {
      that.fireEvent('error',
                     'Status ' + stat + ' | Something isnt right here...');
    }
  };
};

forgeApp.factory('crucible', [
  'sentinel',
  function(sentinel) { return new CrucibleService(sentinel); }
]);
