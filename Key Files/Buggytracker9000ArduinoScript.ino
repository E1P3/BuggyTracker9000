// All relevant code for this project is available at:
// https://github.com/E1P3/BuggyTracker9000


#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "MotorController.h"
#include <Arduino_LSM6DS3.h>

//motor pins
#define M1_DIR1 A2
#define M1_DIR2 11
#define M2_DIR1 A3
#define M2_DIR2 12 

//IR pins
#define IR_LEFT 2
#define IR_RIGHT 3

//========================WIFI STUFF=================================

WiFiClient client;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int port = 8080;

WiFiServer server(port);

char c;                                                                     // variable for recieving go and stop commands from processing
bool RIDE_ENABLE = LOW;                                                     // buggy does not move when first turned on



//====================HARDWARE CONTROL STUFF=========================

MotorController motor;

bool darkMode = LOW;                                                         // HIGH = black track, LOW = white track
volatile int dir = 0;                                                        // direction variable, 1 = forward 3 = left 4 = right works in reverse when currentSpeed < 0
long distance = 0;

// gyro

volatile int currentSpeed = 255;

float x, y, px, py;
float gx, gy, gz;
float angleX, angleY, angleZ;
float GyroErrorX;
float GyroErrorY;
float GyroErrorZ;
float elapsedTime, currentTime, previousTime;
float velapsedTime, vcurrentTime, vpreviousTime;
float gyroOffset = 1.15;
int count = 0;
float velocity;



void setup(){

  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  delay(500);
  computeError();

  motor = MotorController(M1_DIR1, M1_DIR2, M2_DIR1, M2_DIR2, 0.9, 1);
  
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  attachInterrupt(digitalPinToInterrupt(IR_LEFT), Turn, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(IR_RIGHT), Turn, CHANGE);
  
  WiFi.begin(ssid, pass);
  server.begin();

  
  
}

void loop() {
 
  client = server.available();
  
  while (client.connected()) {

    updateAngle();
    
    c = client.read();
   
    if(c == 'g'){
      RIDE_ENABLE = HIGH;
      currentSpeed = 255;
      dir = 1;
    } else if(c == 's'){
      RIDE_ENABLE = LOW; 
    }

    
    if(RIDE_ENABLE == HIGH){

      motor.moveTo(dir, currentSpeed);
      if(dir != 0){
        updateVelocityVector();  
      }
         
    } else {
      
      dir = 0;
      motor.moveTo(0, 0);
                                                           
    }

    client.print(x);
    client.print('/');
    client.print(y);
    client.print('/');
    client.println(angleZ);

  }
  
}

void Turn(){
  
  if(digitalRead(IR_RIGHT) == darkMode && digitalRead(IR_LEFT) == darkMode){
    dir = 1;  
  } else if(digitalRead(IR_RIGHT) == darkMode){
    dir = 3;  
  } else if(digitalRead(IR_LEFT) == darkMode ){
    dir = 4;  
  } else {
    dir = 0;  
  }
}

void computeError(){
  
  noInterrupts();
  count = 0;
  // Read gyro values 200 times
  while (count < 200) {
    
    IMU.readGyroscope(gx, gy, gz);
  
    // Sum all readings
    GyroErrorX = GyroErrorX - gx;
    GyroErrorY = GyroErrorY - gy;
    GyroErrorZ = GyroErrorZ - gz;
    count++;
    delay(10);
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  interrupts();

}

void updateVelocityVector(){

    velocity = 0.2;

    px = x; py = y;

    x = x + velocity * sin(angleZ * PI/180);
    y = y + velocity * cos(angleZ * PI/180);

}

void updateAngle(){
  
    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000;
    
    IMU.readGyroscope(gx, gy, gz);

    gz = gz + GyroErrorZ;
    
    angleZ = angleZ +  gz * elapsedTime * gyroOffset;

    if(angleZ > 360){
      angleZ = angleZ - 360;  
    } else if(angleZ < -360){
      angleZ = angleZ + 360;  
    }

}
