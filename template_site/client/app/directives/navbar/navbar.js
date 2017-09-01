'use strict';

angular
  .module('app')
  .directive('navbar', navbar);

/** @ngInject */
function navbar() {
  return {
    restrict: 'AE',
    templateUrl: 'app/directives/navbar/navbar.html',
    scope: {},
    controller: navbarController
  };
};

/** @ngInject */
function navbarController($scope, $element, $attrs, $rootScope, $state, $sce,
  $location, Person, search) {

  $scope.rootScope = $rootScope;

  /**
   * Called when an item is selected (or enter is pressed) in the search bar.
   */
  $scope.select = function(searchMatch) {
    console.log('Select: ', searchMatch);
    if (searchMatch) {
      $scope.navigate(searchMatch.item.url);
    }
  };

  /**
   * Searches the indexed search for the given text, limits results to 100,
   * and generates match HTML for use in rendering autocomplete.
   */
  $scope.querySearch = function(text) {
    var results = search.searchAll(text);
    // Keep only the first 100 results.
    // TODO(athilenius): This should be filtered in the Search Service.
    results = results.slice(0, 100);
    // Convert 'matches' to HTML list
    results.forEach(result => {
      result.matchesHtml = [];
      result.matches.forEach(match => {
        var value = result.item[match.key];
        if (typeof(value) === 'string') {
          // Array of array[2]: [ [0, 1], [4, 6] ]
          var indices = match.indices;
          var html = '';
          for (var i = 0; i < indices.length; i++) {
            var lastIndex = i > 0 ? indices[i - 1] : [0, -1];
            // Text up to bold
            html += s.truncate(
              value.slice(lastIndex[1] + 1, indices[i][0]), 20);
            html +=
              `<u>${value.slice(indices[i][0], indices[i][1] + 1)}</u>`;
          }
          html += s.truncate(value.slice(_(indices).last()[1] + 1), 20);
          result.matchesHtml.push($sce.trustAsHtml(html));
        } else {
          result.matchesHtml.push($sce.trustAsHtml(value));
        }
      });
    });
    $scope.results = results;
    return results;
  };

  // TODO(athilenius): All the crap below should be removed, and the navbar
  // should be a child.
  
  $scope.logout = function() {
    Person.logout().$promise.finally(() => {
      $rootScope.currentUser = null;
      $scope.goToHome();
    });
  };

  $scope.navigate = function(link) {
    $location.path(link);
  };

  $scope.goToHome = function() {
    $state.go('/');
  };

  $scope.goToState = function(state) {
    $state.go(state);
  };

};
