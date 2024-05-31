#include <WiFi.h>
#include <HTTPClient.h>

const int ledpin = 12;  // LED connected to GPIO pin 12 (13 on ESP32 Dev Board)
String incomingstring;

const char *ssid = "Lynchhhh";
const char *password = "whoops12";

int onoff,offon;

WiFiClient client;

void setup() {
  pinMode(ledpin, OUTPUT);
  
  Serial.begin(9600);  
  while (!Serial);  // Wait for Serial Monitor to open
  
  Serial2.begin(9600, SERIAL_8N1, 16, -1);  // Initialize Serial2 for LoRa communication (RX2 pin 16)
  
  Serial.println("Hardware Serial (Serial2) initialized for LoRa communication.");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
}

void loop() {
 if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
 
   while (Serial2.available()){
  if (Serial2.available()) {
    incomingstring = Serial2.readString();
  }

    if (incomingstring.length() > 0) {
      Serial.println("Received Message: " + incomingstring);
      digitalWrite(ledpin,HIGH);
      delay(250);
      digitalWrite(ledpin,LOW);
      delay(250);
    }

    // Find the position of latitude, longitude,movement, and battery data in the incoming string
    int comma2 = incomingstring.indexOf(',', incomingstring.indexOf(',') + 1); // Find the second comma
    int comma3 = incomingstring.indexOf(',', comma2 + 1); // Find the third comma
    int comma4 = incomingstring.indexOf(',', comma3 + 1); // Find the fourth comma
    int comma5 = incomingstring.indexOf(',', comma4 + 1); // Find the fifth comma
    int comma6 = incomingstring.indexOf(',', comma5 + 1); // Find the sixth comma


    if (comma2 != -1 && comma3 != -1 && comma4 != -1 && comma5!= -1 && comma6!= -1) {
      String longitudeStr = incomingstring.substring(comma3 + 1, comma4); // Extract longitude
      String binaryDataStr = incomingstring.substring(comma4 + 1, comma5); // Extract mstate
      String latitudeStr = incomingstring.substring(comma2 + 1,comma3); // Extract latitude data
      String battStr = incomingstring.substring(comma5 + 1, comma6); // Extract mstate

      // Convert string data to float and int
      float latitude = latitudeStr.toFloat();
      float longitude = longitudeStr.toFloat();
      float battery_status = battStr.toFloat();
      int mstate = binaryDataStr.toInt();
      onoff=1;

      // Build the request body
      String data = "latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&mstate=" + String(mstate) + "&battery_status=" + String(battery_status) + "&onoff=" + String(onoff);

    http.begin(client, "http://192.168.181.228/MyWebsite/RealTimeDataDisplay/process.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Set content type header

    // Send HTTP POST request and handle response
    int httpCode = http.POST(data);

    if (httpCode == HTTP_CODE_OK) 
    {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } 
    
    else
    {
      Serial.print("HTTP POST request failed with error code: ");
      Serial.println(httpCode);
      if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
      {
        Serial.println("Connection refused by the server.");
      }
      else if (httpCode == HTTP_CODE_NOT_FOUND) 
      {
        Serial.println("Server resource not found.");
      }
      else {
        Serial.println("Unknown error occurred.");
      }
    }

     

    http.end(); 
  } else {
    Serial.println("WiFi not connected");
  }

  delay(1000); 

  }
  if (!Serial2.available())
   {
        offon=0;
        String data = "offon=" + String(offon);

    http.begin(client, "http://192.168.181.228/MyWebsite/RealTimeDataDisplay/process.php");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Set content type header

    // Send HTTP POST request and handle response
    int httpCode = http.POST(data);

    if (httpCode == HTTP_CODE_OK) 
    {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } 
    
    else
    {
      Serial.print("HTTP POST request failed with error code: ");
      Serial.println(httpCode);
      if (httpCode == HTTPC_ERROR_CONNECTION_REFUSED)
      {
        Serial.println("Connection refused by the server.");
      }
      else if (httpCode == HTTP_CODE_NOT_FOUND) 
      {
        Serial.println("Server resource not found.");
      }
      else {
        Serial.println("Unknown error occurred.");
      }
    }

  

    http.end(); 

delay(1000); 
  }
}
}