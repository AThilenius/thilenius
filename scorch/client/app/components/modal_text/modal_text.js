// Copyright 2015 Alec Thilenius
// All rights reserved.

var forgeApp = angular.module('app');

forgeApp.controller('ModalTextController', [
  '$scope',
  '$uibModalInstance',
  function($scope, $uibModalInstance, text) {
    $scope.text = text;
    $scope.ok = function() { $uibModalInstance.close($scope.text); };
    $scope.cancel = function() { $uibModalInstance.dismiss('cancel'); };
  }
]);
