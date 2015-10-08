// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.console_window', [])
    .directive('atConsoleWindow', [
      '$sce',
      '$rootScope',
      function($sce, $rootScope) {
        return {
          restrict : 'AE',
          scope : {control : '=', show : '='},
          templateUrl : 'app/directives/console_window/console_window.htm',
          link : function(scope, iElement, iAttrs) {

            scope.compilerContent = [];
            scope.applicationContent = [];
            scope.activeTab = 'compilerOutput';
            scope.escapeCodes = {
              "[0m" : "<span class='console-text' style='color: white;'>",
              "[31m" : "<span class='console-text' style='color: #FF0000;'>",
              "[32m" : "<span class='console-text' style='color: #00FF00;'>",
              "[33m" : "<span class='console-text' style='color: yellow;'>",
              "[36m" : "<span class='console-text' style='color: #00D8FF;'>"
            };
            scope.compilerScroller = $('#compilerScroller');
            scope.consoleScroller = $('#consoleScroller');

            // Helper for writing content to a log
            scope.write = function(contentString, contentArray) {
              var lines = contentString.split('\n');
              for (var i = 0; i < lines.length; i++) {
                // For example, if line is:
                // green Hello blue world
                // <span><pre>
                // </pre></span><span gree><pre>Hello
                // </pre></span><span blue><pre>world
                // </pre></span>
                // TODO(athilenius): sanatize oritinal text
                var line = lines[i];
                if (line === '') {
                  // Assume it's an escape character
                  line = '\u001b';
                }
                for (var escapeCode in scope.escapeCodes) {
                  var htmlCode = scope.escapeCodes[escapeCode];
                  line = line.split(escapeCode).join("</span>" + htmlCode);
                }
                line = "<span class='console-text'>" + line + "</span>";
                contentArray.push($sce.trustAsHtml(line));
              }
            };

            // Expose a control object and show
            scope.internalControl = scope.control || {};

            $rootScope.$on('billet.compileBegin', function(eventArgs) {
              scope.$apply(function() {
                scope.activeTab = 'compilerOutput';
                scope.compilerContent = [];
                scope.applicationContent = [];
                scope.show = true;
              });
            });

            $rootScope.$on('billet.compileOutput', function(eventArgs,
                                                            app_output) {
              scope.$apply(function() {
                for (var i = 0; i < app_output.output_tokens.length; i++) {
                  var output_token = app_output.output_tokens[i];
                  scope.write(output_token.content, scope.compilerContent);
                }
              });
              $('#compilerScroller')
                  .animate(
                      {scrollTop : $('#compilerScroller')[0].scrollHeight}, 50);
            });

            $rootScope.$on('billet.compileEnd', function(eventArgs,
                                                         app_output) {
              scope.$apply(function() {
                scope.write("Ended with exit code: " +
                                app_output.termination_code,
                            scope.compilerContent);
                scope.show = true;
              });
              $('#compilerScroller')
                  .animate(
                      {scrollTop : $('#compilerScroller')[0].scrollHeight}, 50);
            });

            $rootScope.$on('billet.runBegin', function(eventArgs) {
              scope.$apply(function() {
                scope.activeTab = 'consoleOutput';
                scope.show = true;
              });
            });

            $rootScope.$on('billet.runOutput', function(eventArgs, app_output) {
              scope.$apply(function() {
                for (var i = 0; i < app_output.output_tokens.length; i++) {
                  var output_token = app_output.output_tokens[i];
                  scope.write(output_token.content, scope.applicationContent);
                }
              });
              $('#consoleScroller')
                  .animate({scrollTop : $('#consoleScroller')[0].scrollHeight},
                           50);
            });

            $rootScope.$on('billet.runEnd', function(eventArgs, app_output) {
              scope.$apply(function() {
                scope.write("Ended with exit code: " +
                                app_output.termination_code,
                            scope.applicationContent);
                scope.show = true;
              });
              $('#consoleScroller')
                  .animate({scrollTop : $('#consoleScroller')[0].scrollHeight},
                           50);
            });

            // scope.$watch(function() { return $('.console').children().length;
            // },
            // function() {
            //// Wait for templates to render
            // scope.$evalAsync(function() {
            // var elem = $('.console');
            // elem.scroll(0, elem.offsetHeight);
            //});
            //});

          }
        };
      }
    ]);
