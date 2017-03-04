/* Load this file on the wearable component. This receives pings from the 
walking-aid component and detects the distance between the two components based on the
signal strength of the ping. If three weak pings are received in a row, alert the user
with a melody, until the button on the walking-aid component is pressed.*/


//to use nRF24L01 modules
#include <SPI.h>
#include "RF24.h" //from https://github.com/TMRh20/RF24


RF24 radio(7,8); //set up nRF24L01 radio on SPI bus plus pins 7 & 8
const int led = 4; //pin 4


//variables used for alert triggering and alert melody playing

int numWeaksInARow = 0; //number of received pings with weak signals
const int numWeaksInARowToTriggerAlert = 3;

bool alertOn = false;


void setup() {
    radio.begin();
    
    byte addresses[][6] = {"1Node","2Node"};
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    
    radio.setPALevel(RF24_PA_MIN); //sets power level of transmitted pings
    
    
    Serial.begin(115200); //for debugging
    
    
    pinMode(led,OUTPUT);
    
    
    radio.startListening();
}

void loop() {
    bool buttonPressed = false;
    
    if(radio.available()){
        bool strongSignal = radio.testRPD();
        Serial.println(strongSignal);
        
        if(strongSignal){
            Serial.println("Received ping has strong signal strength");
            numWeaksInARow = 0;
        } else{
            Serial.println("Received ping has weak signal strength");
            numWeaksInARow++;
            if(numWeaksInARow >= numWeaksInARowToTriggerAlert){
                alertOn = true; //turn on alert
            }
        }
        
        digitalWrite(led, alertOn);
        

        while (radio.available()) {
            radio.read(&buttonPressed, sizeof(bool));
        }
        
        if(buttonPressed){
            alertOn = false; //turn off alert
        }
        
        radio.stopListening(); //stop listening so we can talk
        radio.write(&alertOn, sizeof(bool));
        radio.startListening(); //resume listening to catch next pings
    }
}
