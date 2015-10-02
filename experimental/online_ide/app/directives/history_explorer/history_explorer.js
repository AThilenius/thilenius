// Copyright 2015 Alec Thilenius
// All rights reserved.

// Fires the events:
// historyExplorer.snapshotSelected ({ repo, relativePath })
angular.module('thilenius.history_explorer', [])
    .directive('atHistoryExplorer', function($rootScope) {
      return {
        restrict : 'AE',
        scope : {control : '='},
        templateUrl : 'app/directives/history_explorer/history_explorer.htm',
        link : function(scope, iElement, iAttrs) {
          scope.hasSelection = false;
          scope.repoName = null;
          scope.fileRelativePath = null;
          scope.historyTree = {
            parents : []
          };

          // Watch for selections
          scope.$watch('fileTree.selected', function(newVal, oldVal) {
            if (newVal) {
              $rootScope.$broadcast('historyExplorer.snapshotSelected',
                                    newVal.repo, newVal.relativePath,
                                    newVal.changeList);
            }
          });

          // Expose a control object
          scope.internalControl = scope.control || {};

          // Should be called when a file is selected, renders out all CLs
          scope.internalControl.setRepoFile = function(repo, relativePath) {
            if (!repo || !relativePath) {
              scope.hasSelection = false;
              scope.repoName = null;
              scope.fileRelativePath = null;
              return;
            }
            scope.hasSelection = true;
            var selectedRepoProto = repo.repoProto;
            scope.repoName = selectedRepoProto.repo_header.repo_name;
            scope.fileRelativePath = relativePath;
            // Compleatly rebuild the tree
            var newHistoryTree = {
              parents : []
            };
            // First organize it all into buckets by CL timestamp day
            var days = {};
            for (var i = 0; i < selectedRepoProto.change_lists.length; i++) {
              var changeList = selectedRepoProto.change_lists[i];
              var date = (new Date(parseInt(changeList.timestamp)))
                             .toLocaleDateString();
              if (!days[date]) {
                days[date] = [];
              }
              days[date].push(changeList);
            }
            // Add it all to the new tree
            for (var dayDate in days) {
              var changeLists = days[dayDate];
              var newParent = {
                label : dayDate,
                children : []
              };
              for (var j = 0; j < changeLists.length; j++) {
                var cl = changeLists[j];
                newParent.children.push({
                  label :
                      (new Date(parseInt(cl.timestamp))).toLocaleTimeString(),
                  repo : selectedRepoProto,
                  relativePath : relativePath,
                  changeList : cl
                });
              }
              newHistoryTree.parents.push(newParent);
            }
            if (newHistoryTree.parents.length > 0) {
              newHistoryTree.parents[0].expanded = true;
              if (newHistoryTree.parents[0].children.length > 0) {
                newHistoryTree.selected = newHistoryTree.parents[0].children[0];
              }
            }
            $scope.historyTree = newHistoryTree;
          };

        }
      };
    });
