package org.mqttserver.services.http;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import org.mqttserver.policy.SystemController;

import java.util.ArrayDeque;
import java.util.Deque;

public class DataService extends AbstractVerticle {

    private final int port;
    private final SystemController systemController;

    private final int MAX_HISTORY = 200;
    private final Deque<Float> wlHistory = new ArrayDeque<>(MAX_HISTORY);

    public DataService(int port, SystemController systemController) {
        this.port = port;
        this.systemController = systemController;
    }

    @Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(BodyHandler.create());

        // Dashboard -> Java
        router.post("/api/mode").handler(this::handleSetMode);
        router.post("/api/valve").handler(this::handleSetValve);

        // Dashboard <- Java
        router.get("/api/systemdata").handler(this::handleGetSystemData);
        router.get("/api/wlhistory").handler(this::handleGetWlHistory);

        vertx.createHttpServer()
                .requestHandler(router)
                .listen(port, ar -> {
                    if (ar.succeeded()) {
                        System.out.println("HTTP Service ready on port: " + port);
                    } else {
                        System.err.println("HTTP Service FAILED: " + ar.cause().getMessage());
                    }
                });

        // salva WL ogni secondo per grafico
        vertx.setPeriodic(1000, id -> recordWlSample());
    }

    // Mode puo essere: AUTO, MANUAL_LOCAL, MANUAL_REMOTE
// Mode puo essere: AUTO, MANUAL_LOCAL, MANUAL_REMOTE
private void handleSetMode(RoutingContext ctx) {
    HttpServerResponse response = ctx.response();
    JsonObject body = ctx.body().asJsonObject();

    if (body == null || !body.containsKey("mode")) {
        response.setStatusCode(400).end("Missing field: mode");
        return;
    }

    String mode = body.getString("mode", "").trim().toUpperCase();

    switch (mode) {
        case "AUTO":
            systemController.setIsAutomatic();   
            // opzionale: invalida comando manuale
            // systemController.setValveValue(-1);
            response.setStatusCode(200).end();
            return;
        case "AUTOMATIC":
            systemController.setIsAutomatic();   
            // opzionale: invalida comando manuale
            // systemController.setValveValue(-1);
            response.setStatusCode(200).end();
            return;

        case "MANUAL_LOCAL":
            systemController.setIsManualLocal();
            response.setStatusCode(200).end();
            return;

        case "MANUAL_REMOTE":
            systemController.setIsManualRemote();
            response.setStatusCode(200).end();
            return;

        default:
            response.setStatusCode(400).end("Invalid mode. Use: AUTO, MANUAL_LOCAL, MANUAL_REMOTE");
    }
}


    private void handleSetValve(RoutingContext ctx) {
        HttpServerResponse response = ctx.response();
        JsonObject body = ctx.body().asJsonObject();

        if (body == null || !body.containsKey("valveValue")) {
            response.setStatusCode(400).end();
            return;
        }

        int valveValue = body.getInteger("valveValue", -1);

        // solo in MANUAL
        if (!systemController.getIsManual()) {
            response.setStatusCode(409).end("Valve can be set only in MANUAL mode");
            return;
        }

        systemController.setValveValue(valveValue);
        response.setStatusCode(200).end();
    }

    private void handleGetSystemData(RoutingContext ctx) {
        JsonObject data = new JsonObject()
                .put("systemStatus", String.valueOf(systemController.getSystemStatus()))
                .put("arduinoConnectionStatus", String.valueOf(systemController.getArduinoConnectionStatus()))
                .put("espConnectionStatus", String.valueOf(systemController.getEspConnectionStatus()))
                .put("isManualLocal", systemController.getIsManualLocal())
                .put("isManualRemote", systemController.getIsManualRemote())
                .put("isAutomatic", systemController.getIsAutomatic())
                .put("valveValue", systemController.getValveValue())
                .put("wl", systemController.getWl());

        System.out.println("GET /api/systemdata -> " + data.encode());

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(data.encode());
        //System.out.println("ValveValue" + systemController.getValveValue());
    }

    private void handleGetWlHistory(RoutingContext ctx) {
        int n = 50;
        try {
            String nParam = ctx.request().getParam("n");
            if (nParam != null) n = Math.max(1, Math.min(Integer.parseInt(nParam), MAX_HISTORY));
        } catch (Exception ignored) {}

        JsonArray arr = new JsonArray();
        Float[] values = wlHistory.toArray(new Float[0]);
        int start = Math.max(0, values.length - n);
        for (int i = start; i < values.length; i++) arr.add(values[i]);

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(arr.encode());
    }

    private void recordWlSample() {
        float wl = systemController.getWl();
        if (wlHistory.size() >= MAX_HISTORY) wlHistory.removeFirst();
        wlHistory.addLast(wl);
    }
}
