$(function() {
  // ACE Editor
  var editor = ace.edit("editor");
  editor.setTheme("ace/theme/twilight");
  editor.getSession().setMode("ace/mode/javascript");
});

var forgeApp =
    angular.module('forgeApp', [ 'ngCookies', 'thilenius.tree_view' ]);

forgeApp.controller('forgeController', [
  '$scope',
  'sentinel',
  function($scope, sentinel) {
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
      sentinel.login(email, password, function(){}, function(){
        $scope.$apply();
      });
    };
  }
]);
