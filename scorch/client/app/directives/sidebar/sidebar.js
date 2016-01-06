// Copyright 2015 Alec Thilenius
// All rights reserved.


angular.module('thilenius.sidebar', [])
    .directive('atSidebar', [
      '$rootScope',
      '$mdDialog',
      'Person',
      'Project',
      'SourceFile',
      function($rootScope, $mdDialog, Person, Project, SourceFile) {
        return {
          restrict: 'AE',
          templateUrl: 'app/directives/sidebar/sidebar.htm',
          link: function($scope, iElement, iAttrs) {

            $scope.sampleTree = {
              directories: [{
                name: 'dir_one',
                isExpanded: true,
                directories: [{
                  name: 'sub_dir_one',
                  isExpanded: false,
                  directories: [{
                    name: 'sub_dir_two',
                    isExpanded: true,
                    directories: [],
                    files: [{name: 'nested_file.cc', sourceFileId: ''}]
                  }],
                  files: [{name: 'nested_file_one.cc', sourceFileId: ''}]
                }],
                files: []
              }],
              files: [{name: 'root_file.txt', sourceFileId: ''}]
            };

            $scope.getFileById = function(fileId) {
              return _($scope.state.activeProject.sourceFiles)
                  .find(function(file) { return file.id === fileId; });
            };

            $scope.renameDialog = function(ev, file) {
              var oldName = file.name;
              $mdDialog.show({
                controller: function($scope, $mdDialog) {
                  $scope.file = file;
                  $scope.cancel = function() { $mdDialog.cancel(); };
                  $scope.rename = function() { $mdDialog.hide(); };
                },
                templateUrl: 'renameDialog',
                parent: angular.element(document.body),
                targetEvent: ev,
                clickOutsideToClose: true
              })
                  .then(
                      function() {
                        if (isBlank(file.name)) {
                          file.name = oldName;
                        }
                        // TODO(athilenius): save here
                      },
                      function() {
                        file.name = oldName;
                      });

            };

          }
        };
      }
    ]);
