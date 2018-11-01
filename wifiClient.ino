/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>
#include <Wire.h> //I2C library
#include <RtcDS3231.h> //RTC library
#include <string.h>
const char* ssid = "Xp";
const char* password = "x5233573";
int state,p6,p7,p8,counter;
RtcDS3231<TwoWire> rtcObject(Wire);
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  counter=0;
  state=1;
  p6=0;
  p7=0;
  p8=0;
  delay(10);
  rtcObject.Begin();
  // prepare GPIO2
  pinMode(D6, OUTPUT);
  digitalWrite(D6, 0);
  pinMode(D7, OUTPUT);
  digitalWrite(D7, 0);
  pinMode(D8, OUTPUT);
  digitalWrite(D8, 0);
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.print("server.available()=");
  Serial.println(server.available());
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
   //Serial.println("loop started");
  RtcTemperature temperature = rtcObject.GetTemperature(); //read temperature
   //Serial.println("temperature read");
 if(state==1){
   //Serial.println("first if run");
            if(temperature<10){
              if(p6=0){
                 digitalWrite(D6, 1);
                 p6=1;
                }
                 if(p7=0){
                 digitalWrite(D7, 1);
                 p7=1;
                }
                 if(p8=0){
                 digitalWrite(D8, 1);
                 p8=1;
                }
                               
              }
              else if(temperature>10 && temperature<15){
                   if(p6=1){
                 digitalWrite(D6, 0);
                 p6=0;
                }
                 if(p7=0){
                 digitalWrite(D7, 1);
                 p7=1;
                }
                 if(p8=0){
                 digitalWrite(D8, 1);
                 p8=1;
                }
                          
                }
              else if(temperature>15 && temperature<18){
                      if(p6=1){
                 digitalWrite(D6, 0);
                 p6=0;
                }
                 if(p7=1){
                 digitalWrite(D7, 0);
                 p7=0;
                }
                 if(p8=0){
                 digitalWrite(D8, 1);
                 p8=1;
                }
                            
                }
              else if(temperature>18){
                           if(p6=1){
                 digitalWrite(D6, 0);
                 p6=0;
                }
                 if(p7=1){
                 digitalWrite(D7, 0);
                 p7=0;
                }
                 if(p8=1){
                 digitalWrite(D8, 0);
                 p8=0;
                }
                         
                }
                delay(1);
  }
  //----------------------------------------------------------------
  //temperature.Print(Serial);
  //Serial.println(" C");
  // Check if a client has connected
  WiFiClient client = server.available();
  
  if (!client) {
     delay(1000);
     Serial.println(counter++);
     if(counter>100){
      counter=0;
      Serial.flush();
      }
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  int countAvail=0;
  while (!client.available()) {
    Serial.flush();
    Serial.print("Try To client.available()");
    Serial.println(countAvail++);
    delay(1000);
    if(countAvail>10)
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
 if(req.indexOf("/state/0") != -1){
  state=0;
  }
  else  if(req.indexOf("/state/1") != -1){
  state=1;
  }
  else if (req.indexOf("/l1/0") != -1 &&state==0) {
     digitalWrite(D6, 0);
     p6=0;
  } else if (req.indexOf("/l1/1") != -1&&state==0) {
     digitalWrite(D6, 1);
     p6=1;
  }
   else if (req.indexOf("/l2/0") != -1&&state==0) {
    digitalWrite(D7, 0);
    p7=0;
  }
   else if (req.indexOf("/l2/1") != -1&&state==0) {
    digitalWrite(D7, 1);
    p7=1;
  }
   else if (req.indexOf("/l3/0") != -1&&state==0) {
    digitalWrite(D8, 0);
    p8=0;
  }
   else if (req.indexOf("/l3/1") != -1&&state==0) {
    digitalWrite(D8, 1);
    p8=1;
  }
  
  else {
    Serial.println("invalid request");
    //client.stop();
    //return;
  }

  Serial.println("WiFiClient 2");
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
   s+="<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">";
   s+="<div class=\"row\" style=\"text-align:center\"><div style=\"border:solid 1px blue;margin:auto;width:50%;margin-top:30px;min-width:200px;border-radius:5px\" class=\"panel shadow p-3 mb-5 bg-white rounded\" id=\"info\"> ";
   s+="</br><div class=\"alert alert-info pull-right col-xs-12\">Temperature : "+String(temperature.AsFloatDegC(),2)+" C</div>";
   s+="<div class=\"alert alert-info pull-left col-xs-12\">State : ";
   s+=(state) ? "Automatic" : "Manual";
   s+="</div>";
   if(state==0){
       s+="<div  style=\"margin-bottom:10px\"><a href=\"/state/1\" class=\"btn btn-success\">Put Automatic</a></div>";
       s+="<div class=\"col-xs-12\"> ";
       s+="\r\nLamp 1 is now ";
       s += (p6) ? "<span class=\"badge badge-success\">ON</span> <a href=\"/l1/0\" class=\"btn btn-outline-info\">Off</a>" : "<span class=\"badge badge-secondary\">OFF</span> <a href=\"/l1/1\" class=\"btn btn-outline-warning\">ON</a>";
       s += "</br>\r\nLamp 2 is now ";
       s += (p7) ?"<span class=\"badge badge-success\">ON</span> <a href=\"/l2/0\" class=\"btn btn-outline-info\">Off</a>" : "<span class=\"badge badge-secondary\">OFF</span> <a href=\"/l2/1\" class=\"btn btn-outline-warning\">ON</a>";
       s += "</br>\r\nLamp 3 is now ";
       s += (p8) ? "<span class=\"badge badge-success\">ON</span> <a href=\"/l3/0\" class=\"btn btn-outline-info\">Off</a>" : "<span class=\"badge badge-secondary\">OFF</span> <a href=\"/l3/1\" class=\"btn btn-outline-warning\">ON</a>";
       s+="</div>";
    }
    else{
         s+="<div style=\"margin-bottom:10px\"><a href=\"/state/0\" class=\"btn btn-danger\">Put Manual</a></div>";
         s+="<div class=\"col-xs-12\"> ";
         s+="\r\nLamp 1 is now ";
         s += (p6) ? "<span class=\"badge badge-success\">ON</span> " : "<span class=\"badge badge-secondary\">OFF</span> ";
         s += "</br>\r\nLamp 2 is now ";
         s += (p7) ?"<span class=\"badge badge-success\">ON</span> " : "<span class=\"badge badge-secondary\">OFF</span> ";
         s += "</br>\r\nLamp 3 is now ";
         s += (p8) ? "<span class=\"badge badge-success\">ON</span> " : "<span class=\"badge badge-secondary\">OFF</span> ";
         s+="</div>";
      }
   s+="</div></div>";
   s += "</html>\n";

  // Send the response to the client
   Serial.println("WiFiClient 3");
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

