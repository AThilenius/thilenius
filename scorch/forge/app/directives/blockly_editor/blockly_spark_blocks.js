var importText = 'import scorch.anvil as anvil';

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
    this.setTooltip('');
  }
};

Blockly.Python.spark_move = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.move_spark(0, \"' + dropdown_direction + '\")';
  Blockly.Python.definitions_.import_anvil = importText;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Python.ORDER_NONE];
};

Blockly.Blocks.spark_turn = {
  init: function() {
    this.appendDummyInput()
        .appendField("Turn Spark")
        .appendField(
            new Blockly
                .FieldDropdown([["Left", "TurnLeft"], ["Right", "TurnRight"]]),
            "DIRECTION");
    this.setOutput(true, "Boolean");
    this.setColour(240);
    this.setTooltip('');
  }
};

Blockly.Python.spark_turn = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.move_spark(0, \"' + dropdown_direction + '\")';
  Blockly.Python.definitions_.import_anvil = importText;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Python.ORDER_NONE];
};

Blockly.Blocks.spark_mine = {
  init: function() {
    this.appendDummyInput().appendField("Mine").appendField(
        new Blockly.FieldDropdown(
            [["Forward", "Forward"], ["Up", "Up"], ["Down", "Down"]]),
        "DIRECTION");
    this.setOutput(true, "Boolean");
    this.setColour(240);
    this.setTooltip('');
  }
};

Blockly.Python.spark_mine = function(block) {
  var dropdown_direction = block.getFieldValue('DIRECTION');
  var code = 'anvil.mine_spark(0, \"' + dropdown_direction + '\")';
  Blockly.Python.definitions_.import_anvil = importText;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Python.ORDER_NONE];
};

Blockly.Blocks.spark_drop_items = {
  init: function() {
    this.appendDummyInput().appendField("Drop All Items In Bag");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('');
  }
};

Blockly.Python.spark_drop_items = function(block) {
  var code = 'anvil.drop_items_spark(0)\n';
  Blockly.Python.definitions_.import_anvil = importText;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Python.ORDER_NONE];
};

Blockly.Blocks.spark_recall = {
  init: function() {
    this.appendDummyInput().appendField("Teleport Back To Pad");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setColour(240);
    this.setTooltip('');
  }
};

Blockly.Python.spark_recall = function(block) {
  var code = 'anvil.recall_spark(0)\n';
  Blockly.Python.definitions_.import_anvil = importText;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Python.ORDER_NONE];
};
