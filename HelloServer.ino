#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "UPC6349762";
const char* password = "EFTRJBEQ";

const boolean IS_DEBUG = true;

ESP8266WebServer server(80);
int led = -1;
String rememberedMessage = "";

/* Handlers BEGIN */
void handleRoot() {
  digitalWrite(BUILTIN_LED, LOW);
  if(server.argName(0) != NULL){
    led=server.arg(0).toInt();
  }
  /*
  Serial.println("/n");
  Serial.println(server.argName(0));
  Serial.println("/n");
  Serial.println(server.arg(0).toInt());
  */
  String message2 = "Current value is ";
  message2 += led;
  server.send(200, "text/plain", message2);
  digitalWrite(BUILTIN_LED, HIGH);
}

void handleBlip() {
  String state;
  if(readParamOrDie("state", state)) {
    changeState(state.toInt());
  }
  respondOk(state);
}

void handleTestParam() {
  String action, value;
  if(readParamOrDie("action", action)) {
    if(action == "put" || action == "save" || action == "set") {
      if(readParamOrDie("val", value)) {
        rememberedMessage = value;
        respondOk("saved");
      }
    }
    else if(action == "read" || action == "get") {
      respondOk(rememberedMessage);
    }
    else if(action == "clear") {
      rememberedMessage = "";
      respondOk("OK");
    }
    else {
      respond(200, "Unknown action: " + action);
    }
  }
}

void handleNotFound(){
  digitalWrite(BUILTIN_LED, LOW);
  String message;
  if(IS_DEBUG){
    message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  }
  else {
    message = "<h1>HTTP 404 - Not found</h1><p><img src='https://http.cat/404'></p>";
  }
  respondHTML(404, message);
  
  //digitalWrite(BUILTIN_LED, HIGH); // this line shows us that server changed something
}

/* Handlers END */

/* HTTP helpers BEGIN */

String readParam(String paramName){
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == paramName) {
      return server.arg(i);
    }
  }
  return "";
}

boolean readParamOrDie(String paramName, String& param) {
  param = readParam(paramName);
  if(param.length() == 0) {
    // ded
    if(IS_DEBUG) {
      server.send(422, "text/plain", "Missing parameter: '" + paramName + "'");
    }
    else {
      respondHTML(422, "<h1>HTTP 422 - Unprocessable Entity</h1><p><img src='https://http.cat/422'></p>");
    }
    return false;
  }
  return true;
}

void respond(int code, String message) {
  server.send(code, "text/plain", message);
}

void respondHTML(int code, String message) {
  server.send(code, "text/html", message);
}

void respondOk(String message) {
  respond(200, message);
}

/* HTTP helpers END */

void changeState(int state)
{
  if(state%2==0)
  {
  digitalWrite(BUILTIN_LED, HIGH);
  }
  else if (state%2==1)
  {
  digitalWrite(BUILTIN_LED, LOW);  
  }
}

void setup(void){
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("1");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("2");
  }
  Serial.println("3");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/blip", handleBlip);

  server.on("/testParam", handleTestParam);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  changeState(led);
}
