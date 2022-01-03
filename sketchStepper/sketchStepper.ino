#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#define WIFI_MANAGER_USE_ASYNC_WEB_SERVER
#include <WiFiManager.h>
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <AccelStepper.h>
#define dirPin 14    // D5
#define stepPin 12   // D6
#define enablepin 13 // D7
#define motorInterfaceType 1
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP,"fritz.box", 36000, 60000);
NTPClient timeClient(ntpUDP, "pool.ntp.org", 36000, 60000);

// nur diese Werte manuell ändern!
const char *ssid = "Develop";
const char *password = "384783478";
int factorServo = 1; // 2, wenn 360°Servo. 1, wenn 180°Servo
int servopin = 15;
// nur diese Werte manuell ändern!

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

const int led = 2;
Servo myservo;
int addr = 0;
char eeprom[1000] = "";
char *pointer = eeprom;
char *pointerLoop = eeprom;
int positionServo = 90;
int direction = 1;
const char *PARAM_INPUT = "value";
const char *PARAM_INPUT_SECOND = "buttonN";
int recievedValue = 0;
int recievedButtonNumber = 0;
String speedfirstButton = "9999999";
String speedsecondButton = "9999999";
String speedthirdButton = "9999999";
String speedfourButton = "9999999";
String speedfiveButton = "9999999";
String maxValueAngle = "135";
String minValueAngle = "100";
String status = "0";
String selection = "1";
unsigned long currentTime = 0;
unsigned long timePoint = 0;
unsigned long timePointTwo = 0;
String minValueAngleREAD = "200";
String maxValueAngleREAD = "200";
String selectionREAD = "0";

String BrakeBeginnREAD = "10:00";
String BrakeEndREAD = "20:00";
String summertimeREAD = "false";
String BrakePositionREAD = "110";
String automaticREAD = "false";

String summertime = "false";
String automatic = "false";
String BrakePosition = "170";
char BrakeBeginn[6] = "18:45";
char BrakeEnd[6] = "00:05";
char actualTimeString[9] = "44:44:44";
int paused = 0;
int errorTime = 0;
bool ntpstatus = 1;
bool timeforSet = true;
unsigned long timePointNTP = 0;
bool timeisSet = false;
String servowaiting = "false";
String servowaitingREAD = "false";
bool servoattached=0;
bool overrideTimePaused=0;

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
        #tentacles {
          size=5
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
        .inputfieldtext-group {
            color: #2196F3;
            position: relative;
            margin-top: 10px;
            margin-bottom: 0px;
            text-align: center;
        }
        #summertimeDIV {
            position: relative;
            margin-top: 15px;
            text-align: center;

        }
        #speedDIV {
            position: relative;
            margin-top: 100px;
            text-align: center;
        }
        #actualTime {
            color: #2196F3;
        }
        #timerDIV {
            position: relative;
            margin-top: 20px;
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
        #placeholderBottom {
          padding-bottom: 20px;
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
        <input onclick="powerbutton()" id="onoff" type="checkbox">
        <span class="slider round"></span>
    </label>
    </div>
    <div id=speedDIV>
        <p>Geschwindigkeit:</p>
    </div>
    <div class=button-group>
    <button onclick="buttonchange(1)" class="button">1<h2 id="speedbuttonone" class="valuesButtons">%One%</h2></button>
    <button onclick="buttonchange(2)" class="button">2<h2 id="speedbuttontwo" class="valuesButtons">%Two%</h2></button>
    <button onclick="buttonchange(3)" class="button">3<h2  id="speedbuttonthree" class="valuesButtons">%Three%</h2></button>
    <button onclick="buttonchange(4)" class="button">4<h2 id="speedbuttonfour" class="valuesButtons">%Four%</h2></button>
    <button onclick="buttonchange(5)" class="button">5<h2 id="speedbuttonfive" class="valuesButtons">%Five%</h2></button>
    </div>
    <div id="settingbutton">
    <button id="setting" onclick="alert()"><img src="https://raw.githubusercontent.com/eleminer/AquariumStroemungspumpeWinkel/master/settingPicture.png"height="20%" width="20%"></button> 
    </div>
    <div id=timerDIV>
        <p>Uhrzeit und Zeitschaltung:</p>
    </div>
    <div class=button-group>
    <time id=actualTime>00:00:00</time>
    </div>
    <div id=summertimeDIV>
  <label>
  <input type="checkbox" id="summer" onclick="checkboxValueChanged()" name="summer">
  Sommerzeit?</label>
  </div>
  <div class=inputfieldtext-group>
  <p>Pause von:</p>
  <input id=userInputBeginn type="time" value=%BRAKEBEGINN% name="TimeStop">
  </div>
  <div class=inputfieldtext-group>
  <p>Pause bis:</p>
  <input id=userInputEnd type="time" value=%BRAKEEND% name="TimeResume">
  </div>
  <div class=inputfieldtext-group id=placeholderBottom>
  <p>An der Position:</p>
  <input type="number" id="tentacles" name="tentacles"
       min="0" max="180" step="1" value=%BRAKEPOSITION% step=1>
  </div>
  <div class=inputfieldtext-group id=placeholderBottom>
  <label>
  <input type="checkbox" id="automaticSwitch" onclick="checkboxAutomatic()" name="onoff">
  Zeitschaltung?</label>
  </div>
  <div class=inputfieldtext-group id=placeholderBottom>
  <label>
  <input type="checkbox" id="servooffduringwaitingSwitch" onclick="sendStatusServoWaiting()" name="onoffServo">
  Servo Abschaltung?</label>
  </div>
    <script>

    function buttonchange(buttonnumber)
    {
    var i=1;
    while (i<=5)
    {
      if(i==buttonnumber)
      {
        switch (i)
        {
          case 1:
          (document.getElementById("speedbuttonone")).style.backgroundColor = "red";
          break;
          case 2:
          (document.getElementById("speedbuttontwo")).style.backgroundColor = "red";
          break;
          case 3:
          (document.getElementById("speedbuttonthree")).style.backgroundColor = "red";
          break;
          case 4:
          (document.getElementById("speedbuttonfour")).style.backgroundColor = "red";
          break;
          case 5:
          (document.getElementById("speedbuttonfive")).style.backgroundColor = "red";
          break;
        }

      }
      else
      {
        switch (i)
        {
          case 1:
          (document.getElementById("speedbuttonone")).style.backgroundColor = "#2196F3";
          break;
          case 2:
          (document.getElementById("speedbuttontwo")).style.backgroundColor = "#2196F3";
          break;
          case 3:
          (document.getElementById("speedbuttonthree")).style.backgroundColor = "#2196F3";
          break;
          case 4:
          (document.getElementById("speedbuttonfour")).style.backgroundColor = "#2196F3";
          break;
          case 5:
          (document.getElementById("speedbuttonfive")).style.backgroundColor = "#2196F3";
          break;
        }
      }
      i++;
    }
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/button?value="+buttonnumber, true);
    xhr.send();
    }

    function powerbutton()
    {
    if (onoff.checked != true)
    {
    var value = 0;
    }
    else
    {
    var value = 1;
    }
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/power?value="+value, true);
    xhr.send();
    }

    function setMin()
    {
    var value = document.getElementById("minSlider").value;
    var xhr = new XMLHttpRequest();
    if(Math.sign(value)==1 || Math.sign(value)==+0)
    {
      if(Math.abs(Math.round(value))<=180)
      {
        xhr.open("GET", "/sliderMIN?value="+Math.abs(Math.round(value)), true);
        xhr.send();
      }
    }
    else
    {
      console.log("invalid input");
    }
    }

    function setMax()
    {
    var value = document.getElementById("maxSlider").value;
    var xhr = new XMLHttpRequest();
    if(Math.sign(value)==1 || Math.sign(value)==+0)
    {
      if(Math.abs(Math.round(value))<=180)
      {
        xhr.open("GET", "/sliderMAX?value="+Math.abs(Math.round(value)), true);
        xhr.send();
      }
    }
    else
    {
      console.log("invalid input");
    }
    }

    function update()
    {
    document.getElementById("speedbuttonone").innerHTML = %SPEEDFIRSTBUTTON%+"ms";
    document.getElementById("speedbuttontwo").innerHTML = %SPEEDSECONDBUTTON%+"ms";
    document.getElementById("speedbuttonthree").innerHTML = %SPEEDTHIRDBUTTON%+"ms";
    document.getElementById("speedbuttonfour").innerHTML = %SPEEDFOURBUTTON%+"ms";
    document.getElementById("speedbuttonfive").innerHTML = %SPEEDFIVEBUTTON%+"ms";
    document.getElementById("summer").checked = %SUMMERTIME%;
    document.getElementById("automaticSwitch").checked = %AUTOMATIC%;
    document.getElementById("servooffduringwaitingSwitch").checked = %SERVOWAITING%;
    if (%STATUS%==1)
    {
    document.getElementById("onoff").checked = true;
    }
    else
    {
    document.getElementById("onoff").checked = false;
    }
    switch (%SELECTION%)
    {
      case 1:
      (document.getElementById("speedbuttonone")).style.backgroundColor = "red";
      break;
      case 2:
      (document.getElementById("speedbuttontwo")).style.backgroundColor = "red";
      break;
      case 3:
      (document.getElementById("speedbuttonthree")).style.backgroundColor = "red";
      break;
      case 4:
      (document.getElementById("speedbuttonfour")).style.backgroundColor = "red";
      break;
      case 5:
      (document.getElementById("speedbuttonfive")).style.backgroundColor = "red";
      break;
      default:
      console.log("error");
      break;
    }

    }

    function alert()
    {
        var buttonnumber = prompt("Einstellungen Button (Nummer???)", "1");
        switch (buttonnumber)
        {
            case "1":
            var temp=prompt("Millisekunden von 1:", "50");
            var letters = /^[0-9]+$/;
            if(temp.match(letters))
            {
            document.getElementById("speedbuttonone").innerHTML = temp+"ms";
            var invalue=temp;
            }else
            {
                alert('invalid input');
            }
            break;
            case "2":
            var temp=prompt("Millisekunden von 2:", "50");
            var letters = /^[0-9]+$/;
            if(temp.match(letters))
            {
            document.getElementById("speedbuttontwo").innerHTML = temp+"ms";
            var invalue=temp;
            }else
            {
                alert('invalid input');
            }
            break;
            case "3":
            var temp=prompt("Millisekunden von 3:", "50");
            var letters = /^[0-9]+$/;
            if(temp.match(letters))
            {
            document.getElementById("speedbuttonthree").innerHTML = temp+"ms";
            var invalue=temp;
            }else
            {
                alert('invalid input');
            }
            break;
            case "4":
            var temp=prompt("Millisekunden von 4:", "50");
            var letters = /^[0-9]+$/;
            if(temp.match(letters))
            {
            document.getElementById("speedbuttonfour").innerHTML = temp+"ms";
            var invalue=temp;
            }else
            {
                alert('invalid input');
            }
            break;
            case "5":
            var temp=prompt("Millisekunden von 5:", "50");
            var letters = /^[0-9]+$/;
            if(temp.match(letters))
            {
            document.getElementById("speedbuttonfive").innerHTML = temp+"ms";
            var invalue=temp;
            }else
            {
                alert('invalid input');
            }
            break;
            default: 
            break;
        }
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/buttonValues?value="+invalue+"&buttonN="+buttonnumber, true);
    xhr.send();
    }

    function checkboxValueChanged()
    {
      var xhr = new XMLHttpRequest();
      if(document.getElementById("summer").checked == true)
      {
        xhr.open("GET", "/summertime?value=true", true);
      }
      else
      {
        xhr.open("GET", "/summertime?value=false", true);
      }
      xhr.send();
    }

    function checkboxAutomatic()
    {
      var xhr = new XMLHttpRequest();
      if(document.getElementById("automaticSwitch").checked == true)
      {
        xhr.open("GET", "/automatic?value=true", true);
      }
      else
      {
        xhr.open("GET", "/automatic?value=false", true);
      }
      xhr.send();
    }

    function sendStatusServoWaiting()
    {
      var xhr = new XMLHttpRequest();
      if(document.getElementById("servooffduringwaitingSwitch").checked == true)
      {
        xhr.open("GET", "/servowaiting?value=true", true);
      }
      else
      {
        xhr.open("GET", "/servowaiting?value=false", true);
      }
      xhr.send();
    }

    function reqListener()
    {
      document.getElementById("actualTime").innerHTML = this.responseText;
    }

    function updateClock()
    {
      var xhr = new XMLHttpRequest();
      xhr.addEventListener("load",reqListener);
      xhr.open("GET", "/timeRequestPackage", true);
      xhr.send();
    }

    document.addEventListener('visibilitychange', function (event) {
    if (document.hidden) {
        clearInterval(handle);
    } else {
        document.location.reload(true)
    }
    });

    var handle=setInterval(updateClock, 1000);


    var startTime = document.getElementById("userInputBeginn");
    startTime.addEventListener("input", function() {
    var xhr = new XMLHttpRequest();
    if((!userInputBeginn.value==""))
    {
    xhr.open("GET", "/startTime?value="+String(userInputBeginn.value), true);
    xhr.send();
    }
    }, true);

    var endTime = document.getElementById("userInputEnd");
    endTime.addEventListener("input", function() {
    var xhr = new XMLHttpRequest();
    if((!userInputEnd.value==""))
    {
    xhr.open("GET", "/endTime?value="+String(userInputEnd.value), true);
    xhr.send();
    }
    }, true);

    var postionBrake = document.getElementById("tentacles");
    postionBrake.addEventListener("input", function() {
    var xhr = new XMLHttpRequest();
    if((tentacles.value)>=0 && (tentacles.value)<=180)
    {
      if(Math.abs(Math.round(tentacles.value))<=180)
      {
        xhr.open("GET", "/positionBrake?value="+String(Math.abs(Math.round(tentacles.value))), true);
        xhr.send(); 
      }
    }
    else
    {
      console.log("invalid input");
    }
    }, true);


    
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
  if (var == "SELECTION")
  {
    return selection;
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
  if (var == "SUMMERTIME")
  {
    return summertime;
  }
  if (var == "BRAKEPOSITION")
  {
    return BrakePosition;
  }
  if (var == "BRAKEBEGINN")
  {
    return BrakeBeginn;
  }
  if (var == "BRAKEEND")
  {
    return BrakeEnd;
  }
  if (var == "AUTOMATIC")
  {
    return automatic;
  }
  if (var == "SERVOWAITING")
  {
    return servowaiting;
  }

  return String();
}

void setup()
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  myservo.attach(servopin);
  servoattached=1;
  myservo.write(positionServo * factorServo);
  EEPROM.begin(512);
  Serial.begin(115200);
  int i = 0;
  while ((char)EEPROM.read(i) != 'E')
  {
    *pointer = (char)EEPROM.read(i);
    pointer++;
    i++;
  }
  speedfirstButton = getValue(eeprom, ',', 1);
  speedsecondButton = getValue(eeprom, ',', 2);
  speedthirdButton = getValue(eeprom, ',', 3);
  speedfourButton = getValue(eeprom, ',', 4);
  speedfiveButton = getValue(eeprom, ',', 5);
  status = getValue(eeprom, ',', 6);
  minValueAngle = getValue(eeprom, ',', 7);
  maxValueAngle = getValue(eeprom, ',', 8);
  selection = getValue(eeprom, ',', 9);
  summertime = getValue(eeprom, ',', 10);
  (getValue(eeprom, ',', 11)).toCharArray(BrakeBeginn, 6);
  (getValue(eeprom, ',', 12)).toCharArray(BrakeEnd, 6);
  BrakePosition = getValue(eeprom, ',', 13);
  automatic = getValue(eeprom, ',', 14);
  servowaiting = getValue(eeprom, ',', 15);
  WiFiManager wifiManager;
  wifiManager.autoConnect("Pumpe");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

  server.on("/timeRequestPackage", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    request->send(200, "text/plain", actualTimeString); });

  server.on("/sliderMIN", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      minValueAngle = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/summertime", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      summertime = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/servowaiting", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      servowaiting = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/automatic", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      automatic = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/startTime", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      inputMessage.toCharArray(BrakeBeginn, 6);
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/endTime", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      inputMessage.toCharArray(BrakeEnd, 6);
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/positionBrake", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      BrakePosition = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/sliderMAX", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      maxValueAngle = inputMessage;
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      status = inputMessage;
      String defaultSettings = String("," + String(speedfirstButton) + "," + String(speedsecondButton) + "," + String(speedthirdButton) + "," + String(speedfourButton) + "," + String(speedfiveButton) + "," + String(status) + "," + String(minValueAngle) + "," + String(maxValueAngle) + "," + String(selection) + "," + String(summertime) + "," + String(BrakeBeginn) + "," + String(BrakeEnd) + "," + String(BrakePosition) + "," + String(automatic) + "," + String(servowaiting) + "," + "H" + "E");
      for (int i = 0; i < defaultSettings.length(); i++)
      {
        EEPROM.write(0x0F + i, defaultSettings[i]);
      }
      EEPROM.commit();
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/button", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      selection = inputMessage;
    }
    else
    {
      inputMessage = "No message sent1";
    }
    request->send(200, "text/plain", "OK"); });

  server.on("/buttonValues", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    String inputMessageSecond;
    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      recievedValue = (inputMessage).toInt();
      inputMessageSecond = request->getParam(PARAM_INPUT_SECOND)->value();
      recievedButtonNumber = (inputMessageSecond).toInt();
    }
    else
    {
      inputMessage = "No message sent";
    }
    request->send(200, "text/plain", "OK");
    if (recievedButtonNumber > 0 && recievedButtonNumber <= 5)
    {
      if (recievedValue > 0)
      {
        switch (recievedButtonNumber)
        {
        case 1:
          speedfirstButton = String(recievedValue);
          break;
        case 2:
          speedsecondButton = String(recievedValue);
          break;
        case 3:
          speedthirdButton = String(recievedValue);
          break;
        case 4:
          speedfourButton = String(recievedValue);
          break;
        case 5:
          speedfiveButton = String(recievedValue);
          break;
        }
          String defaultSettings = String("," + String(speedfirstButton) + "," + String(speedsecondButton) + "," + String(speedthirdButton) + "," + String(speedfourButton) + "," + String(speedfiveButton) + "," + String(status) + "," + String(minValueAngle) + "," + String(maxValueAngle) + "," + String(selection) + "," + String(summertime) + "," + String(BrakeBeginn) + "," + String(BrakeEnd) + "," + String(BrakePosition) + "," + String(automatic) + "," + String(servowaiting) + "," + "H" + "E");
          for (int i = 0; i < defaultSettings.length(); i++)
        {
          EEPROM.write(0x0F + i, defaultSettings[i]);
        }
        EEPROM.commit();
      }
    } });
  server.begin();
  timeClient.begin();
}

void loop()
{
  if (millis() <= 5000 && !timeisSet)
  {
    timeforSet = true;
  }
  if (millis() >= 4294667295)
  {
    timeisSet = false;
  }

  if ((unsigned long)millis() - timePointNTP > 5000)
  {
    timePointNTP = millis();
    if (WiFi.status() == WL_CONNECTED && timeforSet)
    {
      if (ntpstatus)
      {
        if (timeClient.forceUpdate())
        {
          digitalWrite(led, 1);
          errorTime = 0;
          timeforSet = false;
          timeisSet = true;
        }
        else
        {
          digitalWrite(led, 0);
          errorTime++;
          timeforSet = true;
          timeisSet = false;
        }
        if (errorTime >= 25)
        {
          Serial.println("error with ntp, shutdown the connection");
          digitalWrite(led, 0);
          errorTime = 0;
          ntpstatus = 0;
          timeforSet = true;
          timeisSet = false;
        }
      }
    }
  }

  if (summertime == "true")
  {
    timeClient.setTimeOffset(7200);
  }
  else
  {
    timeClient.setTimeOffset(3600);
  }
  (timeClient.getFormattedTime()).toCharArray(actualTimeString, 9);

  int firstHourActual = (String(actualTimeString[0])).toInt();
  int secondHourActual = (String(actualTimeString[1])).toInt();
  int firstMinActual = (String(actualTimeString[3])).toInt();
  int secondMinActual = (String(actualTimeString[4])).toInt();

  int firstHourBeginn = (String(BrakeBeginn[0])).toInt();
  int secondHourBeginn = (String(BrakeBeginn[1])).toInt();
  int firstMinBeginn = (String(BrakeBeginn[3])).toInt();
  int secondMinBeginn = (String(BrakeBeginn[4])).toInt();

  int firstHourEnd = (String(BrakeEnd[0])).toInt();
  int secondHourEnd = (String(BrakeEnd[1])).toInt();
  int firstMinEnd = (String(BrakeEnd[3])).toInt();
  int secondMinEnd = (String(BrakeEnd[4])).toInt();

  int numberActualMinutes = firstHourActual * 600 + secondHourActual * 60 + firstMinActual * 10 + secondMinActual;
  int numberBeginnMinutes = firstHourBeginn * 600 + secondHourBeginn * 60 + firstMinBeginn * 10 + secondMinBeginn;
  int numberEndMinutes = firstHourEnd * 600 + secondHourEnd * 60 + firstMinEnd * 10 + secondMinEnd;

  currentTime = millis();
  int speed = 100;
  int intervalEEPROMcheck = 5000;
  int temp = selection.toInt();
  switch (temp)
  {
  case 1:
    speed = speedfirstButton.toInt();
    break;
  case 2:
    speed = speedsecondButton.toInt();
    break;
  case 3:
    speed = speedthirdButton.toInt();
    break;
  case 4:
    speed = speedfourButton.toInt();
    break;
  case 5:
    speed = speedfiveButton.toInt();
    break;
  default:
    break;
  }
  if ((unsigned long)currentTime - timePoint > speed)
  {
    if (numberBeginnMinutes >= numberEndMinutes)
    {
      // special case
      if ((numberActualMinutes >= numberBeginnMinutes || numberActualMinutes < numberEndMinutes) && automatic == "true")
      {
        paused = 1;
      }
      else
      {
        paused = 0;
      }
    }
    else
    {
      if ((numberActualMinutes >= numberBeginnMinutes && numberActualMinutes < numberEndMinutes) && automatic == "true")
      {
        paused = 1;
      }
      else
      {
        paused = 0;
      }
    }

    if (paused == 1 && status == "1")
    {
      if (positionServo > BrakePosition.toInt())
      {
        positionServo--;
        myservo.write(positionServo);
        if ((positionServo) >= (maxValueAngle.toInt()))
        {
          direction = 0;
        }
        if ((positionServo) <= (minValueAngle.toInt()))
        {
          direction = 1;
        }
      }
      if (positionServo < BrakePosition.toInt())
      {
        positionServo++;
        myservo.write(positionServo);
        if ((positionServo) >= (maxValueAngle.toInt()))
        {
          direction = 0;
        }
        if ((positionServo) <= (minValueAngle.toInt()))
        {
          direction = 1;
        }
      }
      if (positionServo == BrakePosition.toInt() && servowaiting=="true" && servoattached)
      {
        overrideTimePaused=1;
        myservo.detach();
        servoattached=0;
      }
    }
    if(paused==0 || servowaiting=="false")
    {
      overrideTimePaused=0;
    }

    if (status == "1" && factorServo == 1 && paused == 0)
    {
      if (direction == 1)
      {
        positionServo++;
      }
      else
      {
        positionServo--;
      }
      myservo.write(positionServo);
      if ((positionServo) >= (maxValueAngle.toInt()))
      {
        direction = 0;
      }
      if ((positionServo) <= (minValueAngle.toInt()))
      {
        direction = 1;
      }
    }

    if (status == "1" && factorServo == 2 && paused == 0)
    {
      if (direction == 1)
      {
        positionServo++;
      }
      else
      {
        positionServo--;
      }
      myservo.write(positionServo);
      int difference = maxValueAngle.toInt() - minValueAngle.toInt();
      int midpoint = minValueAngle.toInt() + (difference / 2);

      int maxNEW = (midpoint * 2) + (difference);
      int minNEW = (midpoint * 2) - (difference);
      if ((positionServo) >= (maxNEW))
      {
        direction = 0;
      }
      if ((positionServo) <= (minNEW))
      {
        direction = 1;
      }
    }
    timePoint = millis();
  }
  if ((unsigned long)currentTime - timePointTwo > intervalEEPROMcheck)
  {
    int i = 0;
    char *temppointer = pointerLoop;
    while ((char)EEPROM.read(i) != 'E')
    {
      *temppointer = (char)EEPROM.read(i);
      temppointer++;
      i++;
    }
    minValueAngleREAD = getValue(eeprom, ',', 7);
    maxValueAngleREAD = getValue(eeprom, ',', 8);
    selectionREAD = getValue(eeprom, ',', 9);

    summertimeREAD = getValue(eeprom, ',', 10);
    BrakeBeginnREAD = getValue(eeprom, ',', 11);
    BrakeEndREAD = getValue(eeprom, ',', 12);
    BrakePositionREAD = getValue(eeprom, ',', 13);
    automaticREAD = getValue(eeprom, ',', 14);
    servowaitingREAD = getValue(eeprom, ',', 15);
    if (minValueAngleREAD.toInt() != minValueAngle.toInt() || maxValueAngleREAD.toInt() != maxValueAngle.toInt() || selectionREAD.toInt() != selection.toInt() || String(summertimeREAD) != String(summertime) || String(BrakeBeginnREAD) != String(BrakeBeginn) || String(BrakeEndREAD) != String(BrakeEnd) || String(BrakePositionREAD) != String(BrakePosition) || String(automaticREAD) != String(automatic) || String(servowaiting) != String(servowaitingREAD))
    {
      String defaultSettings = String("," + String(speedfirstButton) + "," + String(speedsecondButton) + "," + String(speedthirdButton) + "," + String(speedfourButton) + "," + String(speedfiveButton) + "," + String(status) + "," + String(minValueAngle) + "," + String(maxValueAngle) + "," + String(selection) + "," + String(summertime) + "," + String(BrakeBeginn) + "," + String(BrakeEnd) + "," + String(BrakePosition) + "," + String(automatic) + "," + String(servowaiting) + "," + "H" + "E");
      for (int i = 0; i < defaultSettings.length(); i++)
      {
        EEPROM.write(0x0F + i, defaultSettings[i]);
      }
      EEPROM.commit();
      Serial.println("save changes to eeprom");
    }
    timePointTwo = millis();
  }
  if(status=="0" && servoattached && servowaiting=="true")
  {
    myservo.detach();
    servoattached=0;
  }
  if(status=="0" && !servoattached && servowaiting=="false")
  {
    myservo.attach(servopin);
    servoattached=1;
  }
  if(status=="1" && !servoattached && !overrideTimePaused)
  {
    myservo.attach(servopin);
    servoattached=1;
  }
}
