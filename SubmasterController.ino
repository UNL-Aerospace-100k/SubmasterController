//  Testing the XBee
//  Jack Shaver
//  2/20/2024

/*
  This program is written for the Sub-Master Controller V1
  It takes XBee communication on Pins 10 and 11 (RX and TX)
  The wiring for these pins is difficult, from left to right with the board oriented upwards:
  TX, unconnected, RX, unconnected, ...
  on the 6 headers to the right of the arduino nano

  Expected Commands:
  coilOne
  coilTwo
  collectData
  help

  TODO:
  Add the ability to check the state of the data collection.
  Change how the Nano speaks to the teensy for data collection.
*/

#include <SoftwareSerial.h>

SoftwareSerial XBee(10, 11);

#define Error_Comm_LED A0

#define Relay1 2
#define Relay2 3
#define Relay3 4
#define Relay4 5

#define ParallelBit0 9
#define ParallelBit1 8
#define ParallelBit2 7
#define ParallelBit3 6

void setup() {
  pinMode(Error_Comm_LED, OUTPUT);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  
  pinMode(ParallelBit0, OUTPUT);
  pinMode(ParallelBit1, OUTPUT);
  pinMode(ParallelBit2, OUTPUT);
  pinMode(ParallelBit3, OUTPUT);

  digitalWrite(ParallelBit0, LOW);
  digitalWrite(Relay1, LOW);
  digitalWrite(Relay2, LOW);

  XBee.begin(9600);
  
  Serial.begin(9600);
  Serial.println("Testing the XBee");
  Serial.println("Expected Messages: coilOne, coilTwo, collectData, help, ping");
}

String message = "";
char character;
int commandRecieved = 0;

long coilOneTimer;
bool coilOneState = LOW;
long coilTwoTimer;
bool coilTwoState = LOW;
long dataTimer;
bool dataPulse = LOW;
long commTimer;
bool commPulse = LOW;

void loop()
{
  // This statement reads the incoming characters from the XBee, and formats a string
  while(XBee.available())
  {
    digitalWrite(Error_Comm_LED, HIGH);
    commTimer = millis();
    commPulse = HIGH;

    character = XBee.read();
    message.concat(character);
    if(message.lastIndexOf(0x0D) > 0)
    {
      commandRecieved = decodeMessage(message);
      message = "";
    }
  }

  // This statement evaluates what to do when a valid command is recieved
  if(commandRecieved != 0)
  {
    switch(commandRecieved){
      case 1: 
        digitalWrite(Relay1, HIGH);
        coilOneState = HIGH;
        coilOneTimer = millis();
      break;
      case 2:
        digitalWrite(Relay2, HIGH);
        coilTwoState = HIGH;
        coilTwoTimer = millis();
      break;
      case 3:
        digitalWrite(ParallelBit0, HIGH);
        dataPulse = HIGH;
        dataTimer = millis();
      break;
      case 4:
        XBee.println("XBee Serial Interface with Sensor Monitor");
        XBee.println("Commands: coilOne, coilTwo, collectData, help, ping");
      break;
      case 5:
        XBee.println("XBee_B Successfully Pinged");
      break;
    }
    commandRecieved = 0;
  }

  // These if cases handle returning the IO back to their resting after a time period
  if((coilOneState == HIGH) && ((millis() - coilOneTimer) > 1000)){
    coilOneState = LOW;
    digitalWrite(Relay1, LOW);
  }
  if((coilTwoState == HIGH) && ((millis() - coilTwoTimer) > 1000)){
    coilTwoState = LOW;
    digitalWrite(Relay2, LOW);
  }
  if((dataPulse == HIGH) && ((millis() - dataTimer) > 500)){
    dataPulse = LOW;
    digitalWrite(ParallelBit0, LOW);
  }
  if((commPulse == HIGH) && ((millis() - commTimer) > 50)){
    commPulse = LOW;
    digitalWrite(Error_Comm_LED, LOW);
  }
}

// This function takes a compiled string, and checks if a valid command exists within it.
// It also transmits the validity of the command back to the user.
int decodeMessage(String message){
  int returnCode = 0;
  String confirmationString = "";
  confirmationString = "Message Recieved: ";
  message.toLowerCase();

  if(strstr(message.c_str(), "coilone") != NULL)
  {
    confirmationString.concat("coilone");
    returnCode = 1;
  }
  else if(strstr(message.c_str(), "coiltwo") != NULL)
  {
    returnCode = 2;
    confirmationString.concat("coiltwo");
  }
  else if(strstr(message.c_str(), "collectdata") != NULL)
  {
    returnCode = 3;
    confirmationString.concat("collectdata");
  }
  else if(strstr(message.c_str(), "help") != NULL)
  {
    returnCode = 4;
    confirmationString.concat("help");
  }
  else if(strstr(message.c_str(), "ping") != NULL)
  {
    returnCode = 5;
    confirmationString.concat("ping");
  }
  else
  {
    returnCode = 0;
    confirmationString.concat("INVALID COMMAND");
  }

  XBee.println(confirmationString);

  Serial.print("Message Recieved: ");
  Serial.print(message);
  Serial.print(" -> Response: ");
  Serial.println(returnCode);

  return returnCode;
}
