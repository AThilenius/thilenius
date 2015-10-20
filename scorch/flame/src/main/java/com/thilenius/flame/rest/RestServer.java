package com.thilenius.flame.rest;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.thilenius.blaze.proto.BlazeCommandProcessor;
import com.thilenius.blaze.proto.BlazeRequestWrapper;
import com.thilenius.blaze.proto.BlazeResponseWrapper;
import com.thilenius.blaze.proto.OperationFailure;
import com.thilenius.flame.Flame;
import com.thilenius.flame.entity.FlameActionTarget;
import com.thilenius.flame.entity.FlameActionTargetResponse;
import net.minecraftforge.common.DimensionManager;
import org.apache.thrift.TException;
import org.apache.thrift.async.AsyncMethodCallback;
import org.apache.thrift.server.TNonblockingServer;
import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TNonblockingServerSocket;
import org.apache.thrift.transport.TNonblockingServerTransport;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TServerTransport;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

public class RestServer {

    // Simple wrapper for a request, with the request and the async callback object
    public class BlazeCPRequest {
        public BlazeRequestWrapper RequestWrapper;
        public AsyncMethodCallback AsyncCallback;

        public BlazeCPRequest(BlazeRequestWrapper requestWrapper, AsyncMethodCallback asyncCallback) {
            RequestWrapper = requestWrapper;
            AsyncCallback = asyncCallback;
        }
    }

    // Simple handler that does nothing more than enqueue the request to the synchronized queue
    public class BlazeCPHandler implements BlazeCommandProcessor.AsyncIface {
        private List<BlazeCPRequest> m_receiveQueue;

        public BlazeCPHandler(List<BlazeCPRequest> receiveQueue) {
            m_receiveQueue = receiveQueue;
        }

        @Override
        public void Process(BlazeRequestWrapper request, AsyncMethodCallback resultHandler) throws TException {
            synchronized(m_receiveQueue) {
                m_receiveQueue.add(new BlazeCPRequest(request, resultHandler));
            }
        }
    }

    public BlazeCPHandler m_handler;
    public BlazeCommandProcessor.AsyncProcessor m_processor;

    private List<BlazeCPRequest> m_receiveQueue = new ArrayList<BlazeCPRequest>();
    private static final int PORT = 2500;

    public RestServer() {
        try {
            m_handler = new BlazeCPHandler(m_receiveQueue);
            m_processor = new BlazeCommandProcessor.AsyncProcessor(m_handler);
            new Thread(new Runnable() {
                public void run() {
                    try {
//                        TServerTransport serverTransport = new TServerSocket(PORT);
//                        TServer server = new TThreadPoolServer(
//                                new TThreadPoolServer.Args(serverTransport).processor(m_processor));
                        TNonblockingServerTransport serverTransport = new TNonblockingServerSocket(PORT);
                        TServer server = new TNonblockingServer(new TNonblockingServer.Args(serverTransport).
                                processor(m_processor));
                        System.out.println("Starting BlazeCP server on port " + PORT);
                        server.serve();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        } catch (Exception x) {
            x.printStackTrace();
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
            if (request.RequestWrapper.command_name.equals("action_target_invoke")) {
                List<String> args = request.RequestWrapper.args_json;
                if (args.size() < 3) {
                    System.out.println("Malformed action_target_invoke request. Expected 3 args");
                    request.AsyncCallback.onError(new OperationFailure(
                            "Malformed action_target_invoke request. Expected 3 args <dimension_id> <target> <json>"));
                    continue;
                }

                try {
                    int dimensionId = Integer.parseInt(args.get(0));
                    String target = args.get(1);
                    String json = args.get(2);
                            FlameActionTarget
                    actionTarget = Flame.Globals.EntityRegistry.getTargetByUsernameAndDimension(
                            request.RequestWrapper.minecraft_username, dimensionId, target);
                    if (actionTarget == null) {
                        System.out.println("Failed to find target of invocation for FlameAction");
                        request.AsyncCallback.onError(new OperationFailure(
                                "Failed to find target of invocation for FlameAction"));
                        continue;
                    }
                    // Invoke Action Path
                    Object returnValue = actionTarget.TargetMethod.invoke(actionTarget.TileEntity,
                            DimensionManager.getWorld(dimensionId), Flame.Globals.JsonObjectMapper.readTree(json));
                    // Non-Void return types
                    if (returnValue instanceof FlameActionTargetResponse) {
                        if (returnValue == null) {
                            System.out.println("Failed to invoke TargetMethod for FlameAction, target returned null");
                            request.AsyncCallback.onError(new OperationFailure("Internal Server Error"));
                            continue;
                        }
                        FlameActionTargetResponse flameActionTargetResponse = (FlameActionTargetResponse) returnValue;
                        if (flameActionTargetResponse.IsOnCoolDown) {
                            differedRequests.add(request);
                        } else if (flameActionTargetResponse.OperationFailure != null) {
                            request.AsyncCallback.onError(flameActionTargetResponse.OperationFailure);
                        } else {
                            // Finally, no fucking errors. This shit is ugly. As fuck
                            BlazeResponseWrapper response = new BlazeResponseWrapper();
                            response.json = Flame.Globals.JsonObjectMapper.writeValueAsString(
                                    flameActionTargetResponse.ReturnJson);
                            request.AsyncCallback.onComplete(response);
                        }
                        // Done
                        continue;
                    } else {
                        System.out.println("Invalid return type from FlameActionTarget! Got type: " +
                                returnValue.getClass().getName());
                        request.AsyncCallback.onError(new OperationFailure("Internal Server Error"));
                        continue;
                    }
                } catch (NumberFormatException e) {
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Malformed Dimension ID"));
                    continue;
                } catch (IllegalAccessException e) {
                    System.out.println("A fatal error was caught in Flame: " + e.getMessage() + ".");
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Internal Server Error"));
                    continue;
                } catch (JsonProcessingException e) {
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Malformed JSON"));
                    continue;
                } catch (IOException e) {
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Malformed JSON"));
                    continue;
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Malformed JSON"));
                    continue;
                } catch (Exception e) {
                    System.out.println("A fatal error was caught in Flame: " + e.getMessage() + ".");
                    e.printStackTrace();
                    request.AsyncCallback.onError(new OperationFailure("Internal Server Error"));
                    continue;
                }
            } else {
                request.AsyncCallback.onError(new OperationFailure(
                        "Unknown command type: " + request.RequestWrapper.command_name));
                continue;
            }
        }
    }

    private void dispatchEntityMessage(BlazeCPRequest request) {
        // Process out: String username, String actionPath, JsonNode json
    }

}
