var forgeApp = angular.module('forgeApp');

forgeApp.controller('loginController', [
  '$scope',
  '$location',
  'session',
  'user',
  function($scope, $location, session, user) {
    $scope.message = 'Look! I am an login page.';

    $scope.login = function(username, password) {
      if (!username || username.length === 0 || !password ||
          password.length === 0) {
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
            function(token) {
              $scope.$apply(function() {
                // Fetch User Data
                user.load(
                    function() {
                      console.log("Failed to fetch user data after logging in");
                    },
                    function(user_data) {
                      console.log("User logged in: " +
                                  JSON.stringify(user_data, null, 2));
                      $scope.$apply(function() {
                        $scope.username = $scope.password = null;
                        $location.path("/");
                      });
                    });
              });
            });
      }
    };
  }
]);
