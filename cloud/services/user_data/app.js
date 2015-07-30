//==============================================================================
//==  USER DATA SERVICE  =======================================================
//==============================================================================
var authentication = require('./gen_files/Authentication');
var mongo = require('mongoskin');
var thrift = require('thrift');

var service_racks = require('./service_racks');
var user_data = require('./gen_files/UserData');

//===  MongoDB  ================================================================
var db = mongo.db("mongodb://" + process.env.DB_IP + ":" + process.env.DB_PORT +
                      "/local",
                  {native_parser : true});
var dbc = db.collection('user_data');
console.log("Using MongoDB: " + process.env.DB_IP + ":" + process.env.DB_PORT);

//===  Authentication Service  =================================================
var options = {
  transport : thrift.TBufferedTransport,
  protocol : thrift.TJSONProtocol,
  path : "/services/authentication/",
  headers : {"Connection" : "close"},
  https : false
};
var connection = thrift.createHttpConnection(process.env.LB_IP,
                                             process.env.LB_PORT, options);
var auth_client = thrift.createHttpClient(authentication, connection);
connection.on("error", function(err) {
  console.log("Authentication Service Error: " + err);
});

//===  Service Implementation  =================================================
var racks = service_racks.create_racks(dbc, auth_client);
var user_data_handler = {
  create_user_data : function(token, user_body, result) {
    racks.create_user_rack.execute({token : token, user_body : user_body},
                                   result);
  },

  read_user_data : function(token, result) {
    racks.read_user_rack.execute({token : token}, result);
  },

  update_user_data : function(token, user_body, result) {
    racks.update_user_rack.execute({token : token, user_body : user_body},
                                   result);
  }
};

var server_opt = {
  services : {
    "/" : {
      handler : user_data_handler,
      processor : user_data,
      protocol : thrift.TJSONProtocol,
      transport : thrift.TBufferedTransport
    }
  }
};

// Create and start the web server
thrift.createWebServer(server_opt).listen(80);
console.log("Http/Thrift Server running on port: 80");
