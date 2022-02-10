#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#error "Board not found"
#endif
#include<Adafruit_MCP23X17.h>
#include <WebSocketsServer.h>

#include <ArduinoJson.h>
#include <Ticker.h> 

#define sliderbutton 16
int lastbuttonstate= 0;

Adafruit_MCP23X17 mcp0;
Adafruit_MCP23X17 mcp1;

char ssid[]= "vineet";
char password[]= "12345678";
char APssid[]= "NodeMCUwebserver";
char APpassword[]= "123456789";

String sdata;
char rdata;
float sensor1, sensor2, sensor3, sensor4;
int sensor5, sensor6;
String slider1_value = "0";
String slider2_value = "0";
String slider3_value = "0";
const char* input_parameter = "value";
String slider4_value = "0";
int f1,f2,f3,f4;
int r1,r2,r3,r4;

//mcp23017 portA
int InA1= 8;
int InA2= 9;
int InA3= 10;
int InA4= 11;
int InB1= 12;
int InB2= 13;
int InB3= 14;
int InB4= 15;

//on NodeMCU
#define ENA1 15
#define ENA2 13
#define ENA3 14
#define ENA4 12

//relay pins on MCP port B
int Rl1= 0;
int Rl2= 1;
int Rl3= 2;
int Rl4= 3;
int Rl5= 4;
int Rl6= 5;
int Rl7= 6;
int Rl8= 7;

void send_sensor();

char MAIN_page[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>
<meta name="viewport" content="width=device-width, initial-scale=0.86, maximum-scale=5.0, minimum-scale=0.86">
<style>
p {font-size: 2.0rem;}
.slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FF0000;
outline: none; -webkit-transition: .2s; transition: opacity .2s;}
.slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background:#01070a; cursor: pointer;}
.slider::-moz-range-thumb { width: 35px; height: 35px; background: #01070a; cursor: pointer; } 
</style>
<script>
var connection = new WebSocket('ws://'+location.hostname+':81/');
var Sensor1_data1 = 0;
var Sensor2_data2 = 0;
var Sensor3_data3 = 0;
var Sensor4_data4= 0;
var Sensor5_data5= 0;
var Sensor6_data6= 0;

var relay_1_status = 0;
var relay_2_status = 0;
var relay_3 status = 0;
var relay_4 status = 0;

connection.onmessage = function(event){
  var full_data = event.data;
  console.log(full_data);
  var data = JSON.parse(full_data);
  Sensor1_data1 = data.sensor1;
  Sensor2_data2 = data.sensor2;
  Sensor3_data3 = data.sensor3;
  Sensor4_data4 = data.sensor4;
  Sensor5_data5= data.sensor5;
  Sensor6_data6= data.sensor6;

  document.getElementById("Sensor1_meter1").value = Sensor1_data1;
  document.getElementById("Sensor1_value1").innerHTML = Sensor1_data1;
  document.getElementById("Sensor2_meter2").value = Sensor2_data2;
  document.getElementById("Sensor2_value2").innerHTML = Sensor2_data2;
  document.getElementById("Sensor3_meter3").value = Sensor3_data3;
  document.getElementById("Sensor3_value3").innerHTML = Sensor3_data3;
  document.getElementById("Sensor4_meter4").value = Sensor4_data4;
  document.getElementById("Sensor4_value4").innerHTML = Sensor4_data4;
  document.getElementById("Sensor5_meter5").value = Sensor5_data5;
  document.getElementById("Sensor5_value5").innerHTML = Sensor5_data5;
  document.getElementById("Sensor6_meter6").value = Sensor6_data6;
  document.getElementById("Sensor6_value6").innerHTML = Sensor6_data6;
}

function updateSliderPWM(element) {
  var slider1_value = document.getElementById("pwmSlider").value;
  document.getElementById("textslider_value").innerHTML = slider1_value;
  console.log(slider1_value);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+slider1_value, true);
  xhr.send();
}

function relay_1_on()
{
   relay_1_status = 1; 
  console.log("relay 1 is ON");
  send_data();
}
function relay_1_off()
{
  relay_1_status = 0;
console.log("relay 1 is OFF");
send_data();
}
function relay_2_on()
{
   relay_2_status = 1; 
  console.log("relay 2 is ON");
  send_data();
}
function relay_2_off()
{
  relay_2_status = 0;
console.log("relay 2 is OFF");
send_data();
}

function relay_3_on()
{
   relay_3_status = 1; 
  console.log("relay 3 is ON");
  send_data();
}
function relay_3_off()
{
  relay_3_status = 0;
console.log("relay 3 is OFF");
send_data();
}
function relay_3_on()
{
   relay_3_status = 1; 
  console.log("relay 3 is ON");
  send_data();
}
function relay_3_off()
{
  relay_3_status = 0;
console.log("relay 3 is OFF");
send_data();
}

function send_data()
{
  var full_data = '{"RELAY1" :'+relay_1_status+',"RELAY2":'+relay_2_status+',"RELAY3":'+relay_3_status+',"RELAY4":'+relay_4_status+'}';
  connection.send(full_data);
}

</script>
<body>
<center>
<h1>Virtual Cybertrons Home Automation</h1>
<div style="text-align: center;">
<h3> RELAY 1 </h3>
<button onclick= "relay_1_on()" >On</button><button onclick="relay_1_off()" >Off</button>
<h3> RELAY 2 </h3>
<button onclick="relay_2_on()">On</button><button onclick="relay_2_off()">Off</button>
<h3> RELAY 3 </h3>
<button onclick="relay_3_on()" >On</button><button onclick="relay_3_off()" >Off</button>
<h3> RELAY 4 </h3>
<button onclick="relay_4_on()">On</button><button onclick="relay_4_off()">Off</button>
<h2>Sensor1</h2>
<h4>Temperature</h4><meter value="2" min="0" max="100" id="Sensor1_meter1"> </meter><h4 id="Sensor1_value1" style="display: inline-block;"> 2 </h4>
<h2>Sensor2</h2>
<h4>Temperature</h4><meter value="2" min="0" max="100" id="Sensor2_meter2"> </meter><h4 id="Sensor2_value2" style="display: inline-block;"> 2 </h4>
<h2>Sensor3</h2>
<h4>Temperature</h4><meter value="2" min="0" max="100" id="Sensor3_meter3"> </meter><h4 id="Sensor3_value3" style="display: inline-block;"> 2 </h4>
<h2>Sensor4</h2>
<h4>Temperature</h4><meter value="2" min="0" max="100" id="Sensor4_meter4"> </meter><h4 id="Sensor4_value4" style="display: inline-block;"> 2 </h4>
<h2>Sensor5</h2>
<h4>Temperature</h4><meter value="2" min="0" max="100" id="Sensor5_meter5"> </meter><h4 id="Sensor5_value5" style="display: inline-block;"> 2 </h4>
<h2>Sensor6</h2>
<h4>percentage</h4><meter value="2" min="0" max="100" id="Sensor6_value6"> 
<h2 id="h_263954757891644485853459" style="text-align: center;">&nbsp;</h2>
<h2 style="text-align: center;"><strong><a href="#h_263954757891644485853459" target="_blank">FAN CONTROL</a></strong></h2>
  <h2>FAN1</h2>
  <p><span id="textslider_value">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
</center>
</body>
</html>
)rawliteral";

#include <ESPAsyncWebServer.h>

AsyncWebServer Server(80); // server port 80
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Page Not found");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

switch(type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      String message = String((char*)( payload));
      Serial.println(message);

      
DynamicJsonDocument doc(200);
DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int relay1_status = doc["RELAY1"];
  int relay2_status = doc["RELAY2"];
  int relay3_status = doc["RELAY3"];
  int relay4_status = doc["RELAY4"];

 mcp1.digitalWrite(Rl1, relay1_status);
 mcp1.digitalWrite(Rl2, relay2_status);
 mcp1.digitalWrite(Rl3, relay3_status);
 mcp1.digitalWrite(Rl4, relay4_status);
}
}

String processor(const String& var){
  if (var == "SLIDERVALUE"){
    return slider1_value;
  }
  return String();
}

void setup()
{
  Serial.begin(9600);
  mcp0.begin_I2C(0);
  mcp1.begin_I2C(1);
  mcp0.pinMode(InA1,OUTPUT);
  mcp0.pinMode(InA2,OUTPUT);
  mcp0.pinMode(InA3,OUTPUT);
  mcp0.pinMode(InA4,OUTPUT);
  mcp0.pinMode(InB1,OUTPUT);
  mcp0.pinMode(InB2,OUTPUT);
  mcp0.pinMode(InB3,OUTPUT);
  mcp0.pinMode(InB4,OUTPUT);
  pinMode(ENA1, OUTPUT);
  pinMode(ENA2, OUTPUT);
  pinMode(ENA3, OUTPUT);
  pinMode(ENA4, OUTPUT);
  mcp1.pinMode(Rl1, OUTPUT);
  mcp1.pinMode(Rl2, OUTPUT);
  mcp1.pinMode(Rl3, OUTPUT);
  mcp1.pinMode(Rl4, OUTPUT);
  
Serial.println("Use slider button to switch between STA and AP mode");
delay(2000);
}


void loop()
{ // AP MODE
  if(digitalRead(sliderbutton)==HIGH)
  {
   if(lastbuttonstate!=1)
   {delay(1000);
     WiFi.mode(WIFI_AP);
     Serial.println("switching to AP mode");
    WiFi.softAP(APssid,APpassword);
    IPAddress myIP= WiFi.softAPIP();
    Serial.println("switched to AP mode");
    Serial.println("IP address...");
    Serial.println(myIP);
      if (MDNS.begin("ESP")) { //esp.local/
    Serial.println("MDNS responder started");
  }
   Server.on("/", [](AsyncWebServerRequest * request)
  { request->send_P(200, "text/html", MAIN_page);});
  
  Server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(input_parameter)) {
      message = request->getParam(input_parameter)->value();
      slider1_value = message;
      analogWrite(ENA1, slider1_value.toInt());
      mcp0.digitalWrite(InA1, HIGH);
      mcp0.digitalWrite(InA2, LOW);
    }
    else {
      message = "No message sent";
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });
  Server.onNotFound(notFound);
  Server.begin();
  websockets.begin();
  websockets.onEvent(webSocketEvent);
   }
   send_sensor();
   websockets.loop();
   lastbuttonstate=1;
  }
  // STA MODE
  if(digitalRead(sliderbutton)==LOW)
  {
 if(lastbuttonstate!=0)
 {
   WiFi.mode(WIFI_STA);
   Serial.println("Switching to STA mode");
    WiFi.begin(ssid,password);
    delay(2000);
     if(WiFi.status()!= WL_CONNECTED)
  {
    for (int i = 0; i <10 ; i++)
    {
      delay(500);
      Serial.println(".");
    } 
  }
   Serial.print("WiFi is connected!!");
   Serial.print(WiFi.localIP());

   Server.on("/", [](AsyncWebServerRequest * request)
  { request->send_P(200, "text/html", MAIN_page);});
  
    Server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(input_parameter)) {
      message = request->getParam(input_parameter)->value();
      slider1_value = message;
      analogWrite(ENA1, slider1_value.toInt());
      mcp0.digitalWrite(InA1, HIGH);
      mcp0.digitalWrite(InA2, LOW);
    }
    else {
      message = "No message sent";
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });

  Server.onNotFound(notFound);
  Server.begin();
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  }
  send_sensor();
  websockets.loop();
  lastbuttonstate=0;
  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void send_sensor()
{
      if(Serial.available()>0)
  {
    rdata= Serial.read();
    sdata= sdata+rdata;
      if(rdata == '\n')
      {
        String a= getValue(sdata, ',', 0);
        String b= getValue(sdata, ',', 1);
        String c= getValue(sdata, ',', 3);
        String d= getValue(sdata, ',', 4);
        String e= getValue(sdata, ',', 5);
        String f= getValue(sdata, ',', 6);

        sensor1= a.toFloat();
        sensor2= b.toFloat();
        sensor3= c.toFloat();
        sensor4= d.toFloat();
        sensor5= e.toInt();
        sensor6= f.toInt();
        Serial.println(sensor1);
        Serial.println(sensor2);
        sdata="";
      }

String JSON_Data = "{\"Sensor1\":";
         JSON_Data += sensor1;
         JSON_Data += ",\"Sensor2\":";
         JSON_Data += sensor2;
         JSON_Data += ",\"Sensor3\":";
         JSON_Data += sensor3;
         JSON_Data += ",\"Sensor4\":";
         JSON_Data += sensor4;
         JSON_Data += ",\"Sensor5\":";
         JSON_Data += sensor5;
         JSON_Data += ",\"Sensor6\":";
         JSON_Data += sensor6;
         JSON_Data += "}";
  websockets.broadcastTXT(JSON_Data);
  }
}