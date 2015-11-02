package com.thilenius.flame.rest;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import com.thilenius.flame.Flame;
import com.thilenius.flame.entity.FlameActionTarget;
import com.thilenius.flame.entity.FlameActionTargetResponse;
import net.minecraftforge.common.DimensionManager;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;

public class RestServer {
    class HttpJsonHandler implements HttpHandler {
        public void handle(HttpExchange t) {
            try {
                String json = org.apache.commons.io.IOUtils.toString(t.getRequestBody());
                BlazeCPRequest blazeRequest = Flame.Globals.JsonObjectMapper.readValue(json, BlazeCPRequest.class);
                blazeRequest.HttpExchange = t;
                synchronized (m_receiveQueue) {
                    m_receiveQueue.add(blazeRequest);
                    m_receiveQueue.notify();
                }
            } catch (IOException e) {
                try {
                    byte[] data = Flame.Globals.JsonObjectMapper.writeValueAsBytes(
                            FlameActionTargetResponse.fromOperationFailure("Malformed JSON"));
                    t.getResponseHeaders().add("Content-Type", "application/json");
                    t.getResponseHeaders().add("Access-Control-Allow-Origin", "*");
                    t.sendResponseHeaders(200, data.length);
                    OutputStream os = t.getResponseBody();
                    os.write(data);
                    os.close();
                    e.printStackTrace();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
        }
    }

    private HttpServer m_server = null;
    private List<BlazeCPRequest> m_receiveQueue = new ArrayList<BlazeCPRequest>();
    private static final int PORT = 2500;

    public RestServer() {
        try {
            m_server = com.sun.net.httpserver.HttpServer.create(new InetSocketAddress(PORT), 0);
            m_server.createContext("/", new HttpJsonHandler());
            m_server.setExecutor(Executors.newFixedThreadPool(64));
            m_server.start();
            System.out.println("Blaze HTTP Proto/Json server listening on port " + PORT);
        } catch (IOException e) {
            System.out.println("Failed to start server on port 80. Trying port " + PORT);
            e.printStackTrace();
        }
    }

    public void onGameTick() {
        List<BlazeCPRequest> allWaiting = new ArrayList<BlazeCPRequest>();
        synchronized(m_receiveQueue) {
            while(!m_receiveQueue.isEmpty()) {
                allWaiting.add(m_receiveQueue.remove(0));
            }
        }
        dispatchAllRequests(allWaiting);
    }

    private void dispatchAllRequests(List<BlazeCPRequest> requests) {
        List<BlazeCPRequest> differedRequests = new ArrayList<BlazeCPRequest>();
        for (BlazeCPRequest request : requests) {
                try {
                        FlameActionTarget
                actionTarget = Flame.Globals.EntityRegistry.getTargetByUsernameAndDimension(
                        request.MinecraftUsername, request.Dimension, request.Target);
                if (actionTarget == null) {
                    System.out.println("Failed to find target of invocation for FlameAction");
                    request.respond(FlameActionTargetResponse.fromOperationFailure(
                            "Failed to find target of invocation for FlameAction"));
                    continue;
                }
                // Invoke Action Path
                Object returnValue = actionTarget.TargetMethod.invoke(actionTarget.TileEntity,
                        DimensionManager.getWorld(request.Dimension), request.Json);
                if (returnValue == null) {
                    System.out.println("Failed to invoke TargetMethod for FlameAction, target returned null");
                    request.respond(FlameActionTargetResponse.fromOperationFailure("Internal Server Error"));
                    continue;
                }
                FlameActionTargetResponse flameActionTargetResponse = (FlameActionTargetResponse) returnValue;
                if (flameActionTargetResponse.IsOnCoolDown) {
                    differedRequests.add(request);
                } else {
                    request.respond(flameActionTargetResponse);
                }
            } catch (InvocationTargetException e) {
                e.printStackTrace();
                request.respond(FlameActionTargetResponse.fromOperationFailure("Malformed JSON"));
                continue;
            } catch (Exception e) {
                System.out.println("A fatal error was caught in Flame: " + e.getMessage() + ".");
                e.printStackTrace();
                request.respond(FlameActionTargetResponse.fromOperationFailure("Internal Server Error"));
                continue;
            }
        }
        synchronized(m_receiveQueue) {
            while(!differedRequests.isEmpty()) {
                m_receiveQueue.add(differedRequests.remove(0));
            }
        }
    }

}
