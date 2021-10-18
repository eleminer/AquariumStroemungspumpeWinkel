# Aquarium Stroemungspumpe Winkelsteuerung
Dieses Repository beinhaltet Software zur Steuerung einer Strömungspumpe.

**Schnellstart mit vorkompilierten Files: siehe README (ziemlich am Ende)**

## 1. Installation von der Arduino IDE und Einbindung von ESP8266 und ESP32.
Arduino Installation: [Arduino Download](https://www.arduino.cc/en/software)

Nach der Installation beide Bord URLS unter Voreinstellung eintragen.
Bord URL's:

    https://dl.espressif.com/dl/package_esp32_index.json, 
  	http://arduino.esp8266.com/stable/package_esp8266com_index.json

Im Anschluss, unter Bordverwalter, beide Bords installieren.

## 2. Installation von Libraries

    https://github.com/me-no-dev/ESPAsyncWebServer
    https://github.com/me-no-dev/AsyncTCP
    https://github.com/me-no-dev/ESPAsyncTCP
    https://github.com/zhouhan0126/WIFIMANAGER-ESP32
    https://github.com/zhouhan0126/DNSServer---esp32
    https://github.com/zhouhan0126/WebServer-esp32
    
## 2.2. Info zum Hotspot Mode
[Infoseite](https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/)
Wichtig: modifizierte Library von Wifi Manager, sodass diese mit Async funktioniert.

    https://github.com/btomer/WiFiManager
    
    
## 3. Programm hochladen auf einen ESP8266/ESP32

Beim Hochladen des Programmes ist zu beachten, dass der EEPROM **zuerst** seperat geflasht wird!

1. Dafür wird das Programm im Ordner EEPROM_flasher auf den ESP geladen.

2. Danach kann das Hautprogramm auf die gleiche Art und Weise hochgeladen werden. (sketch)

## 3.2. Info zum Hauptprogramm

Im Hauptprogramm kann der GPIO Pin für den Servo hinterlegt werden und ob es sich um einen 180° Servo handelt oder einen 360° Servo.

GPIO Pin:
    
    int servopin = 15;
       
 Faktor 180/360°
 
    int factorServo = 1; //2, wenn 360°Servo.


## 4. WLAN einstellen

Wenn kein bekanntes WLAN beim Start verfügbar ist, baut der ESP ein eigenes WLAN mit der SSID "**Pumpe**" auf. Unter der IP: **192.168.4.1**, kann nun das WLAN hinterlegt werden. Danach startet der ESP neu und versucht sich zu verbinden. Wenn die funktioniert hat, verbindet sich der ESP mit dem Netzwerk und baut kein eigenes Netzwerk mehr auf.

**Das WLAN Passwort sollte nicht länger als 40Zeichen sein!**
**Die SSID sollte nicht länger sein als 30 Zeichen!**

So sieht die Webseite zum Einstellen des WLAN aus:

<img src="readmePictures/ScreenshotAndroidHotspot.jpeg" width="200">


Damit nun der Webservice verfügbar ist, **muss** der ESP jedoch **nach der erfolgreichen Verbindung nochmal neu gestartet werden**.
Danach sollte der Webservice unter der IP vom ESP und dem Port 80 im Netzwerk erreichbar sein.

## 5. Grafisches Interface

So sieht die "Steuerungswebseite" aus:

Android Smartphone         | Windows PC 
:-------------------------:|:-------------------------:
<img src="readmePictures/ScreenshotAndroid.jpeg" width="300">  |  <img src="readmePictures/ScreenshotWindows.png" width="600">


Die Zeit pro Winkel pro Knopf, in Millisekunden, können über das Einstellungsmenü (mittlerer Knopf (Zahnrad)) eingestellt werden.

Werte zwischen **1ms und 9999999ms** werden vom Programm angenommen und abgespeichert.
Diese Werte bleiben auch nach einem Stromausfall bzw. Neustart erhalten!

## 6. Verkabelung

Servo Datenleitung an GPIO 15. Am Wemos D1 wäre dies D8 zum Beispiel.

Gemeinsames Ground beachten!

## 7. Besonderheit

Da die Webseite auf einem Mikrocontroller gehostet wird, werden Zusatz Pakete im Form von Javascript oder Images von externer Quelle geladen.

Das Wlan in welchem sich der ESP und das Gerät zur Steuerung (Smartphone oder Laptop etc.) befindet muss daher zwingend mit dem Internet verbunden sein.

Wenn dies nicht gewünscht ist, können die Zusatzpakete auch auf einem anderem Gerät im Netzwerk gehostet werden. (NAS System, Raspberry etc.)


Wenn der Webspeicher im Gerät nicht gelöscht wird, hinterlegt der Browser gegebenfalls diese Daten um diese nicht immer wieder neu abfragen zu müssen.

Darauf sollte man sich jedoch nicht verlassen!

### Neu:

Im Hintergrund wird ein NTP Server angepingt. Falls der Zeitserver nicht erreichbar ist bzw. die Zeit nicht korrekt abgefragt werden kann, leuchtet die interne (blaue) LED vom ESP auf.

## 8. Schnellstart ESP8266

1. Dieses Repository klonen (kopieren) und gegebenfalls extrahieren.
2. ESPEasyFlasher-master öffnen.
3. FlashESP8266.exe öffnen.
4. EEPROM flashen. (NewEEPROM8266_flasher.ino.generic.bin)
5. sketch flashen. (NewSketch8266.ino.generic.bin)
6. Mit WLAN Pumpe verbinden.
7. 192.168.4.1 URL öffnen.
8. WIFI einstellen.
9. Kurz warten bis sich der ESP mit dem soeben eingegebenem WLAN verbindet.
10. ESP neu starten
11. Mit der ESP URL verbinden und den Servo einstellen.
12. Kaffe holen, du hast es geschafft. :)
