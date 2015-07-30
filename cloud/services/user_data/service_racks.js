//==============================================================================
//==  USER DATA SERVICE RACKS  =================================================
//==============================================================================
var rack = require('modules/rack');

var user_data = require('./gen_files/UserData');
var auth_types = require('./gen_files/authentication_types');
var std_excep_types = require('./gen_files/standard_exceptions_types');
var user_data_types = require('./gen_files/user_data_types');

exports.create_racks = function(dbc, auth_client) {
  //===  Rack Middleware =======================================================
  var authentication_middleware = function(next, req, result) {
    auth_client.read_user(req.token, function(auth_err, user_atom) {
      if (auth_err) {
        result(auth_err);
      } else {
        if (user_atom.permission_level ===
            auth_types.PermissionLevel.UNTRUSTED) {
          // Untrusted tokens cannot be used for UserData service
          result(new std_excep_types.InsificientPermissions({
            failure_message : "UserData requires primary tokens for all calls"
          }));
        } else {
          req.user_atom = user_atom;
          next();
        }
      }
    });
  };

  var user_data_query_middleware = function(next, req, result) {
    dbc.findOne({uuid : req.user_atom.uuid}, function(db_err, user_data) {
      if (db_err) {
        result(new std_excep_types.InternalServerError());
      } else {
        req.user_data = user_data;
        next();
      }
    });
  };

  //===  Racks  ================================================================
  return {
    create_user_rack : rack.create(
        authentication_middleware, user_data_query_middleware,
        function(next, req, result) {
          console.log("Creating user_data for " +
                      JSON.stringify(req.user_body, null, 2));
          if (req.user_data) {
            // Duplicate user
            result(new user_data_types.DuplicateUserFoundException(
                {user_atom : req.user_atom}));
          } else {
            // Create a new user
            dbc.insert({uuid : req.user_atom.uuid, user_body : req.user_body},
                       function(err, records) {
                         if (err) {
                           result(new std_excep_types.InternalServerError());
                         } else {
                           // All good
                           result();
                         }
                       });
          }
        }),

    read_user_rack : rack.create(
        authentication_middleware, user_data_query_middleware,
        function(next, req, result) {
          if (req.user_data === null) {
            result(new user_data_types.UserBodyNotFoundException(
                {user_atom : req.user_atom}));
          } else {
            // Good to go
            var user_info = new user_data_types.UserInfo({
              user_atom : req.user_atom,
              user_body : new user_data_types.UserBody(req.user_data.user_body)
            });

            result(null, user_info);
          }
        }),

    update_user_rack : rack.create(
        authentication_middleware,
        function(next, req, result) {
          dbc.update({uuid : req.user_atom.uuid},
                     {$set : req.user_body}, function(db_err, count, stat) {
                       if (db_err) {
                         result(new std_excep_types.InternalServerError());
                       } else if (count === 0) {
                         result(new user_data_types.UserBodyNotFoundException(
                             {user_atom : req.user_atom}));
                       } else {
                         result();
                       }
                     });
        })
  };
};
