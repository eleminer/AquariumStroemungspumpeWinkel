#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Develop";
const char *password = "384783478";


String speedfirstButton="50";
String speedsecondButton="1";
String speedthirdButton="2";
String speedfourButton="3";
String speedfiveButton="4";
String maxValueAngle="160";
String minValueAngle="10";
String status="1";


const char* PARAM_INPUT = "value";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        #angle-min, #angle-max {
            color: #2196F3;
        }
        .switch {
            position: relative;
            display: inline-block;
            width: 60px;
            height: 34px;
        }
        .switch input { 
            opacity: 0;
            width: 0;
            height: 0;
        }
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
        .slider.round {
            border-radius: 34px;
        }
        .slider.round:before {
            border-radius: 50px;
        }
        #onoffdiv {
            margin-top: 6px;
            margin-left: 6px;
        }
        .button {
            background-color: #2196F3; /* Green */
            border: none;
            color: white;
            padding: 0.4%;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 20px;
            width: 140px;
            transform: scale(1);
            margin-bottom: 10px;
        }
        .button-group {
            position: relative;
            margin-top: 10px;
            margin-bottom: 0px;
            text-align: center;
        }
        #speedDIV {
            position: relative;
            margin-top: 100px;
            text-align: center;
        }
        #settingbutton {
            position: relative;
            text-align: center;
        }
        #setting {
            background-color: Transparent;
            border:none;
        }
        .valuesButtons {
        margin-top: 0%;
        font-size: 15px;
        }
    </style>
    <link rel="stylesheet" href="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.css">
    <script src="https://code.jquery.com/jquery-1.11.3.min.js"></script>
    <script src="https://code.jquery.com/mobile/1.4.5/jquery.mobile-1.4.5.min.js"></script>
</head>
<body onload="update()">
    <div data-role="page">
        <div data-role="header">
            <h1>Pumpe Winkel</h1>
        </div>
        <div data-role="main" class="ui-content">
            <div data-role="rangeslider">
                <label for="price-min">Winkel:</label>
                <input type="range" onchange="setMin()" id="minSlider" name="angle-min" id="angle-min" value="%MINVALUEANGLE%" min="0" max="180">
                <label for="angle-max">Winkel:</label>
                <input type="range" onchange="setMax()" id="maxSlider" name="angle-max" id="angle-max" value="%MAXVALUEANGLE%"  min="0" max="180">
            </div>
        </div>
    </div>
    <div id="onoffdiv">
    <label class="switch">
        <input id="onoff" type="checkbox">
        <span class="slider round"></span>
    </label>
    </div>
    <div id=speedDIV>
        <p>Geschwindigkeit:</p>
    </div>
    <div class=button-group>
    <button class="button">1<h2 id="speedbuttonone" class="valuesButtons">%One%</h2></button>
    <button class="button">2<h2 id="speedbuttontwo" class="valuesButtons">%Two%</h2></button>
    <button class="button">3<h2  id="speedbuttonthree" class="valuesButtons">%Three%</h2></button>
    <button class="button">4<h2 id="speedbuttonfour" class="valuesButtons">%Four%</h2></button>
    <button class="button">5<h2 id="speedbuttonfive" class="valuesButtons">%Five%</h2></button>
    </div>
    <div id="settingbutton">
    <button id="setting" onclick="alert()"><img src="https://raw.githubusercontent.com/eleminer/AquariumStroemungspumpeWinkel/master/settingPicture.png"height="20%" width="20%"></button> 
    </div>
    <script>
    function setMin()
    {
    var value = document.getElementById("minSlider").value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/sliderMIN?value="+value, true);
    xhr.send();
    }
    function setMax()
    {
    var value = document.getElementById("maxSlider").value;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/sliderMAX?value="+value, true);
    xhr.send();
    }
    function update()
    {
    document.getElementById("speedbuttonone").innerHTML = %SPEEDFIRSTBUTTON%+"ms";
    document.getElementById("speedbuttontwo").innerHTML = %SPEEDSECONDBUTTON%+"ms";
    document.getElementById("speedbuttonthree").innerHTML = %SPEEDTHIRDBUTTON%+"ms";
    document.getElementById("speedbuttonfour").innerHTML = %SPEEDFOURBUTTON%+"ms";
    document.getElementById("speedbuttonfive").innerHTML = %SPEEDFIVEBUTTON%+"ms";
    if (%STATUS%==1)
    {
    document.getElementById("onoff").checked = true;
    }
    else
    {
         document.getElementById("onoff").checked = false;
    }
    }
    function alert()
    {
        var buttonnumber = prompt("Einstellungen Button (Nummer???)", "1");
        switch (buttonnumber)
        {
            case "1":
            var temp=prompt("Millisekunden von 1:", "50");
            break;
            case "2":
            var temp=prompt("Millisekunden von 2:", "50");
            break;
            case "3":
            var temp=prompt("Millisekunden von 3:", "50");
            break;
            case "4":
            var temp=prompt("Millisekunden von 4:", "50");
            break;
            case "5":
            var temp=prompt("Millisekunden von 5:", "50");
            break;
            default: 
            break;
        }
    }

    </script>
</body>
</html>
)rawliteral";
String processor(const String &var)
{
    if (var == "STATUS")
  {
    return status;
  }
    if (var == "MINVALUEANGLE")
  {
    return minValueAngle;
  }
    if (var == "MAXVALUEANGLE")
  {
    return maxValueAngle;
  }
  if (var == "SPEEDFIRSTBUTTON")
  {
    return speedfirstButton;
  }
  if (var == "SPEEDSECONDBUTTON")
  {
    return speedsecondButton;
  }
  if (var == "SPEEDTHIRDBUTTON")
  {
    return speedthirdButton;
  }
  if (var == "SPEEDFOURBUTTON")
  {
    return speedfourButton;
  }
  if (var == "SPEEDFIVEBUTTON")
  {
    return speedfiveButton;
  }
  
  return String();
}

void setup()

{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });


  server.on("/sliderMIN", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      minValueAngle = inputMessage;
      Serial.println("MAX:"+maxValueAngle+"MIN:"+minValueAngle);
    }
    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  server.on("/sliderMAX", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      maxValueAngle = inputMessage;
      Serial.println("MAX:"+maxValueAngle+"MIN:"+minValueAngle);
    }
    else {
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
