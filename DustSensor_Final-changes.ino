/*
  Dust sensor
  28/5 - 2016

  Pin setup:
  Dust sensor : Arduino
  2             D8
  3             +5V
  5             Ground

  REMEMBER:
  - Change node decleration to be unique for every single sensor
  - Change wifi_password and ssid to own pass and ssid
*/

//Include Wifi library, containing all wifi related functions
#include <ESP8266WiFi.h>

//Global variables
int pin = D8; //Input pin from VOUT1 from dust sensor, abbreviation for pin D8
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 10000;
unsigned long lowpulseoccupancy = 0;  //Initial value = 0
float ratio = 0; //Initial value = 0
float concentration = 0; //Initial value = 0


// Constants
const char* ssid = "Hytten";
const char* wifi_password = "12345678";

//Client object
WiFiClient client;
//Complex variable

//Unique node, kind of unique sensor number
int node = 3;


//Function prototypes, declaration of functions used in following code
void setup_wifi();
void wifiSendData(double concentration, double ratio, double mass);



void setup() {
  ESP.wdtDisable(); //Disable watch dog timer, so the board does not reset when setup takes too long
  Serial.begin(115200);  //Start serial communication
  Serial.println("Starting program...");

  setup_wifi(); //Set up wifi with given passwrod and ssid
  Serial.println("Start measuring");
  

  pinMode(D8, INPUT); // Setup pin as input for reading low time
  starttime = millis();//get the current time;
}

void loop() {
  ESP.wdtDisable();//Disable WDT, otherwise causes errors when using millis()
  duration = pulseIn(pin, LOW);  //Measures the time of a single low pulse
  lowpulseoccupancy = lowpulseoccupancy + duration; //Measures total time of low pulse

  if (!((millis() - starttime) < sampletime_ms))  //! = not true
  {
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0); //Converts to ratio of low time
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // Calculates
    concentration = concentration * (3531.46); //Converts to Cubic meter


    Serial.println("\n******** NEW MEASUREMENT ****************");
    
    Serial.print("*Concentration = ");
    Serial.print(concentration);
    Serial.print(" pcs/m3");
    Serial.println("\t*"); //Tab 

    double particleMass = ratio * 0.11667; //Calculated as linear interpolation of spec data, as 1.14/0.12. (table 8.2)
    Serial.print("*Particle mass = ");
    Serial.print(particleMass);
    Serial.print(" mg/m3");
    Serial.println("\t\t\t*");
    
    Serial.print("*Ratio = ");
    Serial.print(ratio);
    Serial.println("\t\t\t\t*");
    
    Serial.println("*****************************************");
    lowpulseoccupancy = 0;  //Reset value
    starttime = millis();  //Update starttime value

    //Checks if device is connected to Wifi, if not then try to reconnect
    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, wifi_password);
    }

    //Send data using send data function
    wifiSendData(concentration, ratio, particleMass);
    
  }
}



void setup_wifi() {
  Serial.println("Starting scanning for WiFi APs");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  byte mac[6];              
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);
  
  while (n == 0)
    {Serial.println("no networks found");
    n = WiFi.scanNetworks();
    delay(500);
    }
  
  
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  
 Serial.println("");

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifi_password);
  delay(5000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  for (int i=0;i<50;i++)  //Blink 50 times with builtin LED
  {
    digitalWrite(BUILTIN_LED, i%2);
    delay(50);
  }


}


void wifiSendData(double concentration, double ratio, double mass){
  
  const int httpPort = 80;
  if (!client.connect("emoncms.org", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected to server");

  
  // We now create a URL for the first request
  //URL string to be requested
  String url = " /input/post.json?node=" + String(node) + "&json={concentration:" + String(concentration) +  ",ratio:" + String(ratio) +  "}&apikey=c3cad7f4bdad9950432c37fa573c9b65"; //Remember to correct API key
  String host = "Host: emoncms.org\n";
  
  // This will send the request to the server
  String requestString = String("GET ") + url  + " HTTP/1.1\n"+ host + "Connection: close\r\n\r\n";
  Serial.print("Request String: \n\n" + requestString);
  client.print(requestString ); //Send request to server

  Serial.println("\n\n********* Response ********* \n");

  //Wait for server to respond and print '-'
  while(!client.available()) {
    Serial.print("-");
    delay(200); 
  }

  //Write out what the server responds with
  while(client.available()){
    Serial.write(client.read());
  }



  

  if (!client.connect("emoncms.org", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected to server");

  
  // We now create a URL for the second request
  url = " /input/post.json?node=" +  String(node) + "&json={mass:" + String(mass) +  "}&apikey=c3cad7f4bdad9950432c37fa573c9b65"; //Remember to correct API key
  
  // This will send the request to the server
  requestString = String("GET ") + url  + " HTTP/1.1\n"+ host + "Connection: close\r\n\r\n";
  Serial.print("Request String: \n\n" + requestString);
  client.print(requestString );

  Serial.println("\n\n********* Response ********* \n");

  
  while(!client.available()) {
    Serial.print("-");
    delay(200); 
  }


  //Write out what the server responds with
  while(client.available()){
    Serial.write(client.read());
  }



  
}




