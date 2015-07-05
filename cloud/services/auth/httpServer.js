var thrift = require('thrift');
var authentication = require('./gen_files/Authentication');
var atypes = require('./gen_files/authentication_types');

var authenticationHandler = {
  create_user: function(email_address, password, result) {
    console.log("Create user called");
    console.log(email_address);
    console.log(password);

    result(null);
  },

  read_user: function(ptoken, result) {
    result(null);
  },

  delete_user: function(ptoken, result) {
    result(null);
  },

  create_ptoken: function(email_address, password, result) {
    result(null);
  },

  create_stoken: function(ptoken, result) {
    result(null);
  }
};

var authenticationServiceOptions = {
  handler: authenticationHandler,
  processor: authentication,
  protocol: thrift.TJSONProtocol,
  transport: thrift.TBufferedTransport
};

var serverOpt = {
  services: { "/": authenticationServiceOptions }
};

//Create and start the web server
thrift.createWebServer(serverOpt).listen(80);
console.log("Http/Thrift Server running on port: 80");
