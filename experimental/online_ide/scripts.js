var forgeApp =
    angular.module('forgeApp', [ 'ngCookies', 'thilenius.tree_view' ]);

forgeApp.controller('forgeController', [
  '$scope',
  'sentinel',
  'crucible',
  function($scope, sentinel, crucible) {
  $scope.editor = ace.edit("editor");
  $scope.editor.setTheme("ace/theme/twilight");
  $scope.editor.getSession().setMode("ace/mode/javascript");
  $scope.sentinel = sentinel;
  $scope.activeSidebarTab = 'file';
  $scope.fileTree = {
    parents : [
      {
        label : "Sack",
        children : [
          {label : "linked_list.h"},
          {label : "linked_list.cc"},
          {label : "CMakeLists.txt"}
        ]
      },
      {
        label : "Linked List",
        expanded : true,
        children : [
          {label : "linked_list.h"},
          {label : "linked_list.cc"},
          {label : "CMakeLists.txt"}
        ]
      },
      {
        label : "Hash Map",
        children : [
          {label : "linked_list.h"},
          {label : "linked_list.cc"},
          {label : "CMakeLists.txt"}
        ]
      },
      {
        label : "Dequeue",
        children : [
          {label : "linked_list.h"},
          {label : "linked_list.cc"},
          {label : "CMakeLists.txt"}
        ]
      }
    ]
  };
  $scope.login = function(email, password) {
    sentinel.login(email,
                   password, function() {}, function() { $scope.$apply(); });
  };
  // Watch for changes to sentinel, load a repo when it changes
  $scope.$watch(function(scope) { return sentinel.token; },
                function(newValue, oldValue) {
                  crucible.loadAllRepos(function(newCrucibleRepo) {
                    var newFileTree = {
                      parents : []
                    };
                    for (var i = 0; i < crucible.repos.length; i++) {
                      var crucibleRepo = crucible.repos[i];
                      var entry = {
                        label : crucibleRepo.repoHeader.repo_name,
                        children : []
                      };
                      for (var relativePath in crucibleRepo.headState) {
                        entry.children.push({
                          label : relativePath,
                          repo : crucibleRepo,
                          source : crucibleRepo.headState[relativePath]
                        });
                      }
                      newFileTree.parents.push(entry);
                    }
                    $scope.$apply(function() { $scope.fileTree = newFileTree;
                    });
                  });
                });
  $scope.$watch(function(scope) { return scope.fileTree.selected; },
                function(newVal, oldVal) {
                  if (newVal) {
                    $scope.editor.setValue(newVal.source.source, 1);
                  }
                });
  // Watch for text changes in the editor
  $scope.editor.on("change", function(e) {
    // Commit at max once every 1000ms
    if (!$scope.editorIdleTimer && !crucible.isCommitPending) {
      $scope.editorIdleTimer = setTimeout(function() {
        $scope.editorIdleTimer = null;
        $scope.fileTree.selected.repo.stash($scope.fileTree.selected.label,
                                            $scope.editor.getValue());
      }, 1000);
    }
  });
}
]);
