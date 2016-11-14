#include <SoftwareSerial.h>
#define Rx 10 // DOUT to pin10
#define Tx 11 // DIN to pin 11
SoftwareSerial Xbee (Rx, Tx);

// reads messages sent from Master Bot until it's your turn
void readMessage(int botNum)
{
  char incoming;
  char codeMsg;
  char expectedMsg;

  // Get codeMsg based on Bot Num
  switch(botNum)
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

  // wait for order message
  while(true)
  {
    if(Xbee.available())
    {
      incoming = Xbee.read();
      if(incoming == expectedMsg)
      {
        break;
      }
    }
  }
}

// Send "I'M GOING" order message
void sendGoing(int botNum)
{ 
  char codeMsg;
  char expectedMsg;

  // get GOING code
  switch(botNum)
  {
  case 1: 
    codeMsg = 'J';
    break;
  case 2: 
    codeMsg = 'M';
    break;
  case 3: 
    codeMsg = 'P';
    break;
  case 4: 
    codeMsg = 'W';
    break;
  case 5: 
    codeMsg = 'V';
    break;
  default: 
    codeMsg = 'J'; // go with number 1
  }
  expectedMsg = codeMsg;

  {
    // send GOING code 10 times to Master Bot
    for (int i=0; i<10; i++)
    {
      Xbee.println(codeMsg);
      delay(100);
    }
  }
}

// Send "I ARRIVED" order message
void sendArrived(int botNum)
{
  char codeMsg;
  char expectedMsg;

  // get Arrived code
  switch(botNum)
  {
  case 1: 
    codeMsg = 'K';
    break;
  case 2: 
    codeMsg = 'N';
    break;
  case 3: 
    codeMsg = 'Q';
    break;
  case 4: 
    codeMsg = 'T';
    break;
  case 5: 
    codeMsg = 'W';
    break;
  default: 
    codeMsg = 'K'; // go with number 1
  }
  expectedMsg = codeMsg;
  {
    // send ARRIVED code 10 times to Master Bot
    for (int i=0; i<10; i++)
    {
      Xbee.println(codeMsg);
      delay(100);
    }
  }
}



