// Copyright 2015 Alec Thilenius
// All rights reserved.

// Fires the events:
// fileExplorer.fileSelected ({ repo, relativePath })
angular.module('thilenius.file_explorer', [])
    .directive('atFileExplorer', function($rootScope) {
      return {
        restrict : 'AE',
        scope : {control : '='},
        templateUrl : 'app/directives/file_explorer/file_explorer.htm',
        link : function(scope, iElement, iAttrs) {
          // Maps: repo_uuid : {parentEntry}
          scope.loadedRepos = {};
          scope.sortField = 'date';
          scope.sortOrder = 'desc';
          scope.fileTree = {
            selected : null,
            parents : []
          };

          // Expose a control object
          scope.internalControl = scope.control || {};
          scope.internalControl.selected = null;

          // Watch for selections
          scope.$watch('fileTree.selected', function(newVal, oldVal) {
            if (newVal) {
              $rootScope.$broadcast('fileExplorer.fileSelected', newVal.repo,
                                    newVal.relativePath);
              scope.internalControl.selected = newVal;
            }
          });

          // Sorts repos by field, in the given order.
          // field is one of: ['date', 'name']
          // order is one of: ['desc', 'asc']
          scope.internalControl.sortBy = function(field, order) {
            field = field ? field : scope.sortField;
            order = order ? order : scope.sortOrder;
          };

          // Parses and adds a ::crucible::Repo to the file tree
          scope.internalControl.addRepo = function(repo) {
            if (scope.loadedRepos[repo.repoProto.repo_header.repo_uuid]) {
              return;
            }
            var repoFullName = repo.repoProto.repo_header.repo_name;
            var humanRepoName = repoFullName.split('/')[0];
            var newParent = {
              label : humanRepoName,
              children : []
            };
            for (var relativePath in repo.headState) {
              newParent.children.push({
                label : relativePath,
                repo : repo,
                relativePath : relativePath
              });
            }
            scope.fileTree.parents.push(newParent);
            scope.loadedRepos[repo.repoProto.repo_header.repo_uuid] = newParent;
            scope.fileTree.parents = scope.fileTree.parents;
            this.sortBy();
            scope.$apply();
          };

          // Parses and removed a ::crucible::Repo from the file tree
          scope.internalControl.removeRepo = function(repo) {

          };

        }
      };
    });
