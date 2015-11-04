// Copyright 2015 Alec Thilenius
// All rights reserved.

var CordStream = function(client, cordProto) {
  this.client = client;
  this.cordProto = cordProto;
  this.lestIndex = 0;
  this.fiberUrl =
      window.location.origin + "#/fiber/" + cordProto.uuid;
  this.observedGrains = [];
  this.cordClosed = false;
  // Array of { grainHandler: function, endOfCordHandler: function }
  this.onGrainHandlers = [];
  // Begin pulling grains
  this.pullGrains();
};

// Called for each Grain that is observed from the Cord
// Registering a new handler will duplicated any already handled Grains
// Note: GrainHandler takes an array of grains
CordStream.prototype.onGrain = function(grainHandler, endOfGrainHandler) {
  for (var i = 0; i < this.observedGrains.length; i++) {
    grainHandler(this.observedGrains[i]);
  }
  if (this.cordClosed) {
    endOfGrainHandler();
  }
  // Add it to the handler list
  this.onGrainHandlers.push({
    grainHandler: grainHandler,
    endOfGrainHandler: endOfGrainHandler
  });
};

// private
CordStream.prototype.pullGrains = function() {
  this.lastIndex = 0;
  var that = this;
  var readCordLambda = function() {
    that.client.ReadCordGTEIndex(that.cordProto, that.lastIndex, null)
        .fail(function(jqXhr, stat, error) {
          if (error.end_of_cord) {
            that.cordClosed = true;
            for (var i = 0; i < that.onGrainHandlers.length; i++) {
              that.onGrainHandlers[i].endOfGrainHandler();
            }
          } else {
            // Ignore other errors and they could be things like timeouts
            readCordLambda();
          }
        })
        .done(function(result) {
          that.observedGrains.push(result);
          that.lastIndex += result.length;
          for (var i = 0; i < that.onGrainHandlers.length; i++) {
            that.onGrainHandlers[i].grainHandler(result);
          }
          readCordLambda();
        });
  };
  readCordLambda();
};
