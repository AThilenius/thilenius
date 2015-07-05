var thrift = require('thrift');
var helloSvc = require('./gen-nodejs/HelloSvc.js');

var options = {
   transport: thrift.TBufferedTransport,
   protocol: thrift.TJSONProtocol,
   path: "/services/thrift/hello",
   headers: {"Connection": "close"},
   https: false
};

var connection = thrift.createHttpConnection("10.17.8.101", 3000, options);
var client = thrift.createHttpClient(helloSvc, connection);

connection.on("error", function(err) {
   console.log("Error: " + err);
});

client.hello_func(function(error, result) {
   console.log("Msg from server: " + result);
});
