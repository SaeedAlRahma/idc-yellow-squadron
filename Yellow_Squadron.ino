// Setup Xbee
#include <Servo.h>
#include <SoftwareSerial.h>
#define Rx 51 // DOUT to pin 51
#define Tx 53 // DIN to pin 53
SoftwareSerial Xbee (Rx, Tx);

// Declare variables
Servo servoLeft;
Servo servoRight;
long rQTI=0, lQTI=0; //right and left line followers
int qtiThreshold = 75; // line follower threshold (black>75 or white<75)
int photoresistor = A2; // photo resistor analog pin
int lightThreshold = 0; // saves the threshold between ambient light and light bulb
int count = 0; // counts number of black strips (stops)
int l1 = 0, l2 = 0, l3=0; // on/off (1/0) light bulbs
int numTurrets = 0; // number of light bulbs (turrets) turned on
int AD=6, B=3, C=2, E=4, F=10, G=5; //pins represent segments on the 7 display segment
int currentCount = 0; // saves the current binary number in base 10
int j=5; //variable to help BOE-Bot stop in position
int bot =1; // send GO order to bots in order

void setup() {
  Xbee.begin(9600);
  Serial.begin(9600);

  // attach the servos to pins 13 and 12
  servoLeft.attach(13);
  servoRight.attach(12); 

  // initialize LED display pins as OUTPUT
  pinMode(AD,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(E,OUTPUT);
  pinMode(F,OUTPUT);
  pinMode(G,OUTPUT);

  // set threshold for ambient light vs light bulb
  int ambientThreshold = 0;
  for (int i=0; i<10; i++){
    ambientThreshold += analogRead(photoresistor);
  }
  ambientThreshold = ambientThreshold/10;
  lightThreshold = ambientThreshold - 100;
}

void loop() {

  // get data from QTI line following sensor
  rQTI = RCTime(8);
  lQTI = RCTime(9);

  // take action based on line followers' data
  // if passed by black strip
  if (lQTI > qtiThreshold  && rQTI > qtiThreshold ){
    // STOP --> action (sense) --> move
    servoLeft.writeMicroseconds(1500);         
    servoRight.writeMicroseconds(1500); 
    count++;

    // if one of the first 3 black strips --> sense and save value
    if (count < 4) {
      // DO SENSING
      int light = lightReading(photoresistor); // light on/off (1/0)?
      if (count ==1) // first lightbulb
      { 
        l1 = light;
        currentCount += l1*1; // update current count of light bulbs in base 10
        showDisplay(currentCount); // show current count of light bulbs
      } 
      else if (count == 2) //second lightbulb
      { 
        l2 = light; 
        currentCount += l2*2; // update current count of light bulbs in base 10
        showDisplay(currentCount); // show current count of light bulbs
      } 
      else if (count == 3) //third lightbulb
      { 
        l3 = light; 
        currentCount += l3*4; // update current count of light bulbs in base 10
        showDisplay(currentCount); // show current count of light bulbs
        delay(5000);
      }
    } 
    else if (count == 4) // stop for communication
    { 
      // Detach the Servos to avoid inereference with Xbee
      disableServos();

      // calculate num of turrets
      numTurrets = (1*l1)+(2*l2)+(4*l3);

      // display number
      showDisplay(numTurrets);

      //stop for communication until signaled to move
      servoLeft.writeMicroseconds(1500);         
      servoRight.writeMicroseconds(1500);
      // either send order to go, or move if it's our turn
      while(bot<=5)
      {
        if(bot == numTurrets)
        {
          // if I'm 5 sendGo for 5 to trigger Pokemon music
          if (bot == 5)
          {
            for (int i=0; i<5;i++)
            {
              Xbee.println('U'); 
            }
          }
          bot = bot + 1;
          break;
        } 
        else {
          sendGo(bot);
          bot = bot + 1;
        } 
      }
    } 
    else if(count > 4) // count down black strips to position
    {
      j--;
      // stop at position when reached
      if(j<numTurrets)
      {
        delay(100);
        servoLeft.writeMicroseconds(1500);         
        servoRight.writeMicroseconds(1500);

        // Detach the Servos to avoid inereference with Xbee
        disableServos();

        while(true)
        {  
          // send GO order to the remaining bots
          while(bot<=5)
          {
            sendGo(bot);
            bot=bot+1; 
          }
        }
      }      
    } 

    // Attach Servos again
    servoLeft.attach(13);
    servoRight.attach(12);

    // half speed forward (to avoid reading black strin twice)
    servoLeft.writeMicroseconds(1600);         // Left wheel counterclockwise
    servoRight.writeMicroseconds(1400);        // Right wheel clockwise
    delay(250);
  } 
  else if (rQTI > qtiThreshold) // right QTI detected black
  {
    // turn right in place until back to white (fix path)
    servoLeft.writeMicroseconds(1600);         // Left wheel counterclockwise
    servoRight.writeMicroseconds(1500);        // Right wheel stop
    delay(50);

  } 
  else if (lQTI > qtiThreshold) // left QTI detected black
  {
    // turn left in place until back to white (fix path)
    servoLeft.writeMicroseconds(1500);         // Left wheel stop
    servoRight.writeMicroseconds(1400);        // Right wheel clockwise
    delay(50);
  }

  else if(lQTI < qtiThreshold && rQTI < qtiThreshold) // both QTI detect white (correct path)
  {
    // half speed forward
    servoLeft.writeMicroseconds(1600);         // Left wheel counterclockwise
    servoRight.writeMicroseconds(1400);        // Right wheel clockwise
    delay(10);
  }
}

/**
  Takes a QTI Line Follower sensor reading using digital output/input
  @Param sensorIn the pin attached to the QTI
  @return long duration of available reading (high is dark)
*/
long RCTime(int sensorIn) {
  long duration = 0;
  pinMode(sensorIn, OUTPUT);       // make pin output
  digitalWrite(sensorIn, HIGH);    // pin HIGH (discharge capacitor)
  delay(1);
  pinMode(sensorIn, INPUT);       // make pin input
  digitalWrite(sensorIn, LOW);    // turn off internal pullups?
  while(digitalRead(sensorIn)) {     // measure reflectivity
    duration++; 
  }
  return duration;
}

/** find light intensity 10 times using analog pin and average them
  @Param sensor pin connected to photoresistor
  @return 1 if light and 0 if dark
*/
int lightReading(int sensor) {
  long reading = 0;
  long avg = 0;
  // get reading 10 times
  for (int i=0; i<10; i++) {
    reading = analogRead(photoresistor);
    avg += reading;
  }
  // average and return 0 if light and 1 if dark
  avg = avg/10;
  if(avg < lightThreshold ){ 
    return 1;   
  }
  else{
    return 0;
  }     
}

/** shows display on the 7 segment display
  @Param num number between 1-5 to display
*/
void showDisplay(int num) {
  if (num == 1){
    digitalWrite(AD,LOW);
    digitalWrite(B,HIGH);
    digitalWrite(C,HIGH);
    digitalWrite(E,LOW);
    digitalWrite(F,LOW);
    digitalWrite(G,LOW);
  } 
  else if (num == 2){
    digitalWrite(AD,HIGH);
    digitalWrite(B,HIGH);
    digitalWrite(C,LOW);
    digitalWrite(E,HIGH);
    digitalWrite(F,LOW);
    digitalWrite(G,HIGH);
  } 
  else if (num == 3){
    digitalWrite(AD,HIGH);
    digitalWrite(B,HIGH);
    digitalWrite(C,HIGH);
    digitalWrite(E,LOW);
    digitalWrite(F,LOW);
    digitalWrite(G,HIGH);
  } 
  else if (num == 4){
    digitalWrite(AD,LOW);
    digitalWrite(B,HIGH);
    digitalWrite(C,HIGH);
    digitalWrite(E,LOW);
    digitalWrite(F,HIGH);
    digitalWrite(G,HIGH);
  } 
  else if (num == 5){
    digitalWrite(AD,HIGH);
    digitalWrite(B,LOW);
    digitalWrite(C,HIGH);
    digitalWrite(E,LOW);
    digitalWrite(F,HIGH);
    digitalWrite(G,HIGH);
  }
}

/** For Master Robot to send "Go" order and waits for "Going" and "Arrived" confirmation
    Skips after 5 seconds of no response
    @Param otherBotNum number of bot to send the order to
*/
void sendGo(int otherBotNum)
{
  // I-J-K is Go-Going-Arrived for Bot 1
  // L-M-N is Go-Going-Arrived for Bot 2
  // O-P-Q is Go-Going-Arrived for Bot 3
  // R-S-T is Go-Going-Arrived for Bot 4
  // U-V-W is Go-Going-Arrived for Bot 5

  char codeMsg;
  char incoming;
  char expectedMsg;
  boolean skip = false;

  // Get codeMsg based on Bot Num
  switch(otherBotNum)
  {
  case 1: 
    codeMsg = 'I';
    break;
  case 2: 
    codeMsg = 'L';
    break;
  case 3: 
    codeMsg = 'O';
    break;
  case 4: 
    codeMsg = 'R';
    break;
  case 5: 
    codeMsg = 'U';
    break;
  default: 
    codeMsg = 'I'; // go with number 1
  }
  expectedMsg = codeMsg; // set it for later use

  // send GO 10 times
  for (int i=0; i<10; i++)
  {
    Xbee.println(codeMsg);
    //
    Serial.print("GO ");
    Serial.println(codeMsg);
    //
    delay(50);
  }

  // Get time to check for Bot response
  unsigned long start = millis(); // save start time
  unsigned long curTime = millis(); // save current time
  unsigned long wait = curTime - start; // save wait time

  // wait for Bot confirmation msg that it started moving)
  expectedMsg = expectedMsg + 1; // get the next alphabetical char (order for Going for this bot)
  while(true)
  {
    curTime = millis();
    wait = curTime - start; // update wait time on every loop
    Serial.print("wait ");
    Serial.println(wait);

    if(Xbee.available())
    {
      incoming = Xbee.read();
      //
      Serial.print("incoming going ");
      Serial.println(incoming);
      //

      if(incoming == expectedMsg)
      {
        Serial.println("HE IS GOING!!");
        break;
      } 
    }
    if (wait > 5000) //if waited more than 10 secs, then skip
    {
      Serial.println("I SKIPPED!!!! ");
      skip = true;
      break;
    }

  }

  // wait for Bot to arrive at position and send msg
  if(!skip) //skip this if Bot not responding
  {
    expectedMsg = expectedMsg+1; // get the next alphabetical char (order for Arrived for this bot)
    while(true)
    {
      if(Xbee.available())
      {
        incoming = Xbee.read();
        //
        Serial.print("incoming arrived  ");
        Serial.println(incoming);
        //

        if(incoming == expectedMsg)
        {
          Serial.println("HE HAS ARRIVED!!");
          break;
        }
      }
    }
  }
  Serial.print("I'M OUT!!! ");
}

/** Detach the 2 servos
*/
void disableServos()
{
  servoLeft.detach();
  servoRight.detach();
}
