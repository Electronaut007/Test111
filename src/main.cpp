#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>        



#define LED 13

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">  <title>ESP8266 Websocket</title>
   
  <script language="javascript">
   function check() {
      val = document.getElementById("s1").checked;
      if (val)
        webSocket.send('1');
      else 
        webSocket.send('0');
    }

    var gwUrl = "ws://" + location.host + "/ws";
    var webSocket = new WebSocket(gwUrl);
    webSocket.onopen = function(e) {
        console.log("open");
    }
    webSocket.onclose = function(e) {
        console.log("close");
    }
  
    webSocket.onmessage = function(e) {
      console.log("message");
      var data = e.data;
      if (data == '1')
        document.getElementById('s1').checked = true;
      else
       document.getElementById('s1').checked = false;
    }
 </script>
  
  <style>
    h2 {background: #3285DC;
        color: #FFFFFF;
        align:center;
    }
  
    .content {
        border: 1px solid #164372;
        padding: 5px;
    }
    
    .button {
       background-color: #00b300; 
       border: none;
       color: white;
       padding: 8px 10px;
       text-align: center;
       text-decoration: none;
       display: inline-block;
       font-size: 14px;
  }
  
  /* The switch - the box around the slider */
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

/* Hide default HTML checkbox */
.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

/* The slider */
.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}
  </style>
</head>
<body>
  <h2>ESP8266 Websocket</h2>
  <div class="content">
     <p><h4>Turn on Led</h4>
     <label class="switch">
        <input type="checkbox" id="s1" onchange="check()" />
        <span class="slider round"></span>
     </label>
   </p>
  </div>
</body>
</html>
)rawliteral"; 

// Web server running on port 80
AsyncWebServer server(80);
// Web socket
AsyncWebSocket ws("/ws");



void notifyClient(int status) {
    ws.textAll(String((char) status));
}

void handlingIncomingData(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    if (data[0] == '1')
      digitalWrite(LED, HIGH);
    else
      digitalWrite(LED, LOW);
    
    notifyClient(data[0]);
  }
  
}

// Callback for incoming event
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
             void * arg, uint8_t *data, size_t len){
   switch(type) {
      case WS_EVT_CONNECT:
        Serial.printf("Client connected: \n\tClient id:%u\n\tClient IP:%s\n", 
             client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
         Serial.printf("Client disconnected:\n\tClient id:%u\n", client->id());
         break;
      case WS_EVT_DATA:
         handlingIncomingData(arg, data, len);
         break;
      case WS_EVT_PONG:
          Serial.printf("Pong:\n\tClient id:%u\n", client->id());
          break;
      case WS_EVT_ERROR:
          Serial.printf("Error:\n\tClient id:%u\n", client->id());
          break;     
   }
  
}

void setup() {
  Serial.begin(9600);
 WiFi.begin("vineet","12345678");

   Serial.print("Connecting...");
   while(WiFi.status() != WL_CONNECTED)
   {
     delay(500);
     Serial.print("waiting to connect..");
   }
   Serial.print("WiFi is connected!!");
   Serial.print(WiFi.localIP());

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.on("/post", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, NULL);
  });
 
  server.begin();

  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);
  
}


void loop() {
 ws.cleanupClients();
}