// Follows the structure:
//
// tree:
//   selected: { // child object // },
//   parents: [
//     label: "Linked List",
//     expanded: false,
//     children: [
//       label: "linked_list.cc"
//     ]
//   ],
angular.module('thilenius.tree_view', [])
    .directive('atTreeView', function() {
      return {
        restrict : 'AE',
        scope : {tree : '=ngModel'},
        templateUrl : 'app/directives/tree_view/tree_view.htm',
        link : function(scope, iElement, iAttrs) {
          // Methods can go here
        }
      };
    });
