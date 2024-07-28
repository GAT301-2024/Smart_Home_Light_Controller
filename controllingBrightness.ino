#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const int ledPin = 27;
WebServer server(80);

// HTML content for the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 LED Brightness Control</title>
  <script>
    function sliderChange(){
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/brightness?value="+document.getElementById("slider").value, true);
      xhr.send();
    }
  </script>
</head>
<body>
<h2>LED Brightness Control</h2>
<p>Adjust the brightness:</p>
<input type="range" min="0" max="255" id="slider" onchange="sliderChange()" />
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });

  // Route for changing LED brightness
  server.on("/brightness", HTTP_GET, []() {
    if (server.hasArg("value")) {
      String value = server.arg("value");
      analogWrite(ledPin, value.toInt());
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
