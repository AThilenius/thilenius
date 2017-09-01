var boot = require('loopback-boot');
var loopback = require('loopback');
var path = require('path');

var app = module.exports = loopback();

app.start = function() {
  // Autoupdates
  var appModels = [
    'Person',
    'AccessToken',
    'ACL',
    'RoleMapping',
    'Role'
  ];
  var ds = app.dataSources.db;
  ds.isActual(appModels, function(err, actual) {
    if (!actual) {
      ds.autoupdate(appModels, function(err) {
        if (err) {
          throw (err);
        }
      });
    }
  });
  // Add paths
  app.use(loopback.static(path.resolve(__dirname, '../client')));
  app.use(loopback.static(path.resolve(__dirname, '../client/index.html')));
  // start the web server
  return app.listen(function() {
    app.emit('started');
    var baseUrl = app.get('url').replace(/\/$/, '');
    console.log('Web server listening at: %s', baseUrl);
    if (app.get('loopback-component-explorer')) {
      var explorerPath = app.get('loopback-component-explorer').mountPath;
      console.log('Browse your REST API at %s%s', baseUrl, explorerPath);
    }
  });
};

// Bootstrap the application, configure models, datasources and middleware.
// Sub-apps like REST API are mounted via boot scripts.
boot(app, __dirname, function(err) {
  if (err) {
    throw err;
  }
  // start the server if `$ node server.js`
  if (require.main === module) {
    app.start();
  }
});
