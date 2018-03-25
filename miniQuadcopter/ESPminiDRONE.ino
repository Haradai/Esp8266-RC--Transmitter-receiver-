#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

//defining constants
const char *ssid = "ESPminiDRONE!";
const char *password = "sudomakemeasandwich";
const char *domain = "drone-controller";
//here we are going to write our webpage:
const char *html = "<h1>ESP8266 MiniDrone Controller!</h1>";

//controll values
int throttle;
int jaw;
int roll;
int pitch;
int WIFImode = 0;

ESP8266WebServer server(80); //set the webserver to be in port 80 in the esp8266

void getdata(){ //receive data from the client
  String data = server.arg("plain");
  StaticJsonBuffer<200> jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  String throttlep = jObject["throttle"];
  String jawp = jObject["jaw"];
  String rollp = jObject["roll"];
  String pitchp = jObject["pitch"];

  //asigning the adquired data to the correspondent variable.
  throttle = throttlep.toInt(); //we have to convert the adquired variable(string) to int
  jaw = jawp.toInt();
  roll = rollp.toInt();
  pitch = pitchp.toInt();
  
  
}
void webpage(){
  server.send(200, "text/html",html);
  
}
void setup() {
  delay(1000);
  Serial.begin(115200); //start serial communication
  Serial.println("Hi father! <3 ");
  Serial.print("Creating access point: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password); //create access point
  Serial.print("password: ");
  Serial.println(password);
  

  IPAddress myIP = WiFi.softAPIP(); //define Ip address
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", webpage);
  server.begin(); //start webserver
  Serial.println("HTTP server started! :)");

  if (!MDNS.begin(domain)) { //create 
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.print("You can controll me through this webpage:  ");
  Serial.print(domain);
  Serial.println(".local");
  
}
void loop(){
  server.handleClient();
  getdata();
}

