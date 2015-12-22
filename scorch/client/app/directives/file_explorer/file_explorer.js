// Copyright 2015 Alec Thilenius
// All rights reserved.

// Fires the events:
// fileExplorer.fileSelected ({ repo, relativePath })
angular.module('thilenius.file_explorer', [])
    .directive('atFileExplorer', [
      '$rootScope',
      '$uibModal',
      'Person',
      'Project',
      'SourceFile',
      function($rootScope, $uibModal, Person, Project, SourceFile) {
        return {
          restrict: 'AE',
          templateUrl: 'app/directives/file_explorer/file_explorer.htm',
          link: function($scope, iElement, iAttrs) {

            $scope.explorerDropDown = [[
              'Add Project',
              function($itemScope) {
                $scope.projects.push(Person.projects.create(
                    {id: Person.getCurrentId()},
                    {config: 'No-config-string', name: 'Unnamed'}));
              }
            ]];

            $scope.projectDropDown = [
              [
                'Rename Porject',
                function($itemScope) { $itemScope.editCtrl.$show(); }
              ],
              [
                'Delete Porject',
                function($itemScope) {
                  Project.deleteById({id: $itemScope.project.id});
                  $scope.projects =
                      _($scope.projects).without($itemScope.project);
                }
              ],
              null,
              [
                'Add New File',
                function($itemScope) {
                  //var modalInstance = $uibModal.open({
                    //size: 'sm',
                    //templateUrl: 'ModalText.html',
                    //controller: 'ModalTextController',
                    //resolve: {text: function() { return $scope.text; }}
                  //});
                  //modalInstance.result.then(
                      //function(text) {
                        //console.log('After: ' + text);
                      //},
                      //function() {
                        //// STUB - Called when cancel is clicked
                      //});
                  $itemScope.project.sourceFiles.push(
                      Project.sourceFiles.create({id: $itemScope.project.id},
                                                 {fullPath: 'Unnamed.txt'}));
                }
              ]
            ];

            $scope.fileDropDown = [[
              'Rename File',
              function($itemScope) { $itemScope.fileEditCtrl.$show(); }
            ]];

            $scope.saveProjectShallow = function(project) {
              Project.prototype$updateAttributes(
                  {id: project.id},
                  {name: project.name, expanded: project.expanded});
            };

            $scope.saveSourceFileShallow = function(sourceFile) {
              SourceFile.prototype$updateAttributes(
                  {id: sourceFile.id}, {fullPath: sourceFile.fullPath});
            };

            $scope.toggleExpanded = function(project) {
              project.expanded = !project.expanded;
              $scope.saveProjectShallow(project);
            };

          }
        };
      }
    ]);
