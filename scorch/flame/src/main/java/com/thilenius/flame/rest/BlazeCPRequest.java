package com.thilenius.flame.rest;


import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.sun.net.httpserver.HttpExchange;
import com.thilenius.flame.Flame;
import com.thilenius.flame.entity.FlameActionTargetResponse;

import java.io.IOException;
import java.io.OutputStream;

// Simple wrapper for a request, with the request and the async callback object
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(fieldVisibility = JsonAutoDetect.Visibility.NONE)
public class BlazeCPRequest {

    @JsonProperty(value = "user_uuid", required = false)
    public String UserUUID;

    @JsonProperty("minecraft_username")
    public String MinecraftUsername;

    @JsonProperty("dimension")
    public int Dimension;

    @JsonProperty("target")
    public String Target;

    @JsonProperty("json")
    public JsonNode Json;

    public com.sun.net.httpserver.HttpExchange HttpExchange;

    public void respond(FlameActionTargetResponse response) {
        try {
            byte[] data = Flame.Globals.JsonObjectMapper.writeValueAsBytes(response);
            HttpExchange.getResponseHeaders().add("Content-Type", "application/json");
            HttpExchange.getResponseHeaders().add("Access-Control-Allow-Origin", "*");
            HttpExchange.sendResponseHeaders(200, data.length);
            OutputStream os = HttpExchange.getResponseBody();
            os.write(data);
            os.close();
        } catch (JsonProcessingException e) {
            System.out.println("Failed to send JSON response to client:");
            e.printStackTrace();
        } catch (IOException e) {
            System.out.println("Failed to send JSON response to client:");
            e.printStackTrace();
        }
    }
}