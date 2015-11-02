package com.thilenius.flame.entity;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.JsonNode;
import com.thilenius.flame.Flame;

import java.io.IOException;

// An instance of a FlameActionTargetResponse is returned by all FlameActionTargets
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(fieldVisibility = JsonAutoDetect.Visibility.NONE)
public class FlameActionTargetResponse {

    @JsonProperty("is_exception")
    public boolean IsException;

    @JsonProperty("json")
    public JsonNode Json;

    public boolean IsOnCoolDown = false;

    public static FlameActionTargetResponse fromOnCoolDown() {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.IsOnCoolDown = true;
        return flameActionTargetResponse;
    }

    public static FlameActionTargetResponse fromOperationFailure(String message) {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.IsException = true;
        try {
            flameActionTargetResponse.Json =
                    Flame.Globals.JsonObjectMapper.readTree("{\"user_message\":\"" + message + "\"}");
        } catch (IOException e) {
            e.printStackTrace();
        }
        return flameActionTargetResponse;
    }

    public static FlameActionTargetResponse fromJson(JsonNode json) {
        FlameActionTargetResponse flameActionTargetResponse = new FlameActionTargetResponse();
        flameActionTargetResponse.Json = json;
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
        flameActionTargetResponse.Json = jsonNode;
        return flameActionTargetResponse;
    }

    private FlameActionTargetResponse() { }
}
