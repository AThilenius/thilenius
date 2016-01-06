// Copyright 2015 Alec Thilenius
// All rights reserved.

// Fires the events:
// fileExplorer.fileSelected ({ repo, relativePath })
angular.module('thilenius.file_explorer', [])
    .directive('atFileExplorer', [
      '$rootScope',
      'Person',
      'Project',
      'SourceFile',
      function($rootScope, Person, Project, SourceFile) {
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
                  $itemScope.project.sourceFiles.push(
                      Project.sourceFiles.create({id: $itemScope.project.id},
                                                 {fullPath: 'Unnamed.txt'}));
                }
              ]
            ];

            $scope.fileDropDown = [
              [
                'Rename File',
                function($itemScope) { $itemScope.fileEditCtrl.$show(); }
              ],
              [
                'Delete File',
                function($itemScope) {
                  SourceFile.deleteById({id: $itemScope.sourceFile.id});
                  $itemScope.project.sourceFiles =
                      _($itemScope.project.sourceFiles)
                          .without($itemScope.sourceFile);
                }
              ]
            ];

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
