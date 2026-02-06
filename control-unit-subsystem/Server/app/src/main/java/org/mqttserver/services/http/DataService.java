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

    private void handleSetMode(RoutingContext ctx) {
        HttpServerResponse response = ctx.response();
        JsonObject body = ctx.body().asJsonObject();

        if (body == null || !body.containsKey("isManual")) {
            response.setStatusCode(400).end();
            return;
        }

        boolean isManual = body.getBoolean("isManual", false);
        systemController.setIsManual(isManual);

        // se torni in AUTO, invalida eventuale comando manuale
        if (!isManual) {
            systemController.setValveValueFromDashboard(-1);
        }

        response.setStatusCode(200).end();
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

        systemController.setValveValueFromDashboard(valveValue);
        response.setStatusCode(200).end();
    }

    private void handleGetSystemData(RoutingContext ctx) {
        JsonObject data = new JsonObject()
                .put("status", String.valueOf(systemController.getStatus()))
                .put("isManual", systemController.getIsManual())
                .put("valveValue", systemController.getValveValue())
                .put("wl", systemController.getWl());

        ctx.response()
                .putHeader("content-type", "application/json")
                .end(data.encode());
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
