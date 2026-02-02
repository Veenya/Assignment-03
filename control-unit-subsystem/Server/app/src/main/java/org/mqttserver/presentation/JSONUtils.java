package org.mqttserver.presentation;
import com.google.gson.Gson;

// Used to convert between:
//     Java objects -> JSON strings
//     JSON strings -> Java objects
public class JSONUtils {
    public static String objectToJson(Object object) {
        return gson.toJson(object);
    }

    public static <T> T jsonToObject(String json, Class<T> classOfT) {
        return gson.fromJson(json, classOfT);
    }
}
