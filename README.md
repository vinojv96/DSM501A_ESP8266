# DSM501A-ESP8266

Dust sensor connected to Wifi sending data to www.emoncms.org
Send a mail to Kasperspersonalmail@gmail.com for questions



Arduino IDE setup guide:

Go to : File -> Preferences -> Additional Board manager. Then paste: "http://arduino.esp8266.com/stable/package_esp8266com_index.json"
Go to : Tools -> Boards. Choose Wemos D1 (retired)
Go to : Tools -> Upload speed. Choose 115200

When loading code, set Com Port to the one your USB is connected too.



  Pin setup:
  Dust sensor : Arduino
  2             D8
  3             +5V
  5             Ground
  
  Wire no. 1 and 4 can be used to change the sensitivity, however this is not required.
  
  
  Code to change:
  
  
  // Constants
const char* ssid = "you ssid";
const char* wifi_password = "your password";

int node = 1 // This is the unique sensor number, changing it will make a "new" sensor.

 String url = " /input/post.json?node=" + String(node) + "&json={concentration:" + String(concentration) +  ",ratio:" + String(ratio) +  "}&apikey=c3cad7f4bdad9950432c37fa573c9b65"; //Remember to correct API key with your own
 
 
   url = " /input/post.json?node=" +  String(node) + "&json={mass:" + String(mass) +  "}&apikey=c3cad7f4bdad9950432c37fa573c9b65"; //Remember to correct API key with your own
