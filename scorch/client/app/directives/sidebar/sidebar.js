// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.sidebar', [])
    .directive('atSidebar', [
      '$rootScope',
      '$mdDialog',
      'atTextDialog',
      'Person',
      'Project',
      'SourceFile',
      function($rootScope, $mdDialog, atTextDialog, Person, Project,
               SourceFile) {
        return {
          restrict: 'AE',
          templateUrl: 'app/directives/sidebar/sidebar.htm',
          link: function($scope, iElement, iAttrs) {

            window.mdDialog = $mdDialog;

            $scope.sidebarState = {};

            $scope.sampleTree = [
              {
                name: 'dir_one',
                type: 'directory',
                isExpanded: true,
                children: [{
                  name: 'sub_dir_one',
                  type: 'directory',
                  isExpanded: false,
                  children: [
                    {
                      name: 'sub_dir_two',
                      type: 'directory',
                      isExpanded: true,
                      children: [{
                        name: 'nested_file.cc',
                        type: 'file',
                        sourceFileId: ''
                      }]
                    },
                    {
                      name: 'nested_file_one.cc',
                      type: 'file',
                      sourceFileId: ''
                    }
                  ]
                }]
              },
              {name: 'root_file.txt', type: 'file', sourceFileId: ''},
              {name: 'root_file_two.txt', type: 'file', sourceFileId: ''}
            ];

            $scope.test = function(index) {
              console.log('Test: ' + index);
            };

            // Dropdowns
            $scope.directoryDropdown = [
              ['Add New Directory', function($itemScope){}],
              ['Add New File', function($itemScope){}],
              null,
              [
                'Rename',
                function($itemScope){

                }
              ]
            ];

            $scope.getFileById = function(fileId) {
              return _($scope.state.activeProject.sourceFiles)
                  .find(function(file) { return file.id === fileId; });
            };

            $scope.renameDialog = function(ev, file) {
              var oldName = file.name;
              atTextDialog({
                title: 'Rename',
                content: 'Rename ' + file.name + '...',
                placeholder: file.name,
                change: function(val) { file.name = val; }
              })
                  .then(
                      function(newVal) {
                        if (isBlank(file.name)) {
                          file.name = oldName;
                        }
                        // TODO(athilenius): save here
                      },
                      function() { file.name = oldName; });
            };

          }
        };
      }
    ]);
