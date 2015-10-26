var forgeApp = angular.module('forgeApp');

forgeApp.controller('crucibleController', [
  '$scope',
  '$rootScope',
  '$routeParams',
  function($scope, $rootScope, $routeParams, $sce) {
    $rootScope.htmlClass = 'none';
    $rootScope.bodyClass = 'none';
    $rootScope.title = 'Crucible';
    var transport = new Thrift.Transport("/api/crucible/");
    var protocol = new Thrift.Protocol(transport);
    var client = new CrucibleClient(protocol);

    $scope.repoUuid = $routeParams.repoUuid;
    $scope.clUuid = $routeParams.clUuid;
    $scope.files = [];

    client.GetRepoById($scope.repoUuid, null)
        .fail(function(jqXhr, stat, error) {
          // Ignore errors
          console.log("Crucible Error: " + error);
        })
        .done(function(result) {
          var crucibleRepo = new CrucibleRepo(null, result);
          $scope.$apply(function() {
            $scope.repoHeader = result.repo_header;
            $scope.crucibleRepo = crucibleRepo;
            $scope.repoTimestamp =
                new Date(parseInt(result.repo_header.creation_timestamp))
                    .toLocaleString();
            // Find the CL
            for (var i = 0; i < result.change_lists.length; i++) {
              var changeList = result.change_lists[i];
              if (changeList.change_list_uuid === $scope.clUuid) {
                $scope.changeList = changeList;
                $scope.changeListTimestamp =
                    new Date(parseInt(changeList.timestamp)).toLocaleString();
                $scope.commitNumber = i + 1;
                break;
              }
            }
            // Map of <relativePath : crucible::proto::File>
            $scope.files = crucibleRepo.reconstructFilesForCL($scope.clUuid);
          });
        });
  }
]);
