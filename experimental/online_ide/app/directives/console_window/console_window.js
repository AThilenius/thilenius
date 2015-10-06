// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.console_window', [])
    .directive('atConsoleWindow', [
      '$sce',
      '$rootScope',
      function($sce, $rootScope) {
        return {
          restrict : 'AE',
          scope : {control : '='},
          templateUrl : 'app/directives/console_window/console_window.htm',
          link : function(scope, iElement, iAttrs) {

            scope.content = [];
            scope.activeTab = 'compilerOutput';

            // Expose a control object
            scope.internalControl = scope.control || {};

            scope.internalControl.write = function(contentString) {
              var lines = contentString.split('\n');
              for (var i = 0; i < lines.length; i++) {
                var line = lines[i];
                scope.content.push($sce.trustAsHtml(line));
              }
            };

            scope.internalControl.clear = function() { scope.content = []; };

            $rootScope.$on('billet.runBegin', function(eventArgs) {
              scope.$apply(function() { scope.internalControl.clear(); });
            });

            $rootScope.$on('billet.runOutput', function(eventArgs, app_output) {
              scope.$apply(function() {
                for (var i = 0; i < app_output.output_tokens.length; i++) {
                  var output_token = app_output.output_tokens[i];
                  scope.internalControl.write(output_token.content);
                }
              });
            });

            $rootScope.$on('billet.runEnd', function(eventArgs) {
              scope.$apply(function() {
                scope.internalControl.write("Run Ended");
              });
            });

          }
        };
      }
    ]);
