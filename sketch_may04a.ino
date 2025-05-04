#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// === RGB Setup ===
#define RGB_PIN     48
#define NUM_PIXELS  1
Adafruit_NeoPixel rgb_led(NUM_PIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// === AP WiFi Setup ===
const char* ssid = "ESP32_RGB_Control";
const char* password = "12345678";

WebServer server(80);

// === HTML Page ===
const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>ESP32 RGB LED Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body {
      background: linear-gradient(to bottom, #1e1e2f, #12121a);
      color: #ffffff;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      text-align: center;
      margin: 0;
      padding: 0;
    }

    h1 {
      margin-top: 40px;
      font-size: 2em;
      color: #00e6e6;
      text-shadow: 1px 1px 5px #000;
    }

    .container {
      margin-top: 40px;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    input[type="color"] {
      width: 180px;
      height: 180px;
      border: none;
      border-radius: 16px;
      box-shadow: 0 4px 20px rgba(0, 255, 255, 0.3);
      cursor: pointer;
      transition: transform 0.2s ease;
    }

    input[type="color"]:hover {
      transform: scale(1.05);
    }

    .footer {
      margin-top: 60px;
      font-size: 0.9em;
      color: #aaa;
    }
  </style>
</head>
<body>
  <h1>ESP32 RGB LED Controller</h1>
  <div class="container">
    <input type="color" id="colorPicker">
  </div>
  <div class="footer">Made with ❤️ using ESP32-S3</div>

  <script>
    const picker = document.getElementById("colorPicker");
    picker.addEventListener("input", function() {
      const color = picker.value.substring(1);
      const r = parseInt(color.substring(0, 2), 16);
      const g = parseInt(color.substring(2, 4), 16);
      const b = parseInt(color.substring(4, 6), 16);
      fetch(`/setColor?r=${r}&g=${g}&b=${b}`);
    });
  </script>
</body>
</html>
)rawliteral";


// === Handle Routes ===
void handleRoot() {
  server.send(200, "text/html", html_page);
}

void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    rgb_led.setPixelColor(0, rgb_led.Color(r, g, b));
    rgb_led.show();
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing color parameters");
  }
}

void setup() {
  // Start RGB
  rgb_led.begin();
  rgb_led.setBrightness(50);
  rgb_led.show(); // Clear

  // Start Wi-Fi AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.begin(115200);
  Serial.println("Access Point started");
  Serial.print("IP address: ");
  Serial.println(IP);

  // Setup web server
  server.on("/", handleRoot);
  server.on("/setColor", handleSetColor);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
