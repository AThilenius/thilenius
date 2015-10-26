// Copyright 2015 Alec Thilenius
// All rights reserved.

var CordStream = function(client, cordProto) {
  this.client = client;
  this.cordProto = cordProto;
  this.lestIndex = 0;
  this.fiberUrl =
      window.location.origin + "#/fiber/" + cordProto.uuid;
  this.callbacks = {
    close: []
  };
};

// TODO(athilenius): All callbacks in here should be registered like this except
// I also need to 'backfill' them
CordStream.prototype.addHandler = function(type, handler) {
  this.callbacks[type].push(handler);
};

// Called  for each Grain that is observed from the Cord
// Registering a new handler will duplicated any already handled Grains
// Note: GrainHandler takes an array of grains
CordStream.prototype.onGrain = function(grainHandler, endOfGrainHandler) {
  this.lastIndex = 0;
  var that = this;
  var readCordLambda = function() {
    that.client.ReadCordGTEIndex(that.cordProto, that.lastIndex, null)
        .fail(function(jqXhr, stat, error) {
          if (error.end_of_cord) {
            endOfGrainHandler();
            for (var i = 0; i < that.callbacks.close.length; i++) {
              that.callbacks.close[i]();
            }
          } else {
            // Ignore other errors and they could be things like timeouts
            readCordLambda();
          }
        })
        .done(function(result) {
          that.lastIndex += result.length;
          grainHandler(result);
          readCordLambda();
        });
  };
  readCordLambda();
};

// Data can be a single object or an array
CordStream.prototype.write = function(channel, data) {

};

// Closes the Cord, ignoring errors
CordStream.prototype.close = function() {

};
