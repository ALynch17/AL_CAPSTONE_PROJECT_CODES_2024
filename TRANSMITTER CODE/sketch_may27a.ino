#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#define RXPin 2
#define TXPin 3
#define RYLR896_RX 4
#define RYLR896_TX 5
#define ledpin 6
#define xPin A1
#define yPin A2
#define zPin A3
#define batteryPin A0 // Analog input pin for battery voltage
// Define the reference voltage for the ADC
const float referenceVoltage = 3.3;

// Define the voltage divider ratio (with 1kΩ and 2kΩ resistors)
const float voltageDividerRatio = 3.0 / 2.0;

// Define the minimum and maximum voltage of the battery
const float minBatteryVoltage = 2.7; // Minimum voltage considered as 0%
const float maxBatteryVoltage = 3.4; // Maximum voltage considered as 100%
SoftwareSerial mySerial(RYLR896_RX, RYLR896_TX); // RX, TX
TinyGPSPlus gps;
SoftwareSerial SerialGPS(RXPin, TXPin);
float z;
String data;

// Number of samples for averaging
const int numSamples = 10;

// Number of samples for dynamic threshold calculation
const int calibrationSamples = 100;

// Variables to store the previous average values
int prevX = 0;
int prevY = 0;
int prevZ = 0;

// Dynamic threshold value
int threshold = 0;
// Function to read and average multiple samples
int readAverage(int pin) {
  long sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += analogRead(pin);
    delay(10); // Small delay between samples
  }
  return sum / numSamples;
}

// Function to calculate the dynamic threshold
int calculateDynamicThreshold() {
  int sumDeltaX = 0;
  int sumDeltaY = 0;
  int sumDeltaZ = 0;

  int lastX = readAverage(xPin);
  int lastY = readAverage(yPin);
  int lastZ = readAverage(zPin);

  for (int i = 0; i < calibrationSamples; i++) {
    int currentX = readAverage(xPin);
    int currentY = readAverage(yPin);
    int currentZ = readAverage(zPin);

    sumDeltaX += abs(currentX - lastX);
    sumDeltaY += abs(currentY - lastY);
    sumDeltaZ += abs(currentZ - lastZ);

    lastX = currentX;
    lastY = currentY;
    lastZ = currentZ;

    delay(10);
  }

  int avgDeltaX = sumDeltaX / calibrationSamples;
  int avgDeltaY = sumDeltaY / calibrationSamples;
  int avgDeltaZ = sumDeltaZ / calibrationSamples;

  // Set the threshold as the maximum average delta with a margin
  return max(max(avgDeltaX, avgDeltaY), avgDeltaZ) + 5; // Adjust margin as needed
}
void sendData(int movem,float per)
{
  float latitude = gps.location.lat();
  float longitude = gps.location.lng();
  
data=String(latitude)+","+String(longitude)+","+String(movem)+","+String(per);
String cmd = "AT+SEND=11, "+String(data.length())+","+data;

      mySerial.println(cmd);
      while(mySerial.available())
  {
    Serial.write(mySerial.read());
    
  }
    Serial.println();
    Serial.println(cmd);
      digitalWrite(ledpin,HIGH);
      delay(500);
      digitalWrite(ledpin,LOW);
      delay(500);
    }
void setup() {
  // Initialize hardware serial communication for debugging
  Serial.begin(9600);

  // Initialize SoftwareSerial communication for RYLR896
  mySerial.begin(9600);
  SerialGPS.begin(9600);
  pinMode(ledpin,OUTPUT);

   // Calculate initial average values
  prevX = readAverage(xPin);
  prevY = readAverage(yPin);
  prevZ = readAverage(zPin);

  // Calculate the dynamic threshold based on initial readings
  threshold = calculateDynamicThreshold();
}

void loop() {
    int moving;
    int analogValue = analogRead(batteryPin);

  // Convert the raw analog value to a voltage
  float batteryVoltage = (analogValue * referenceVoltage) / 1023.0 * voltageDividerRatio;

  // Calculate the battery percentage
  float batteryPercentage = (batteryVoltage - minBatteryVoltage) / (maxBatteryVoltage - minBatteryVoltage) * 100.0;

  // Constrain the battery percentage to be between 0% and 100%
  batteryPercentage = constrain(batteryPercentage, 0, 100);
  int currentX = readAverage(xPin);
  int currentY = readAverage(yPin);
  int currentZ = readAverage(zPin);

  // Calculate the difference between the current and previous average values
  int deltaX = abs(currentX - prevX);
  int deltaY = abs(currentY - prevY);
  int deltaZ = abs(currentZ - prevZ);

  // Check if the movement exceeds the dynamic threshold
  if (deltaX > threshold || deltaY > threshold || deltaZ > threshold) {
    moving=1; // Movement detected
  } else {
    moving=0; // No movement
  }

  // Update the previous average values
  prevX = currentX;
  prevY = currentY;
  prevZ = currentZ;
  int batteryPerc;
    z+=0.5;
    batteryPerc=batteryPercentage-z;
    if (batteryPerc==0)
    {
    batteryPerc=batteryPercentage;
    z=0;
    }
   while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read())) 
  sendData(moving,batteryPerc);
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("GPS NOT DETECTED!");
    while(true);
  }
  delay(1000); // Wait for 5 seconds before sending next message
}
