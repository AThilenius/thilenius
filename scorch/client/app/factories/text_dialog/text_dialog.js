// Copyright 2015 Alec Thilenius
// All rights reserved.

var app = angular.module('app');

app.factory('atTextDialog', function($mdDialog) {
  return function(properties) {
    properties.ok = properties.ok || 'Okay';
    properties.cancel = properties.cancel || 'Cancel';
    return $mdDialog.show({
      controller: function($scope, $mdDialog) {
        $scope.properties = properties;
        $scope.model = properties.placeholder || '';
        if (properties.change) {
          $scope.$watch('model', function(oldval, newval) {
            properties.change($scope.model);
          });
        }
        $scope.cancel = function() { $mdDialog.cancel(); };
        $scope.okay = function() { $mdDialog.hide($scope.model); };
      },
      templateUrl: 'app/factories/text_dialog/text_dialog.htm',
      parent: angular.element(document.body),
      clickOutsideToClose: true
    });
  };
});
