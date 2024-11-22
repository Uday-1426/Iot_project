#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <ESPAsyncWebServer.h>

// Motor Pins
const int in1 = 12;
const int in2 = 14;
const int in3 = 27;
const int in4 = 26;
const int ena = 25;
const int enb = 33;

// Ultrasonic Sensor Pins
const int trigPin = 5;
const int echoPin = 18;

// IR Sensor Pins
#define IR_SENSOR_LEFT 17
#define IR_SENSOR_RIGHT 16

// Constants
const float wheelCircumference = 0.2046;  // Circumference in meters (204.6 mm)
const int pulsesPerRevolution = 20;       // Encoder pulses per wheel revolution

// Variables
volatile int leftRotationCount = 0;
volatile int rightRotationCount = 0;
bool lastStateLeft = LOW;
bool lastStateRight = LOW;

Adafruit_MPU6050 mpu;
AsyncWebServer server(80);

// Wi-Fi Credentials
const char* ssid = "lab14";
const char* password = "expert1234";

// Sensor Data Variables
long duration;
int distance;
unsigned long lastTime = 0;
unsigned long interval = 1000;  // Data update interval (ms)

// ISR for Left Wheel
void IR_Left_ISR() {
  bool currentState = digitalRead(IR_SENSOR_LEFT);
  if (currentState != lastStateLeft) {
    if (currentState == HIGH) leftRotationCount++;
    lastStateLeft = currentState;
  }
}

// ISR for Right Wheel
void IR_Right_ISR() {
  bool currentState = digitalRead(IR_SENSOR_RIGHT);
  if (currentState != lastStateRight) {
    if (currentState == HIGH) rightRotationCount++;
    lastStateRight = currentState;
  }
}

void setup() {
  Serial.begin(115200);

  // Motor Pins Setup
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);

  // Ultrasonic Sensor Pins Setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // IR Sensors Setup
  pinMode(IR_SENSOR_LEFT, INPUT);
  pinMode(IR_SENSOR_RIGHT, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_LEFT), IR_Left_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_RIGHT), IR_Right_ISR, CHANGE);

  // MPU6050 Setup
  Wire.begin(21, 22);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }
  Serial.println("MPU6050 Initialized");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Wi-Fi Setup
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    String jsonData = getSensorData();
    request->send(200, "application/json", jsonData);
  });
  server.begin();
}

void loop() {
  // Measure Distance
  measureDistance();

  // Stop if obstacle is detected
  if (distance <= 10) {  // Adjust threshold as needed
    stopMotors();
  } else {
    moveForward();
  }

  // Send data every second
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= interval) {
    Serial.println(getSensorData());
    lastTime = currentTime;
  }
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
}

void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ena, 255);
  analogWrite(enb, 255);
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(ena, 0);
  analogWrite(enb, 0);
}

String getSensorData() {
  // Read MPU6050 Data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calculate Wheel Speed and Rotations
  static int prevLeftCount = 0, prevRightCount = 0;
  float leftRotations = (leftRotationCount - prevLeftCount) / (float)pulsesPerRevolution;
  float rightRotations = (rightRotationCount - prevRightCount) / (float)pulsesPerRevolution;

  float leftSpeed = leftRotations * (60000.0 / interval);  // RPM
  float rightSpeed = rightRotations * (60000.0 / interval);

  prevLeftCount = leftRotationCount;
  prevRightCount = rightRotationCount;

  // Construct JSON Data
  String jsonData = "{";
  jsonData += "\"acceleration_x\":" + String(a.acceleration.x, 2) + ",";
  jsonData += "\"acceleration_y\":" + String(a.acceleration.y, 2) + ",";
  jsonData += "\"acceleration_z\":" + String(a.acceleration.z, 2) + ",";
  jsonData += "\"gyro_x\":" + String(g.gyro.x, 2) + ",";
  jsonData += "\"gyro_y\":" + String(g.gyro.y, 2) + ",";
  jsonData += "\"gyro_z\":" + String(g.gyro.z, 2) + ",";
  jsonData += "\"temperature\":" + String(temp.temperature, 2) + ",";
  jsonData += "\"distance\":" + String(distance) + ",";
  jsonData += "\"left_rotations\":" + String(leftRotationCount) + ",";
  jsonData += "\"right_rotations\":" + String(rightRotationCount) + ",";
  jsonData += "\"left_speed\":" + String(leftSpeed, 2) + ",";
  jsonData += "\"right_speed\":" + String(rightSpeed, 2);
  jsonData += "}";

  return jsonData;
}
