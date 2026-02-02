package org.mqttserver.services.http;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import org.mqttserver.services.mqtt.Broker;

import java.util.ArrayDeque;
import java.util.Deque;

public class DataService extends AbstractVerticle {

    private final int port;
    private final Broker broker;

    // store last N WL samples for dashboard graph
    private final int MAX_HISTORY = 200;
    private final Deque<Float> wlHistory = new ArrayDeque<>(MAX_HISTORY);

    public DataService(int port, Broker broker) {
        this.port = port;
        this.broker = broker;
    }

    @Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(BodyHandler.create());

        // Dashboard -> CUS
        router.post("/api/mode").handler(this::handleSetMode);      // { "isManual": true/false }
        router.post("/api/valve").handler(this::handleSetValve);    // { "valveValue": 0..100 } (only if MANUAL)

        // Dashboard <- CUS
        router.get("/api/systemdata").handler(this::handleGetSystemData);
        router.get("/api/wlhistory").handler(this::handleGetWlHistory); // ?n=50

        vertx.createHttpServer().requestHandler(router).listen(port);
        System.out.println("HTTP Service ready on port: " + port);

        // Periodically capture WL for history (simple approach).
        // If you already have WL updates elsewhere, you can push into history there instead.
        vertx.setPeriodic(1000, id -> recordWlSample());
    }

    private void handleSetMode(RoutingContext ctx) {
        HttpServerResponse response = ctx.response();
        JsonObject body = ctx.body().asJsonObject();

        if (body == null || !body.containsKey("isManual")) {
            sendError(400, response);
            return;
        }

        boolean isManual = body.getBoolean("isManual", false);
        broker.getSystemController().setIsManual(isManual);

        // When switching back to automatic, manual valve command should not be used
        if (!isManual) {
            broker.getSystemController().setValveValueFromDashboard(-1);
        }

        response.setStatusCode(200).end();
    }

    private void handleSetValve(RoutingContext ctx) {
        HttpServerResponse response = ctx.response();
        JsonObject body = ctx.body().asJsonObject();

        if (body == null || !body.containsKey("valveValue")) {
            sendError(400, response);
            return;
        }

        int valveValue = body.getInteger("valveValue", -1);

        // Spec: valve can be controlled remotely only in MANUAL mode
        if (!broker.getSystemController().getIsManual()) {
            response.setStatusCode(409) // conflict with current mode
                    .end("Valve can be set only in MANUAL mode");
            return;
        }

        broker.getSystemController().setValveValueFromDashboard(valveValue);
        response.setStatusCode(200).end();
    }

    private void handleGetSystemData(RoutingContext ctx) {
        JsonObject data = new JsonObject();
        data.put("status", String.valueOf(broker.getSystemController().getStatus())); // as string
        data.put("isManual", broker.getSystemController().getIsManual());
        data.put("valveValue", broker.getSystemController().getValveValue());
        data.put("wl", broker.getSystemController().getWl());

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(data.encodePrettily());
    }

    private void handleGetWlHistory(RoutingContext ctx) {
        int n = 50;
        try {
            String nParam = ctx.request().getParam("n");
            if (nParam != null) n = Math.max(1, Math.min(Integer.parseInt(nParam), MAX_HISTORY));
        } catch (Exception ignored) {}

        JsonArray arr = new JsonArray();
        // take last n elements
        Float[] values = wlHistory.toArray(new Float[0]);
        int start = Math.max(0, values.length - n);
        for (int i = start; i < values.length; i++) {
            arr.add(values[i]);
        }

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(arr.encodePrettily());
    }

    private void recordWlSample() {
        float wl = broker.getSystemController().getWl();
        if (wlHistory.size() >= MAX_HISTORY) {
            wlHistory.removeFirst();
        }
        wlHistory.addLast(wl);
    }

    private void sendError(int statusCode, HttpServerResponse response) {
        response.setStatusCode(statusCode).end();
    }
}
