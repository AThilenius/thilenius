var app = angular.module('app');

app.controller('consoleController', [
  '$rootScope',
  '$scope',
  'Person',
  'Club',
  'World',
  'Server',
  function($rootScope, $scope, Person, Club, World, Server) {
    $scope.activeClub = null;
    $scope.activeContent = 'club';
    $scope.activePerson = Person.getCurrent();
    $scope.clubs = Person.clubs({id: Person.getCurrentId()});
    $scope.worlds = World.find({});
    $scope.servers = Server.find({});

    $scope.addMember = function() {
      if (!$scope.activeClub.members) {
        $scope.activeClub.members = [];
      }
      $scope.activeClub.members.push({username: 'Unnamed'});
    };

    $scope.saveClub = function() {
      // Prune members
      var members = $scope.activeClub.members;
      if (members) {
        $scope.activeClub.members = _(members).reject(function(m) {
          return m.username === 'Unnamed' || m.username === '';
        });
      }
      $scope.activeClub.$save();
    };

  }
]);
