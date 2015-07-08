//==============================================================================
//==  AUTHENTICATION SERVICE  ==================================================
//==============================================================================
var crypto = require('crypto');
var mongo = require('mongoskin');
var thrift = require('thrift');

var authentication = require('./gen_files/Authentication');
var auth_types = require('./gen_files/authentication_types');
var std_excep_types = require('./gen_files/standard_exceptions_types');

//===  MongoDB  ================================================================
var db = mongo.db("mongodb://" + process.env.DB_IP + ":" + process.env.DB_PORT +
  "/local", {
    native_parser: true
  });
var dbc = db.collection('auth_atoms');
console.log("Using MongoDB at " + process.env.DB_IP + ":" + process.env.DB_PORT);

//===  Service Implementation  =================================================
var authenticationHandler = {
  create_user: function(email_address, password, result) {
    // Pre-process some fields
    var emailAddress = email_address.replace(/ /g, '').toLowerCase();
    if (password.length < 8) {
      result(new auth_types.PasswordInvalidException({
        message: "Password must be 8 characters or more"
      }));
      return;
    }
    dbc.find({
      email_address: emailAddress
    }).toArray(function(err, items) {
      if (items.length !== 0) {
        // User already exists.
        result(new auth_types.EmailInvalidException({
          message: "A user with the email address already exists"
        }));
        return;
      } else {
        // User doesn't already exist, create it
        var user = {
          email_address: emailAddress,
          password: password,
          permissions: auth_types.PermissionLevel.UNTRUSTED
        };
        dbc.insert(user, function(err, records) {
          result(null, null);
          console.log("New account created: " +
            JSON.stringify(user, null, 2));
          return;
        });
      }
    });
  },

  read_user: function(token, result) {
    dbc.findOne({
      $or: [{
        primary_token: token
      }, {
        secondary_tokens: token
      }, ]
    }, function(err, user) {
      if (err) {
        result(new std_excep_types.InternalServerError());
        return;
      }
      if (user) {
        if (user.primary_token === token) {
          // Primary Token
          result(null, new auth_types.UserAtom({
            uuid: user._id.toString(),
            permission_level: user.permissions,
            email_address: user.email_address
          }));
        } else {
          // Secondary Token
          result(null, new auth_types.UserAtom({
            uuid: user._id.toString(),
            permission_level: auth_types.PermissionLevel.UNTRUSTED
          }));
        }
      } else {
        result(new auth_types.AuthenticationException({
          failure_message: "Invalid token.",
          error_code: 403
        }));
      }
    });
  },

  delete_user: function(ptoken, result) {
    dbc.remove({
      primary_token: ptoken
    }, function(err, count, stat) {
      if (err) {
        result(new std_excep_types.InternalServerError());
        return;
      }
      if (count > 0) {
        result(null, null);
      } else {
        result(new auth_types.AuthenticationException({
          failure_message: "Invalid token.",
          error_code: 403
        }));
      }
    });
  },

  create_ptoken: function(email_address, password, result) {
    // Normalize fields
    var emailAddress = email_address.replace(/ /g, '').toLowerCase();
    // Generate a crypto safe 48 byte token
    crypto.randomBytes(48, function(ex, buf) {
      var newToken = buf.toString('hex');
      dbc.update({
          email_address: emailAddress,
          password: password
        }, {
          $set: {
            primary_token: newToken
          }
        },
        function(err, count, stat) {
          if (err) {
            result(new std_excep_types.InternalServerError());
            return;
          }
          if (count === 0) {
            result(new auth_types.AuthenticationException({
              failure_message: "Invalid username or password.",
              error_code: 403
            }));
          } else {
            result(null, newToken);
          }
        });
    });
  },

  create_stoken: function(ptoken, result) {
    crypto.randomBytes(48, function(ex, buf) {
      var newToken = buf.toString('hex');
      dbc.update({
          primary_token: ptoken
        }, {
          $addToSet: {
            secondary_tokens: newToken
          }
        },
        function(err, count, stat) {
          if (err) {
            result(new auth_types.AuthenticationException({
              failure_message: "Internal server error",
              error_code: 500
            }));
            return;
          }
          if (count === 0) {
            result(new auth_types.AuthenticationException({
              failure_message: "Invalid token.",
              error_code: 403
            }));
          } else {
            result(null, newToken);
          }
        });
    });
  }
};

var authenticationServiceOptions = {
  handler: authenticationHandler,
  processor: authentication,
  protocol: thrift.TJSONProtocol,
  transport: thrift.TBufferedTransport
};

var serverOpt = {
  services: {
    "/": authenticationServiceOptions
  }
};

//Create and start the web server
thrift.createWebServer(serverOpt).listen(80);
console.log("Http/Thrift Server running on port: 80");
