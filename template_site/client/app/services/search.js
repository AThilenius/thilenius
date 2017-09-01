'use strict';

angular
  .module('app')
  .service('search', searchService);

/** @ngInject */
function searchService() {

  /** A map of searchgroup to Fuse instances */
  this.searchGroups = {};

  /**
   * Adds or replaces an existing searchGroup with the items array, and indexes
   * it based on searchKeys array.
   */
  this.setSearchGroup = function(groupName, items, searchKeys) {
    this.searchGroups[groupName] = new Fuse(items, {
      caseSensitive: false,
      include: ['score', 'matches'],
      shouldSort: false,
      tokenize: false,
      threshold: 0.4,
      location: 0,
      distance: 100,
      maxPatternLength: 32,
      keys: searchKeys
    });
  };

  /**
   * Searches all groups for the text and returns them.
   */
  this.searchAll = function(text) {
    var results = _.flatten(_.keys(this.searchGroups)
      .map(groupName =>
        this.searchGroups[groupName]
        .search(text)
        .map(match => angular.extend(match, {
          groupName
        }))
      ));
    //[ { groupname, matches, score, item } ]
    // Sort by score
    results = results.sort((a, b) => a.score - b.score);
    return results;
  };

  /** Add default page routes */
  this.setSearchGroup('routes', [{
    name: 'Policy',
    url: '/policy',
    icon: 'lock'
  }, {
    name: 'Home',
    url: '/',
    icon: 'home'
  }], ['name']);

};
