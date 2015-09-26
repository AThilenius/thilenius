var forgeApp = angular.module('forgeApp');

forgeApp.controller('loginController', [
  '$scope',
  '$location',
  'session',
  'crucible',
  function($scope, $location, session, crucible) {
    $scope.message = 'Look! I am an login page.';
    $scope.login = function(username, password) {
      if (isBlank(username) || isBlank(password)) {
        $scope.error = "Missing Fields";
        $scope.is_invalid = true;
      } else {
        session.login(
            username, password,
            function(error) {
              console.log("Got error: " + JSON.stringify(error, null, 2));
              $scope.$apply(function() {
                $scope.error = "Invalid Username or Password";
                $scope.is_invalid = true;
                $scope.password = "";
              });
            },
            function(token, user) {
              // DO NOT SUBMIT - Get repo headers
              crucible.client.GetRepoHeadersByUser(
                                  session.session_token, function() {})
                  .fail(function(jqXHR, status, error) {
                    console.log("Crucible error: " + error);
                  })
                  .done(function(repo_headers) {
                    console.log("Got repo headers: " +
                                JSON.stringify(repo_headers, null, 2));
                  });
              $scope.$apply(function() {
                $scope.username = null;
                $scope.password = null;
                $location.path("/");
              });
            });
      }
    };
  }
]);
