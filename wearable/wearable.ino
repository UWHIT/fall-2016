/* Load this file on the wearable component. This receives pings from the 
walking-aid component and detects the distance between the two components based on the
signal strength of the ping. If three weak pings are received in a row, alert the user
with a melody, until the button on the walking-aid component is pressed.*/


//to use nRF24L01 modules

#include <SPI.h>
#include "RF24.h" //from https://github.com/TMRh20/RF24

//for playing alert melody
#include "pitches.h"


RF24 radio(7,8); //set up nRF24L01 radio on SPI bus plus pins 7 & 8
const int speaker = 2; //pin 2
const int led = 4; //pin 4


//variables used for alert triggering and alert melody playing

int numWeaksInARow = 0; //number of received pings with weak signals
const int numWeaksInARowToTriggerAlert = 3;

bool alertOn = false;

//alert melody
int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

//note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4
};

const int numberOfNotes = 8;


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
        
        if(alertOn){
            alertUser();
        }
        

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

// Plays alert melody.
void alertUser(){
    for (int thisNote = 0; thisNote < numberOfNotes; thisNote++) {
        int noteDuration = 1000 / noteDurations[thisNote]; //ms
        int pauseBetweenNotes = noteDuration * 1.30; //ms
        
        tone(speaker, melody[thisNote], noteDuration);
        digitalWrite(led,HIGH);
        delay(pauseBetweenNotes);
        noTone(speaker);
        digitalWrite(led,LOW);
    }
}
