#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

//defining constants
const char *ssid = "------------------";
const char *password = "----------------------";
const char *domain = "controller";
//here we are going to write our webpage:
const char *html = "<!DOCTYPE html>"
                   "<html>"

                   "<script>"

                   "var throttle, pitch, roll, jaw;"
                   "var LX,LY,RX,RY;"
                   "var CanvLXcent, CanvLYcent,CanvRXcent,CanvRYcent;"

                   "function init(){"
                   "    canvasright = document.getElementById('canvas-right');"
                   "     canvasleft = document.getElementById('canvas-left');"
                   "      div = document.getElementById('controls');"

                   "       canvasleft.addEventListener('touchstart', touchstartL); "
                   "        canvasleft.addEventListener('touchmove', touchmoveL); "

                   "canvasright.addEventListener('touchstart', touchstartR); "
                   "canvasright.addEventListener('touchmove', touchmoveR); "
                   "canvasright.addEventListener('touchend', touchendR); "

                   "CanvLXcent = div.offsetLeft + 200;"
                   " CanvLYcent = div.offsetTop + 200;"

                   "  CanvRXcent = div.offsetWidth - 200;"
                   "   CanvRYcent = div.offsetTop + 200;"

                   "setInterval(sendata, 33.3);"

                   
                   "var ipValue = 'ws://controller.local:81';"
                   "connection = new WebSocket(ipValue);  "


                   "}"
                   "function touchstartR(ev){"
                   "ev.preventDefault();"
                   "var touch = ev.touches[1]; "
                   "roll = parseInt(touch.clientX)-CanvRXcent;"
                   " pitch = parseInt(touch.clientY)-CanvRYcent;"
                   "  drawjoystickR();"

                   "}"
                   "function touchmoveR(ev){"
                   "  ev.preventDefault();"
                   "   var touch = ev.touches[1];"
                   "    roll = parseInt(touch.clientX)-CanvRXcent;"
                   "     pitch = parseInt(touch.clientY)-CanvRYcent;"
                   "      drawjoystickR();"



                   "    }"
                   "function touchstartL(ev){"
                   "ev.preventDefault();"
                   "var touch = ev.touches[0];"
                   "jaw = parseInt(touch.clientX)-CanvLXcent;"
                   " throttle = parseInt(touch.clientY)-CanvLYcent;"
                   "  drawjoystickL();"

                   "}"
                   "function touchmoveL(ev){"
                   "ev.preventDefault();"
                   "var touch = ev.touches[0];"
                   "jaw = parseInt(touch.clientX)-CanvLXcent;"
                   " throttle = parseInt(touch.clientY)-CanvLYcent;"
                   "  drawjoystickL();"

                   "}"
                   "function drawjoystickR(){"
                   "if (canvasright.getContext) {"
                   "    var ctx = canvasright.getContext('2d');"
                   "     ctx.clearRect(0,0,400,400);"
                   "      ctx.fillRect(128+roll, 128+pitch, 150, 150);"
                   "   }"

                   "}"
                   "function drawjoystickL(){"
                   "if (canvasleft.getContext) {"
                   "    var ctxx = canvasleft.getContext('2d');"
                   "     ctxx.clearRect(0,0,400,400);"
                   "      ctxx.fillRect(128+jaw, 128+throttle, 150, 150);"
                   "   }"
                   "}"
                   "function touchendR(ev){"
                   " pitch=0;"
                   "  roll=0;"
                   "   drawjoystickR();"

                   "}"
                   "function sendata(){"
                   "var data = JSON.stringify({"
                   " throttle: throttle,"
                   "pitch: pitch,"
                   " jaw: jaw,"
                   "  roll: roll});"
                   " connection.send(data); "

                   "   }"

                   "</script>"
                   "<head>"
                   "  <title>ESP8266 miniDrone Controller</title>"
                   "  <meta name='viewport' content='user-scalable=no'/>  "
                   "</head>"

                   "<body id = 'body' onload='init()'>"
                   "  <div id='controls'>"
                   "<canvas id='canvas-right' width='400' height='400'></canvas>"
                   "<canvas id='canvas-left' width='400' height='400'></canvas>"
                   "   </div>"
                   "</body>"
                   "<style>"
                   "#title{"
                   "  text-align: center;"
                   "   font-size: 25px; "
                   "}"
                   "#body{"
                   "   height:100%;"
                   "}"
                   "#controls{"
                   "  position:absolute;"
                   "  bottom:0;"
                   "   width: 99%;"
                   "}"
                   "#canvas-right{"
                   "float:right;"
                   "height:400px;"
                   "width:400px;"
                   "border:2px solid #888;"
                   "border-radius:4px;"
                   " position:relative; /* Necessary for correct mouse co-ords in Firefox */"
                   "  background-color:grey;"
                   "   opacity:0.5;"
                   "}"
                   "#canvas-left{"
                   "float:left;"
                   "height:400px;"
                   "width:400px;"
                   "border:2px solid #888;"
                   "border-radius:4px;"
                   "position:relative; /* Necessary for correct mouse co-ords in Firefox */"
                   "background-color:grey;"
                   "opacity:0.5;"
                   "}"
                   "</style>"
                   "</html>";

WebSocketsServer webSocket = WebSocketsServer(81);

unsigned int localPort = 81; //port where websocket listens


//controll values
int throttle;
int jaw;
int roll;
int pitch;
int WIFImode = 0;

ESP8266WebServer server(80); //set the webserver to be in port 80 in the esp8266

void getdata(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { //receive data from the client
  switch (type) {
    case WStype_DISCONNECTED: //if its not connected do nothing
      break;

    case WStype_CONNECTED: { //if its connected get the ip of the client
        IPAddress ip = webSocket.remoteIP(num);
      }
      break;

    case WStype_TEXT: { //if data is received
        String data = String((char *) &payload[0]);
        StaticJsonBuffer<200> jBuffer; //create JsonBuffer
        JsonObject& jObject = jBuffer.parseObject(data); //make "data" be in the JsonBuffer
        String throttlep = jObject["throttle"]; //search each value by its tag name
        String jawp = jObject["jaw"];
        String rollp = jObject["roll"];
        String pitchp = jObject["pitch"];

        //asigning the adquired data to the correspondent variable.
        throttle = throttlep.toInt(); //we have to convert the adquired variable(string) to int
        jaw = jawp.toInt();
        roll = rollp.toInt();
        pitch = pitchp.toInt();
        Serial.println(throttle);
      }
  }
}
void webpage() {
  server.send(200, "text/html", html);

}
void setup() {
  Serial.begin(115200); //start serial communication
  Serial.println("Hi father! <3 ");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Wait for connection to be stablished
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", webpage);
  server.begin(); //start webserver
  Serial.println("HTTP server started! :)");

  Serial.println("Creating local dns domain...");
  if (!MDNS.begin(domain)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS responder started");
  Serial.print("You can controll me through this webpage:  ");
  Serial.print(domain);
  Serial.println(".local");

  Serial.println("webSocket started");
  webSocket.begin();
  webSocket.onEvent(getdata);


}
void loop() {
  server.handleClient();
  webSocket.loop();
}
