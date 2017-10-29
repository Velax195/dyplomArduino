#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "UPC6349762";
const char* password = "EFTRJBEQ";

ESP8266WebServer server(80);
int led=rand();

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
void handleNotFound(){
  digitalWrite(BUILTIN_LED, LOW);
  String message = "File Not Found\n\n";
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
  server.send(404, "text/plain", message);
  digitalWrite(BUILTIN_LED, HIGH);
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
