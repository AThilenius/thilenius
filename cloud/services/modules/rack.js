// Sample Usage:
//
//var rack = exports.create();

//rack.add(function(next, sarg) {
//console.log("one " + JSON.stringify(sarg));
//sarg.two = "zomg";
//next();
//});

//rack.add(function(next, sarg) {
//console.log("two " + JSON.stringify(sarg));
//next();
//});

//rack.finally(function(sarg) {
//console.log("finally " + JSON.stringify(sarg));
//});

//rack.execute({one:"hello"});

exports.create = function(/* actions ... */) {
  var rack = {
    rack_functions: [],
    rack_end: null,

    // Adds a new middleware to the rack
    add: function(rack_function) {
      this.rack_functions.push(rack_function);
    },

    // Adds an end action to the end of the rack
    finally: function(action) {
      this.rack_end = action;
    },

    // Executes the rack
    execute: function() {
      var rack_i = 0;
      var args_closure = arguments;
      var rack_functions = this.rack_functions;
      var rack_end = this.rack_end;
      var rack_iter = function() {
        if (rack_i < rack_functions.length) {
          var current_rack_function = rack_functions[rack_i++];
          current_rack_function(rack_iter, args_closure[0], args_closure[
              1], args_closure[2], args_closure[3], args_closure[4],
            args_closure[5], args_closure[6], args_closure[7]);
        } else if (rack_end !== null) {
          rack_end(args_closure[0], args_closure[1], args_closure[2],
            args_closure[3], args_closure[4], args_closure[5],
            args_closure[6], args_closure[7]);
        }
      };
      rack_iter(arguments);
    }
  };
  for (var i = 0; i < arguments.length; i++) {
    rack.add(arguments[i]);
  }
  return rack;
};
