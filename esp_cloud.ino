/*
 * DHT Temperature and humidity monitoring using ESP32 and  askSensors Iot service
 * Description: This examples connects the ESP32 to wifi, and sends Temperature and humidity to askSensors IoT platfom over HTTP GET Request.
 *  Author: https://asksensors.com, 2018
 *  github: https://github.com/asksensors
 */

// includes
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

WiFiMulti WiFiMulti;
HTTPClient ask;

// user config: TODO
const char* ssid     = "TNT_KOST"; //Wifi SSID
const char* password = "100111011"; //Wifi Password
const char* apiKeyIn = "mgcmABflcLgmBQTwU7cWB1oTGb8wcGhi"; // API Key
const unsigned int writeInterval = 25000; // write interval (in ms)

// ASKSENSORS config.
const char* host = "api.asksensors.com";  // ASKSENSORS API host name
const int httpPort = 80;      // port

// DHT config.
#define DHTPIN            13         // Pin which is connected to the DHT sensor.
// Uncomment the type of sensor in use:
#define DHTTYPE           DHT11     // DHT 11 

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
int status = WL_IDLE_STATUS;
float myTemperature = 0, myHumidity = 0; 
// create ASKSENSORS client
// 
void setup() {
 // open serial
  Serial.begin(115200);
   Serial.println("Menghubungkan ESP32 dan DHT ke Cloud");
  Serial.println("Menghubungkan WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi terhubung!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize device.
  dht.begin();
  Serial.println("Suhu dan Kelembapan");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop() {
   
   
    
// Read data from DHT
// Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Tidak dapat membaca Suhu!");
  }
  else {
    // Update temperature and humidity
    myTemperature = (float)event.temperature;
    Serial.print("Suhu: ");
    Serial.print(myTemperature);
    Serial.println(" C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Tidak dapat membaca kelembaban!");
  }
  else {
     myHumidity = (float)event.relative_humidity;
    Serial.print("Kelembaban: ");
    Serial.print(myHumidity);
    Serial.println("%");
  }
  
// Use WiFiClient class to create TCP connections
  WiFiClient client;


  if (!client.connect(host, httpPort)) {
    Serial.println("Koneksi gagal");
    return;
  }else {
    
    // Create a URL for the request
  String url = "https://api.asksensors.com/write/";
  url += apiKeyIn;
  url += "?module1=";
  url += myTemperature;
  url += "&module2=";
  url += myHumidity;
  
  Serial.print("requesting URL: ");
  Serial.println(url);
  
   ask.begin(url); //Specify the URL
  
    //Check for the returning code
    int httpCode = ask.GET();          
 
    if (httpCode > 0) { 
 
        String payload = ask.getString();
        Serial.println(httpCode);
        Serial.println(payload);
      } else {
      Serial.println("Error on HTTP request");
    }
 
    ask.end(); //End 
    Serial.println("*****************************************************");
  }

  client.stop();  // stop client
  
  delay(writeInterval);    // delay
}
