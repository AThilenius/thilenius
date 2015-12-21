var importText = 'import scorch.anvil as anvil';

// ===  Spark Move  ============================================================
Blockly.Blocks.spark_move = {
  init: function() {
    this.appendDummyInput()
        .appendField("Move Spark")
        .appendField(new Blockly.FieldDropdown([
          ["Forward", "Forward"],
          ["Backward", "Backward"],
          ["Up", "Up"],
          ["Down", "Down"]
        ]),
                     "DIRECTION");
    this.setOutput(true, "Boolean");
    this.setColour(240);
    this.setTooltip('Moves a Spark, and checks if it was able to move.');
  }
};

Blockly.Python.spark_move = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.move_spark(0, \"' + dropdown_direction + '\")';
  Blockly.Python.definitions_.import_anvil = importText;
  return [code, Blockly.Python.ORDER_FUNCTION_CALL];
};

// ===  Spark Move No Return  ==================================================
Blockly.Blocks.spark_move_no_return = {
  init: function() {
    this.appendDummyInput()
        .appendField("Move Spark")
        .appendField(new Blockly.FieldDropdown([
          ["Forward", "Forward"],
          ["Backward", "Backward"],
          ["Up", "Up"],
          ["Down", "Down"]
        ]),
                     "DIRECTION");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('Moves a Spark, ignoring if it was able to do so.');
  }
};

Blockly.Python.spark_move_no_return = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.move_spark(0, \"' + dropdown_direction + '\")\n';
  Blockly.Python.definitions_.import_anvil = importText;
  return code;
};

// ===  Spark Turn No Return  ==================================================
Blockly.Blocks.spark_turn_no_return = {
  init: function() {
    this.appendDummyInput()
        .appendField("Turn Spark")
        .appendField(
            new Blockly
                .FieldDropdown([["Left", "TurnLeft"], ["Right", "TurnRight"]]),
            "DIRECTION");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('Turns a Spark left or right 90 degrees.');
  }
};

Blockly.Python.spark_turn_no_return = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.move_spark(0, \"' + dropdown_direction + '\")\n';
  Blockly.Python.definitions_.import_anvil = importText;
  return code;
};

// ===  Spark Mine =============================================================
Blockly.Blocks.spark_mine = {
  init: function() {
    this.appendDummyInput().appendField("Mine").appendField(
        new Blockly.FieldDropdown(
            [["Forward", "Forward"], ["Up", "Up"], ["Down", "Down"]]),
        "DIRECTION");
    this.setOutput(true, "Boolean");
    this.setColour(240);
    this.setTooltip(
        'Tells the Spark to mine, and checks if it was able to mine the block.');
  }
};

Blockly.Python.spark_mine = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.mine_spark(0, \"' + dropdown_direction + '\")';
  Blockly.Python.definitions_.import_anvil = importText;
  return [code, Blockly.Python.ORDER_FUNCTION_CALL];
};

// ===  Spark Mine No Return  ==================================================
Blockly.Blocks.spark_mine_no_return = {
  init: function() {
    this.appendDummyInput().appendField("Mine").appendField(
        new Blockly.FieldDropdown(
            [["Forward", "Forward"], ["Up", "Up"], ["Down", "Down"]]),
        "DIRECTION");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip(
        'Tells the Spark to mine, but ignores if it was able to do so.');
  }
};

Blockly.Python.spark_mine_no_return = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.mine_spark(0, \"' + dropdown_direction + '\")\n';
  Blockly.Python.definitions_.import_anvil = importText;
  return code;
};

// ===  Spark Drop Items  ======================================================
Blockly.Blocks.spark_drop_items = {
  init: function() {
    this.appendDummyInput().appendField("Drop All Items In Bag");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('Drops all items in the Spark\'s inventory infront of it.');
  }
};

Blockly.Python.spark_drop_items = function(block) {
  var code = 'anvil.drop_items_spark(0)\n';
  Blockly.Python.definitions_.import_anvil = importText;
  return code;
};

// ===  Spark Recall  ==========================================================
Blockly.Blocks.spark_recall = {
  init: function() {
    this.appendDummyInput().appendField("Teleport Back To Pad");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('Teleports the Spark back to it\'s pad.');
  }
};

Blockly.Python.spark_recall = function(block) {
  var code = 'anvil.recall_spark(0)\n';
  Blockly.Python.definitions_.import_anvil = importText;
  return code;
};
