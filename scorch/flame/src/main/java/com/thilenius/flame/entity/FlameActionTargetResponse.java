package com.thilenius.flame.entity;

import com.fasterxml.jackson.databind.JsonNode;
import com.thilenius.blaze.proto.OperationFailure;
import com.thilenius.flame.Flame;

import java.io.IOException;

// An instance of a FlameActionTargetResponse is returned by all FlameActionTargets
public class FlameActionTargetResponse {
    public boolean IsOnCoolDown = false;
    public OperationFailure OperationFailure;
    public JsonNode ReturnJson;

    public static FlameActionTargetResponse fromOnCoolDown() {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.IsOnCoolDown = true;
        return flameActionTargetResponse;
    }

    public static FlameActionTargetResponse fromOperationFailure(String message) {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.OperationFailure = new OperationFailure(message);
        return flameActionTargetResponse;
    }

    public static FlameActionTargetResponse fromJson(JsonNode json) {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.ReturnJson = json;
        return flameActionTargetResponse;
    }

    public static FlameActionTargetResponse fromJson(String json) {
        JsonNode jsonNode = null;
        try {
            jsonNode = Flame.Globals.JsonObjectMapper.readTree(json);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.ReturnJson = jsonNode;
        return flameActionTargetResponse;
    }

    private FlameActionTargetResponse() { }
}
