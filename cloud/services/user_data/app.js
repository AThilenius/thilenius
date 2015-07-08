//==============================================================================
//==  USER DATA SERVICE  =======================================================
//==============================================================================
var mongo = require('mongoskin');
var thrift = require('thrift');

var authentication = require('./gen_files/Authentication');
var user_data = require('./gen_files/UserData');
var auth_types = require('./gen_files/authentication_types');
var std_excep_types = require('./gen_files/standard_exceptions_types');
var user_data_types = require('./gen_files/user_data_types');

//===  MongoDB  ================================================================
var db = mongo.db("mongodb://" + process.env.DB_IP + ":" + process.env.DB_PORT +
  "/local", {
    native_parser: true
  });
var dbc = db.collection('user_data');
console.log("Using MongoDB at " + process.env.DB_IP + ":" + process.env.DB_PORT);

//===  Authentication Service  =================================================
var options = {
  transport: thrift.TBufferedTransport,
  protocol: thrift.TJSONProtocol,
  path: "/services/authentication/",
  headers: {
    "Connection": "close"
  },
  https: false
};
var connection = thrift.createHttpConnection(process.env.LB_IP,
  process.env.LB_PORT, options);
var auth_client = thrift.createHttpClient(authentication, connection);
connection.on("error", function(err) {
  console.log("Authentication Service Error: " + err);
});

//===  Service Implementation  =================================================
var userDataHandler = {

  create_user_data: function(ptoken, user_body, result) {
    // Auth check user
    auth_client.read_user(ptoken, function(auth_err, user_atom) {
      if (auth_err) {
        result(auth_err);
      } else {
        if (user_atom.permission_level === auth_types.PermissionLevel.UNTRUSTED) {
          // Untrusted token, cannot insert
          result(new std_excep_types.InsificientPermissions({
            failure_message: "Only a primary token can add a user data entry."
          }));
        } else {
          // Not an untrusted token, see if the user already exists
          dbc.findOne({
            uuid: user_atom.uuid
          }, function(db_err, user_data) {
            if (db_err) {
              result(new std_excep_types.InternalServerError());
            } else {
              if (user_data) {
                // Duplicate user
                result(new user_data_types.DuplicateUserFoundException({
                  user_atom: user_atom
                }));
              } else {
                // Create a new user
                dbc.insert({
                  uuid: user_atom.uuid,
                  user_body: user_body
                }, function(err, records) {
                  if (err) {
                    result(new std_excep_types.InternalServerError());
                  } else {
                    // All good
                    result();
                  }
                });
              }
            }
          });
        }
      }
    });
  },

  read_user_data: function(token, result) {
    // Auth check user
    auth_client.read_user(token, function(auth_err, user_atom) {
      if (auth_err) {
        result(auth_err);
      } else {
        if (user_atom.permission_level === auth_types.PermissionLevel.UNTRUSTED) {
          // Untrusted token, just return the atom
          result(null, new user_data_types.UserInfo({
            user_atom: user_atom
          }));
        } else {
          // Not an untrusted token, lookup all data and send it back
          dbc.findOne({
            uuid: user_atom.uuid
          }, function(db_err, user_data) {
            if (db_err) {
              result(new std_excep_types.InternalServerError());
            } else {
              if (user_data === null) {
                result(new user_data_types.UserBodyNotFoundException({
                  user_atom: user_atom
                }));
              } else {
                // Good to go
                result(null, new user_data_types.UserInfo({
                  user_atom: user_atom,
                  user_body: new user_data_types.UserBody(
                    user_data.user_body)
                }));
              }
            }
          });
        }
      }
    });
  },

  update_user_data: function(ptoken, user_body, result) {
    // Auth check user
    auth_client.read_user(ptoken, function(auth_err, user_atom) {
      if (auth_err) {
        result(auth_err);
      } else {
        if (user_atom.permission_level === auth_types.PermissionLevel.UNTRUSTED) {
          // Untrusted token, cannot update data
          result(new std_excep_types.InsificientPermissions({
            failure_message: "Only a primary token can modify user data."
          }));
        } else {
          // Not an untrusted token, modify all data
          dbc.update({
            uuid: user_atom.uuid
          }, {
            $set: user_body
          }, function(db_err, count, stat) {
            if (db_err) {
              result(new std_excep_types.InternalServerError());
            } else {
              if (!user_data) {
                result(new user_data_types.UserBodyNotFoundException({
                  user_atom: user_atom
                }));
              } else {
                // Good to go
                result(null, new user_data_types.UserInfo({
                  user_atom: user_atom,
                  user_body: new user_data_types.UserBody(
                    user_data)
                }));
              }
            }
          });
        }
      }
    });
  }
};

var userDataServiceOptions = {
  handler: userDataHandler,
  processor: user_data,
  protocol: thrift.TJSONProtocol,
  transport: thrift.TBufferedTransport
};

var serverOpt = {
  services: {
    "/": userDataServiceOptions
  }
};

//Create and start the web server
thrift.createWebServer(serverOpt).listen(80);
console.log("Http/Thrift Server running on port: 80");
