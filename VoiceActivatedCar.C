#define servoPin 10 
int pos;            
int pulsewidth;     
#define SCL_Pin  A5  
#define SDA_Pin  A4  
#define ML_Ctrl 4     
#define ML_PWM 5     
#define MR_Ctrl 2     
#define MR_PWM 6     
#define TRIG_PIN 12   
#define ECHO_PIN 13  
#include "SR04.h"

SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long distance;        
int ledpin = 9;      
unsigned char backward_pattern[] = {0x00, 0x00, 0x00, 0x10, 0x30, 0x60, 0x40, 0xff, 0xff, 0x40, 0x60, 0x30, 0x10, 0x00, 0x00, 0x00};
unsigned char forward_pattern[]  = {0x00, 0x00, 0x00, 0x08, 0x0c, 0x06, 0x02, 0xff, 0xff, 0x02, 0x06, 0x0c, 0x08, 0x00, 0x00, 0x00};
unsigned char right_pattern[]    = {0x18, 0x3c, 0x7e, 0xdb, 0x99, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
unsigned char left_pattern[]     = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x99, 0xdb, 0x7e, 0x3c, 0x18};
unsigned char clear[]            = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void procedure(int myangle); 
void matrix_display(unsigned char matrix_value[]);
void IIC_start();
void IIC_send(unsigned char send_data);
void IIC_end();
void back();
void front();
void turnLeft();
void turnRight();
void Stop();

void setup() {
  Serial.begin(9600);
  delay(1000); 

while (Serial.available() > 0) {
    Serial.read();
  }
  
  pinMode(ML_Ctrl, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_Ctrl, OUTPUT);
  pinMode(MR_PWM, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ledpin, OUTPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
  matrix_display(clear);  
  Stop();
  
  
  pinMode(servoPin, OUTPUT);
  procedure(0);
  delay(1000);    
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.print("Voice Command Received: ");
    Serial.println(command);
    
    bool commandRecognized = false;
    
    if (command.length() > 0) {
      if (command.equalsIgnoreCase("backward")) {
        commandRecognized = true;
        back();
        matrix_display(backward_pattern);
        delay(3000);  
        Stop();
        matrix_display(clear);
      }
      else if (command.equalsIgnoreCase("forward")) {
        commandRecognized = true;
        front();
        matrix_display(forward_pattern);
        delay(3000);   
        Stop();
        matrix_display(clear);
      }
      else if (command.equalsIgnoreCase("left")) {
        commandRecognized = true;
        turnLeft();
        matrix_display(left_pattern);
        delay(3000);   
        Stop();
        matrix_display(clear);
      }
      else if (command.equalsIgnoreCase("right")) {
        commandRecognized = true;
        
        for (pos = 0; pos <= 90; pos++) {
          procedure(pos);
          delay(15);  
        }
        delay(1000); 
        
      
        turnRight();
        digitalWrite(ledpin, HIGH);  
        matrix_display(right_pattern);
        delay(3000);   
        Stop();
        digitalWrite(ledpin, LOW);   
        matrix_display(clear);
        
        for (pos = 90; pos >= 0; pos--) {
          procedure(pos);
          delay(15); 
        }
        delay(1000); 
      }
    }
    
    if (!commandRecognized) {
      Stop();
    }
  } 
  else {
    Stop();
  }
}


void procedure(int myangle) {
  
  myangle = 180 - myangle;
  pulsewidth = myangle * 11 + 500;
  
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulsewidth);    
  digitalWrite(servoPin, LOW);
  
  delay(20 - pulsewidth / 1000);
}

void back() {
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, 100);
  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, 100);
}

void front() {
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 100);
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 100);
}

void turnLeft() {
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 200); 
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 50);   
}

void turnRight() {
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, 50);   
  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, 200);  
}


void Stop() {
  analogWrite(ML_PWM, 0);
  analogWrite(MR_PWM, 0);
}

void matrix_display(unsigned char matrix_value[])
{
    IIC_start();           
    IIC_send(0xc0);        
    for (int i = 0; i < 16; i++) {  
        IIC_send(matrix_value[i]);
    }
    IIC_end();           
    IIC_start();
    IIC_send(0x8A);        
    IIC_end();
}


void IIC_start()
{
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, LOW);
    delayMicroseconds(3);
}

void IIC_send(unsigned char send_data)
{
    for (char i = 0; i < 8; i++) {  
        digitalWrite(SCL_Pin, LOW);
        delayMicroseconds(3);
        if (send_data & 0x01) {
            digitalWrite(SDA_Pin, HIGH);
        } else {
            digitalWrite(SDA_Pin, LOW);
        }
        delayMicroseconds(3);
        digitalWrite(SCL_Pin, HIGH);
        delayMicroseconds(3);
        send_data = send_data >> 1;
    }
}

void IIC_end()
{
    digitalWrite(SCL_Pin, LOW);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, LOW);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SDA_Pin, HIGH);
    delayMicroseconds(3);
}
