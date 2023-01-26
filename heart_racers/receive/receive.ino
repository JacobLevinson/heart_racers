#include <esp_now.h>
#include <WiFi.h>

//Right motor
int enableRightMotor=27; 
int rightMotorPin1=32;
int rightMotorPin2=33;
//Left motor
int enableLeftMotor=23;
int leftMotorPin1=18;
int leftMotorPin2=19;

int ledPin = 14;

const int PWMFreq = 10000; /* 1 KHz */
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int BPM_data;
    int X_data;
    int Y_data;
    int SWITCH_data;
} struct_message;

// Create a struct_message called myData
struct_message myData;

int button_hist[30];

int speed;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("BPM: ");
  Serial.println(myData.BPM_data);
  Serial.print("X: ");
  Serial.println(myData.X_data);
  Serial.print("Y: ");
  Serial.println(myData.Y_data);
  Serial.print("Switch: ");
  Serial.println(myData.SWITCH_data);
  Serial.println();*/
  Serial.println(myData.BPM_data);
  move();
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    //digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    //digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    //digitalWrite(rightMotorPin2,LOW);      
  }
  
  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    //digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    //digitalWrite(leftMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(leftMotorPin1,LOW);
    //digitalWrite(leftMotorPin2,LOW);      
  } 

  ledcWrite(rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorPWMSpeedChannel, abs(leftMotorSpeed));    
}

void move()
{
  int stopped = 1;

  int temp[30];
  temp[0] = myData.SWITCH_data;
  for(int i = 0; i < 29; i++){
    temp[i+1] = button_hist[i];
  }
  for(int i = 0; i < 30; i++){
    button_hist[i] = temp[i];
    if(temp[i] == 0){
      stopped = 0;
    }
  }
  if(myData.BPM_data <= 80){
    speed = 150;
  } else if (myData.BPM_data>80 && myData.BPM_data<=90){
    speed = 185;    
  } else if (myData.BPM_data>90 && myData.BPM_data<=100){
    speed = 220;
  }else {
    speed = 245;
  }
  //speed = (myData.BPM_data + 80)*(myData.BPM_data + 80)*(myData.BPM_data + 80)/25600;

  if(myData.Y_data > 3500 && !stopped){
    rotateMotor(speed, speed*.6);
  }else if(myData.Y_data < 1500 && !stopped){
    rotateMotor(speed*.6, speed);
  }else if(!stopped){
    rotateMotor(speed, speed);
  } else {
    rotateMotor(0, 0);
  }
}

void setup() {
  pinMode(rightMotorPin1,OUTPUT);
  pinMode(rightMotorPin2,OUTPUT);
  
  pinMode(enableLeftMotor,OUTPUT);
  pinMode(leftMotorPin1,OUTPUT);
  pinMode(leftMotorPin2,OUTPUT);

  pinMode(ledPin, OUTPUT);

  //Set up PWM for motor speed
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);  
  ledcAttachPin(enableRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, leftMotorPWMSpeedChannel); 
  
  rotateMotor(0, 0);
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}