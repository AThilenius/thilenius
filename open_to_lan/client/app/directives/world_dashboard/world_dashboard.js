// Copyright 2015 Alec Thilenius
// All rights reserved.

angular.module('thilenius.world_dashboard', [])
    .directive('atWorldDashboard', [
      '$rootScope',
      'World',
      'Club',
      function($rootScope, World, Club) {
        return {
          templateUrl: 'app/directives/world_dashboard/world_dashboard.htm',
          link: function($scope, iElement, iAttrs) {

            $scope.saveWorld = function(world) { world.$save(); };

            $scope.removeWorld = function(world) {
              $scope.worlds = _($scope.worlds).without(world);
              world.$delete();
            };

            $scope.launchWorld = function(world) {
              console.log('Launching new server from world: ' +
                          JSON.stringify(world, null, 2) + '\nFrom Club: ' +
                          JSON.stringify($scope.activeClub, null, 2));
              $scope.servers.push(Club.servers.create(
                  {id: $scope.activeClub.id},
                  {config: 'NotAThingYet', worldId: world.id}));
            };

            $scope.removeServer = function(server) {
              $scope.servers = _($scope.servers).without(server);
              server.$delete();
            };

            $scope.getWorldFromServer = function(server) {
              return _($scope.worlds)
                  .find(function(world) {
                    return world.id === server.worldId;
                  });
            };

            $scope.findServerFromWorld = function(world) {
              return _($scope.servers)
                  .find(function(server) {
                    return server.worldId === world.id;
                  });
            };

            $scope.toDateString = function(date) {
              return new Date(date).toDateString();
            };

          }
        };
      }
    ]);
