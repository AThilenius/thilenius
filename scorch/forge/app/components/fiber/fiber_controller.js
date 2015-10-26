var forgeApp = angular.module('forgeApp');

forgeApp.controller('fiberController', [
  '$scope',
  '$rootScope',
  '$routeParams',
  '$sce',
  function($scope, $rootScope, $routeParams, $sce) {
    $rootScope.htmlClass = 'none';
    $rootScope.bodyClass = 'none';
    $rootScope.title = 'Streaming Fiber';

    var transport = new Thrift.Transport("/api/fiber/");
    var protocol = new Thrift.Protocol(transport);
    var client = new FiberClient(protocol);

    $scope.cordUuid = $routeParams.uuid;
    $scope.content = [];
    $scope.streamState = "PENDING";

    client.GetCord($scope.cordUuid, null)
        .fail(function(jqXhr, stat, error) {
          console.log("Failed to get fiber cord!");
        })
        .done(function(result) {
          $scope.streamState = "STREAMING";
          $scope.cordStream = new CordStream(client, result);
          $scope.cordUrl = $scope.cordStream.fiberUrl;
          $scope.cord = $scope.cordStream.cordProto;
          $scope.timestamp =
              new Date(parseInt($scope.cord.timestamp)).toLocaleString();
          $scope.$apply();
          $scope.cordStream.onGrain(
              function(grains) {
                for (var i = 0; i < grains.length; i++) {
                  $scope.writeLine(grains[i].data);
                }
                $scope.$apply();
              },
              function() {
                $scope.streamState = "CLOSED";
                $scope.$apply();
              });
        });

    // Copied from console_window.js
    // private
    $scope.escapeCodes = {
      "[0m": "<span class='fiber-text' style='color: white;'>",
      "[31m": "<span class='fiber-text' style='color: #AF0000;'>",
      "[32m": "<span class='fiber-text' style='color: #009800;'>",
      "[33m": "<span class='fiber-text' style='color: #FFC000;'>",
      "[36m": "<span class='fiber-text' style='color: #008DCC;'>"
    };

    // private
    // TODO(athilenius): Do somethig with the channel
    $scope.writeLine = function(contentString) {
      var lines = contentString.split('\n');
      // Strip trailing \n
      if (lines.length > 0 && lines[lines.length - 1] === '') {
        lines = lines.slice(0, lines.length - 1);
      }
      for (var i = 0; i < lines.length; i++) {
        // TODO(athilenius): sanatize oritinal text
        var line = lines[i] === '' ? ' ' : lines[i];
        for (var escapeCode in $scope.escapeCodes) {
          var htmlCode = $scope.escapeCodes[escapeCode];
          line = line.split(escapeCode).join("</span>" + htmlCode);
        }
        line = "<span class='fiber-text'>" + line + "</span>";
        $scope.content.push($sce.trustAsHtml(line));
      }
    };

  }
]);
