#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Develop";
const char *password = "384783478";
const int output = 2;
String sliderValue = "0";
const char *PARAM_INPUT = "value";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css">
    <script src="https://code.jquery.com/jquery-1.11.3.min.js"></script>
    <script src="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
</head>

<body>

    <div data-role="page">
        <div data-role="header">
            <h1>Strömungspumpe</h1>
        </div>

        <div data-role="main" class="ui-content">
            <div data-role="rangeslider">
                <label for="price-min">Winkel:</label>
                <input type="range" name="price-min" id="price-min" value="45" min="0" max="180">
                <label for="price-max">Winkel:</label>
                <input type="range" name="price-max" id="price-max" value="135" min="0" max="180">
            </div>
        </div>
    </div>

</body>

</html>
)rawliteral";
String processor(const String &var)
{
  if (var == "SLIDERVALUE")
  {
    return sliderValue;
  }
  return String();
}

void setup()
{
  Serial.begin(115200);
  analogWrite(output, sliderValue.toInt());
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      sliderValue = inputMessage;
      analogWrite(output, sliderValue.toInt());
    }
    else
    {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  server.begin();
}

void loop()
{
}
