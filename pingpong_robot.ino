#include <IRremote.h>
#include <Servo.h>

#define LASKO_ONOFF 0xA32AB931
#define LASKO_TIMER 0x143226DB
#define LASKO_ROTATION 0xE0984BB6
#define LASKO_MINUS 0x1D2FEFF6
#define LASKO_HIGHLOW 0x371A3C86
#define LASKO_ECO 0x4EA240AE
#define LASKO_PLUS 0x39D41DC6

// 
#define ESC1_PORT 8 
#define ESC2_PORT 9
#define SERVO_PORT 2
#define STATUS_LED 3

#define SERVO_MAX_ANGLE 90

Servo ESC1;     // create servo object to control the ESC1 - Upper motor
Servo ESC2;     // create servo object to control the ESC2 - Lower motor
Servo SERVO;   // direction

// Setup IR receiver 
int RECV_PIN = 5; // IR Receiver - Arduino Digital Pin Number 5
IRrecv irrecv(RECV_PIN);

decode_results results;

// on/off state
bool isOn = false;

// rotation;
bool isRotation = false;
bool isMovingRight = false;
int rotVal = 0; // this rotation will run from 0 to 180, then bonce back to 0

int potValue;  // value from the analog pin

void setup()
{
  // Setup serial output
  Serial.begin(9600);
  irrecv.enableIRIn();

  // Setup LED
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  // Setup ESC
  ESC1.attach(ESC1_PORT,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  ESC2.attach(ESC2_PORT,1000,2000); // (pin, min pulse width, max pulse width in microseconds)

  SERVO.attach(SERVO_PORT);
}

void loop() {
  if (irrecv.decode(&results)) {    
    if (results.value == LASKO_ONOFF) {
        blinkLed();
        isOn = !isOn;
    }else if (results.value == LASKO_ROTATION) {
        blinkLed();
        isRotation = !isRotation;
    }
    irrecv.resume();
  }
  
  delay(40);

  /// MOTOR DRIVER
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)

  if (isOn){
    ESC1.write(potValue);    // Send the signal to the ESC1
    ESC2.write(potValue);    // Send the signal to the ESC2

    // Turn on the green led
    digitalWrite(STATUS_LED, HIGH);
  }else{
    ESC1.write(0);
    ESC2.write(0);
    digitalWrite(STATUS_LED, LOW);
  }

  if (isRotation){
    if (isMovingRight){
      rotVal = rotVal + 2;
    }else{
      rotVal = rotVal - 2;
    }

    if (rotVal <= 0){
      rotVal = 0; 
      isMovingRight = true;
    }
    
    if (rotVal >= SERVO_MAX_ANGLE){
      rotVal = SERVO_MAX_ANGLE; 
      isMovingRight = false;
    }
    
    SERVO.write(rotVal);
  }else{
    SERVO.write(SERVO_MAX_ANGLE);
    isMovingRight = false;
  }
}

void blinkLed(){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)  
}
