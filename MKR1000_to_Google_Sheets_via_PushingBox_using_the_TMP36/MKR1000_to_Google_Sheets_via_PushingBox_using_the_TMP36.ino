//-----------------------------------------------
//This sketch is combined from Adafruit DHT sensor and tdicola for dht.h library
// https://learn.adafruit.com/dht/overview
// https://gist.github.com/teos0009/acad7d1e54b97f4b2a88
//other Authors Arduino and associated Google Script:
//Aditya Riska Putra
//Ahmed Reza Rafsanzani
//Ryan Eko Saputro
//See Also:
//http://jarkomdityaz.appspot.com/
//
//ELINS UGM
//
//Modified for Hackster.io project for the MKR1000 
//by Stephen Borsay(Portland, OR, USA)
//Since Arduino can't https, we need to use Pushingbox API (uses http)to run 
//the Google Script (uses https). Alternatly use Ivan's SecureWifi encryption

// Edited by Ahmed Abd-Elrahman

#include <WiFi101.h>
////#include "DHT.h"

////#define TMP36PIN 5    // what pin we're connected to, pin1 is 5th pin from end // (modified by Ahmed Abd-Elrahman)

int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
//                        //the resolution is 10 mV / degree centigrade with a
//                        //500 mV offset to allow for negative temperatures

// Uncomment whatever DHT sensor type you're using!

////#define DHTTYPE DHT11  // DHT 11

//#define DHTTYPE DHT21  // DHT 21
//#define DHTTYPE DHT22  // DHT 22

////DHT dht(DHTPIN,DHTTYPE);

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v54DE8FBCFF6B39B"; //device ID on Pushingbox for our Scenario

const char* MY_SSID = "BHNTC8715D5D54";
const char* MY_PWD =  "349554a8";


int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(MY_SSID);
    //Connect to WPA/WPA2 network.Change this line if using open/WEP network
    status = WiFi.begin(MY_SSID, MY_PWD);

    // wait 10 seconds for connection:
    delay(10000);
  }
  
  Serial.println("Connected to wifi");
  printWifiStatus();
  
}

void loop() {

   // Wait between measurements.
  delay(10000);

  //prefer to use float, but package size or float conversion isnt working
  //will revise in future with a string fuction or float conversion function

  ////int humidityData = dht.readHumidity();
  // Read temperature as Celsius (the default)
  ////int celData = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  ////int fehrData = dht.readTemperature(true);


  int reading = analogRead(sensorPin);  
 
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 3.3;
  voltage /= 1024.0; 
 
  // print out the voltage
  //Serial.print(voltage); Serial.println(" volts");
 
  // now print out the temperature
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
  //Serial.print(temperatureC); Serial.println(" degrees C");
 
  // now convert to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  //Serial.print(temperatureF); Serial.println(" degrees F");

////   Check if any reads failed and exit early (to try again).
////  if (isnan(humidityData) || isnan(celData) || isnan(fehrData))
////  {
////    Serial.println("Failed to read from DHT sensor!");
////    return;
////  }

  if (isnan(temperatureC) || isnan(temperatureF) )
  {
    Serial.println("Failed to read from TMP36 sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print(" *C ");
  Serial.print(temperatureF);
  Serial.print(" *F\n");


Serial.println("\nSending Data to Server..."); 
  // if you get a connection, report back via serial:
WiFiClient client;  //Instantiate WiFi object, can scope from here or Globally

    //API service using WiFi Client through PushingBox then relayed to Google
    if (client.connect(WEBSITE, 80))
      { 
         client.print("GET /pushingbox?devid=" + devid
       + "&celData="      + (String) temperatureC
       + "&fehrData="     + (String) temperatureF
         );

      // HTTP 1.1 provides a persistent connection, allowing batched requests
      // or pipelined to an output buffer
      client.println(" HTTP/1.1"); 
      client.print("Host: ");
      client.println(WEBSITE);
      client.println("User-Agent: MKR1000/1.0");
      //for MKR1000, unlike esp8266, do not close connection
      client.println();
      Serial.println("\nData Sent"); 
      
      client.stop(); //Ahmed added this to help slove the issue: "Only the first couple of temperatures get transferred to the google sheet then the rest of the data stops getting sent"
      }

      
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
