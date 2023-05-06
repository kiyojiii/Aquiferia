#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Capacitive Soil Moisture Sensor Configuration
const int sensorPin = A0;
int moisturePercent = 0;

// Wifi Details
const char* ssid = "NoFreeWifi-ext";
const char* password = "Velasquez101320!";

// DHT11 Configuration
#define DHTPIN 5  // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

//Firebase Portion

/* 1. Define the WiFi credentials */
#define WIFI_SSID "NoFreeWifi-ext"
#define WIFI_PASSWORD "Velasquez101320!"

/* 2. Define the API Key */
#define API_KEY "AIzaSyDOIDgSedybvSSPD2TejDmtdD0SZNoPS7U"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "aquiferia-f9786"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "jibreelgwapo@gmail.com"
#define USER_PASSWORD "123456"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

String uid;

String path = "MoistureRecords"; // set the Firestore collection id here

void setup()
{

    Serial.begin(9600);
    Serial.println("**********************************");
    Serial.println("Welcome to Aquiferia Sensor");
    Serial.println("**********************************");
    delay(2000);
    Serial.println("Initializing...");
    delay(1000);
    
    timeClient.begin();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
	Serial.println("");
	Serial.print("WiFi connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
	
	  delay(3000); // delay start for 3 seconds

  // Calibrate the Moisture Sensor
  Serial.println("Calibrating soil moisture sensor...");
  delay(3000); // delay start for 3 seconds
  Serial.println("...");
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(sensorPin);
    delay(100);
  }
  moisturePercent = map(sum / 10, 0, 1023, -100, 100);
  delay(3000); // delay start for 3 seconds
  Serial.println("Soil Moisture Sensor: [Ready]✓");

  delay(3000); // delay start for 3 seconds
  // Calibrate DHT11 Sensor
  Serial.println("Calibrating DHT11 Sensor...");
  delay(3000); // delay start for 3 seconds
  Serial.println("...");
   // Read DHT11 Sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  delay(3000); // delay start for 3 seconds
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C [Check]✓ ");
  delay(3000); // delay start for 3 seconds
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("% [Check]✓");
  delay(3000); // delay start for 3 seconds
  Serial.println("DHT11 Sensor: [Ready]✓");

  delay(3000); // delay start for 3 seconds
    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    
    Firebase.reconnectWiFi(true);

  delay(3000); // delay start for 3 seconds

  //----------------------------------------------
  // Getting the user UID might take a few seconds
  //-----------------------------------------------
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  //-----------------
  // Print user UID
  //------------------
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
           
}
          

void loop()
{     
// Read Soil Moisture Sensor
int moistureValue = analogRead(sensorPin);
int newMoisturePercent = map(moistureValue, 1023, 0, 0, 100); // Change mapping to 0-100
if (newMoisturePercent < 0) { // Set minimum to 0%
  newMoisturePercent = 0;
}
else if (newMoisturePercent > 100) { // Set maximum to 100%
  newMoisturePercent = 100;
}
if (abs(newMoisturePercent - moisturePercent) > 2) {
  moisturePercent = newMoisturePercent;
}

  // Read DHT11 Sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check for NaN values
  if (isnan(temperature) || isnan(humidity)) {
    Serial.print("Error Getting Data Values");	
  }
  else {
    // Print the values
    Serial.print("");
  }
  
  //-------------------
  //Create Document
  //-------------------
  FirebaseJson content;

  //---------------------
  // Set Soil Metric in the document
  //---------------------
  content.set("fields/Plant Name/stringValue", "Zsa Bhu");
  content.set("fields/Moisture Level/stringValue", String(moisturePercent) + "%");
  content.set("fields/Temperature Level/stringValue", String(temperature) + "°C");
  content.set("fields/Humidity Level/stringValue", String(humidity) + "%");

  //---------------------
  // Set DateTime field with current time
  //---------------------
  timeClient.update();
    time_t now = timeClient.getEpochTime();
    
    if (now == 0) {
      Serial.println("Error getting current time");
    } else {
      // Set timestamp field with current time
      char timestampStr[30];
      strftime(timestampStr, sizeof(timestampStr), "%FT%TZ", gmtime(&now));
      content.set("fields/DateTime/timestampValue", timestampStr);
    }

  //Randomize Document ID 
  String randomStr = "";
  const char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  for (int i = 0; i < 20; i++) {
    randomStr += chars[random(0, 61)];
  }
  uid = randomStr + String(millis());

  Serial.print("Creating document... ");
  delay(3000); // delay start for 3 seconds
  Serial.println("Done");
  delay(3000); // delay start for 3 seconds
  Serial.println("");
  Serial.print("Sending document.... ");

  if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, path.c_str(), content.raw()))
      Serial.printf("Success\n%s\n\n", fbdo.payload().c_str());
  else
  {
    Serial.println(fbdo.errorReason());
  } 
  
  // delay(1 * 60 * 1000); // 1 minutes delay
  delay(15000); // 15 secs delay 
}
