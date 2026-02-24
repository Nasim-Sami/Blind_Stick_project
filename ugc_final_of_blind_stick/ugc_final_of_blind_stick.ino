#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
Adafruit_MPU6050 mpu;

const int MPU2=0x69;
int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
float ax2=0, ay2=0, az2=0, gx2=0, gy2=0, gz2=0;
boolean trigger1n=false; 
boolean trigger2n=false; 
boolean trigger3n=false; 
byte trigger1countn=0;
byte trigger2countn=0;
byte trigger3countn=0; 
int angleChange2=0;

float rotation;
unsigned long prev=0;
unsigned long current=0;

unsigned long currentTime=0;
unsigned long previousTime=0;

boolean fall = false;
boolean trigger1=false; //stores if first trigger (lower threshold) has occurred
boolean trigger2=false; //stores if second trigger (upper threshold) has occurred
boolean trigger3=false; //stores if third trigger (orientation change) has occurred
byte trigger1count=0; //stores the counts past since trigger 1 was set true
byte trigger2count=0; //stores the counts past since trigger 2 was set true
byte trigger3count=0;

String d;

const int MPU_addr=0x68;  // I2C address of the MPU-6050

#define FIREBASE_HOST "blindstick-a0ee0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "iaGnAiAOnxhHFtrGCYpXZpHDvJd6KW3lb1FHvMrO"
#define WIFI_SSID "sami"
#define WIFI_PASSWORD "sami1234"
FirebaseData firebaseData;
FirebaseJson json;
// Define the trigger and echo pins
const int trigPin = D5;
const int echoPin =  D7;
const int buzzer  =  D6;
String s1 = "headphone";

// Define the sound speed in centimeters per second
const int soundSpeed = 340;
float duration,distance;

void GetMpuValue2(const int MPU){ 
   
      Wire.beginTransmission(MPU); 
      Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) 
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 14, true); // request a total of 14 registers 
      AcX2=Wire.read()<<8| Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
      AcY2=Wire.read()<<8|  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcZ2=Wire.read()<<8| Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L) 
      Tmp2=Wire.read()<<8| Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) 
      GyX2=Wire.read()<<8| Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L) 
      GyY2=Wire.read()<<8| Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L) 
      GyZ2=Wire.read()<<8| Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L) 
      Serial.println("gyro sensor 2: ");
      Serial.print("AcX2 = ");
      Serial.print(AcX2);
      Serial.print(" | AcY2 = "); 
      Serial.print(AcY2);
      Serial.print(" | AcZ2 = ");
      Serial.print(AcZ2);  
      Serial.print(" | GyX2 = ");
      Serial.print(GyX2); 
      Serial.print(" | GyY2 = "); 
      Serial.print(GyY2);
      Serial.print(" | GyZ2 = ");
      Serial.println(GyZ2); 
    
     }


void setup() {
  // Set the trigger and echo pins as outputs and inputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer,OUTPUT);
  Serial.begin(9600);

  Wire.begin();
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);// PWR_MGMT_1 register 
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
 
  Serial.println("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD );
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); 
    delay(200);           
  }
 Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  currentTime=millis();
  prev=currentTime;
  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigger pin high for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
   

  // Read the echo pin and calculate the duration of the pulse
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  distance = duration * 0.034;
  Serial.println();
  Serial.print("Distance = ");
  Serial.println(distance);

  if(Firebase.getString(firebaseData, "/Finder/mode"))
  {
    
     d=firebaseData.stringData();
   Serial.println(d);
  }

   if(d == "11")
  {
    Serial.println("both mode");
    if( distance<70)
   {digitalWrite(buzzer,HIGH);
  Firebase.setInt(firebaseData, "/Finder/OnSound",1);}
  
   else
  {digitalWrite(buzzer,LOW);
  Firebase.setInt(firebaseData, "/Finder/OnSound",0);}
  }
   else if(d== "1")
  { 
    Serial.println("buzzer mode");
    if( distance<120)
   {digitalWrite(buzzer,HIGH);
  Firebase.setInt(firebaseData, "/Finder/OnSound",0);
  }
   else
  {digitalWrite(buzzer,LOW);
  Firebase.setInt(firebaseData, "/Finder/OnSound",0);}
  }
  
  else
  { Serial.println("phone mode");
  if(distance<120)
  { digitalWrite(buzzer,LOW);
  Firebase.setInt(firebaseData, "/Finder/OnSound",1);}
   else
  {digitalWrite(buzzer,LOW);
  Firebase.setInt(firebaseData, "/Finder/OnSound",0);}
  }

  //fall detection
   sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(180*g.gyro.x/3.1416);
  Serial.print(", Y: ");
  Serial.print(180*g.gyro.y/3.1416);
  Serial.print(", Z: ");
  Serial.println(180*g.gyro.z/3.1416);
  Serial.print(" rotation=");
  rotation=pow(pow(180*g.gyro.x/3.1416,2)+pow(180*g.gyro.y/3.1416,2)+pow(180*g.gyro.z/3.1416,2),0.5);
  Serial.println(pow(pow(180*g.gyro.x/3.1416,2)+pow(180*g.gyro.y/3.1416,2)+pow(180*g.gyro.z/3.1416,2),0.5));
 
   if(a.acceleration.x>2.0)
   {
     trigger1=true;
     Serial.println();
     Serial.println("trigger1 activated");
   }
     if(trigger1==true)
     { trigger1count++;
       if(a.acceleration.x<2.0)
       {
         trigger2=true;
         trigger1=false;
         Serial.println("trigger1 deactivated");
         Serial.println("trigger2 activated");
       }
     }
   if(trigger2==true) 
   {  trigger2count++;
    
       if(rotation>60) 
       {
         Serial.println("Fall detected");
        Firebase.setInt(firebaseData, "/Finder/falled",1);
        previousTime=currentTime;
        trigger2=false;
        Serial.print("trigger2 deactivated");}
      
   }
   if (trigger2count>=6){ //allow 0.5s for orientation change
   trigger2=false; trigger2count=0;
   Serial.println("TRIGGER 2 DECACTIVATED");
   }
 if (trigger1count>=6){ //allow 0.5s for AM to break upper threshold
   trigger1=false; trigger1count=0;
   Serial.println("TRIGGER 1 DECACTIVATED");
   }
  
  GetMpuValue2(MPU2);
   ax2 = (AcX2-2050)/16384.00;
      ay2 = (AcY2-77)/16384.00;
      az2 = (AcZ2-1947)/16384.00;
      gx2 = (GyX2+270)/131.07;
      gy2 = (GyY2-351)/131.07;
      gz2 = (GyZ2+136)/131.07;
      float Raw_Amp2 = pow(pow(ax2,2)+pow(ay2,2)+pow(az2,2),0.5);
      int Amp2 = Raw_Amp2 * 10;  // Mulitiplied by 10 bcz values are between 0 to 1
      Serial.print("Amp2=");
      Serial.println(Amp2);
      float angleChange2 = pow(pow(gx2,2)+pow(gy2,2)+pow(gz2,2),0.5); 
      Serial.print("angle2=");
      Serial.println(angleChange2);
      //delay(250);
      Serial.println("");
  if (Amp2<=3 && trigger2n==false){ //if AM breaks lower threshold (0.4g)
   trigger1n=true;
   Serial.println("TRIGGER 1n ACTIVATED");
   }
 if (trigger1n==true){
   trigger1countn++;
   if (Amp2>=12){ //if AM breaks upper threshold (3g)
     trigger2n=true;
     Serial.println("TRIGGER 2n ACTIVATED");
     trigger1n=false; trigger1countn=0;
     }
 }
 if (trigger2n==true){
   trigger2countn++;
   angleChange2 = pow(pow(gx2,2)+pow(gy2,2)+pow(gz2,2),0.5); 
   if (angleChange2>=30 && angleChange2<=400){ //if orientation changes by between 80-100 degrees
     trigger3n=true; trigger2n=false; trigger2countn=0;
     Serial.println(angleChange2);
     Serial.println("TRIGGER 3n ACTIVATED");
       }
   }
 if (trigger3n==true){
    trigger3countn++;
    if (trigger3countn>=10){ 
       angleChange2 = pow(pow(gx2,2)+pow(gy2,2)+pow(gz2,2),0.5);
       //delay(10);
       Serial.println(angleChange2); 
       if ((angleChange2>=0) && (angleChange2<=10)){ //if orientation changes remains between 0-10 degrees
           fall=true; trigger3n=false; trigger3countn=0;
           Serial.println(angleChange2);
             }
       else{ //user regained normal orientation
          trigger3n=false; trigger3countn=0;
          Serial.println("TRIGGER 3n DEACTIVATED");
       }
     }
  }
 if (fall==true){ //in event of a fall detection
  Serial.println("FALL DETECTED");
  delay(2000);
  Firebase.setInt(firebaseData, "/Finder/falled",1);
  previousTime=currentTime;
  fall=false;
   }
   
 if (trigger2countn>=3){ //allow 0.5s for orientation change
   trigger2n=false; trigger2countn=0;
   Serial.println("TRIGGER 2n DECACTIVATED");
   }
 if (trigger1countn>=3){ //allow 0.5s for AM to break upper threshold
   trigger1n=false; trigger1countn=0;
   Serial.println("TRIGGER 1n DECACTIVATED");
   }
  //prev=currentTime;
 
  if(currentTime-previousTime>15000)
  {
    Firebase.setInt(firebaseData, "/Finder/falled",0);
  }
  
 }
