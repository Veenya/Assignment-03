#include "MsgService.h"
#include "Arduino.h"

String content;

MsgServiceClass MsgService;

bool MsgServiceClass::isMsgAvailable() {
    return msgAvailable;
}

Msg* MsgServiceClass::receiveMsg() {
    if (msgAvailable) {
        Msg* msg = currentMsg;
        msgAvailable = false;
        currentMsg = NULL;
        content = "";
        return msg;
    } else {
        return NULL;
    }
}

void MsgServiceClass::init() {
    Serial.begin(9600);
    content.reserve(256);
    content = "";
    currentMsg = NULL;
    msgAvailable = false;
}

void MsgServiceClass::sendMsg(const String& msg) {
    Serial.println(msg);
}

void serialEvent() {
    /* reading the content */
    while (Serial.available()) {
        char ch = (char)Serial.read();
        if (ch == '\n') {
            MsgService.currentMsg = new Msg(content);
            MsgService.msgAvailable = true;
        } else {
            content += ch;
        }
    }
}

bool MsgServiceClass::isMsgAvailable(Pattern& pattern) {
    return (msgAvailable && pattern.match(*currentMsg));
}

Msg* MsgServiceClass::receiveMsg(Pattern& pattern) {
    if (msgAvailable && pattern.match(*currentMsg)) {
        Msg* msg = currentMsg;
        msgAvailable = false;
        currentMsg = NULL;
        content = "";
        return msg;
    } else {
        return NULL;
    }
}

bool MsgServiceClass::isJsonMsgAvailable() {
    if (!msgAvailable) return false;
    if (currentMsg == nullptr) return false;
    
    String c = currentMsg->getContent();
    c.trim();
    // Controllo molto basilare: inizia con { e finisce con }
    return c.length() >= 2 && c[0] == '{' && c[c.length()-1] == '}';
}

JsonDocument* MsgServiceClass::receiveJson() {
    if (!isJsonMsgAvailable()) return nullptr;

    String jsonStr = currentMsg->getContent();
    JsonDocument* doc = new JsonDocument();

    DeserializationError error = deserializeJson(*doc, jsonStr);

    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        delete doc;
        msgAvailable = false;
        currentMsg = nullptr;
        content = "";
        return nullptr;
    }

    // consuma il messaggio
    delete currentMsg;
    currentMsg = nullptr;
    msgAvailable = false;
    content = "";

    return doc;
}