/* Load this file on the walking-aid component. Every second, this pings the wearable
component and tells the wearable component if the button is pressed. It also listens
to the wearable component to find out if the alert is on. When the alert is on the
walking-aid component turns on an LED.*/


//to use nRF24L01 modules
#include <SPI.h>
#include "RF24.h" //from https://github.com/TMRh20/RF24


RF24 radio(7,8); //set up nRF24L01 radio on SPI bus plus pins 7 & 8
const int btn = 2; //pin 2
const int led = 4; //pin 4


void setup() {
    radio.begin();
    
    byte addresses[][6] = {"1Node","2Node"};
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
    
    radio.setPALevel(RF24_PA_MAX); //sets power level of transmitted pings
    
    
    Serial.begin(115200);
    
    
    pinMode(led,OUTPUT);
    
    
    radio.startListening();
}

void loop() {
    radio.stopListening(); //stop listening so we can talk
    Serial.println("Now sending");
    
    bool btnPressed = digitalRead(btn);
    if(btnPressed){
        digitalWrite(led,LOW);
    }
    
    if(!radio.write(&btnPressed, sizeof(bool))){
        Serial.println("Failed to send data");
    }
    
    
    radio.startListening();
    unsigned long started_waiting_at = micros(); //get current time
    bool timeout = false;
    
    while (!radio.available()){
        if (micros() - started_waiting_at > 200000 ){ //if waited longer than 200 ms
            timeout = true;
            break;
        }
    }
    
    
    if(timeout){
        Serial.println("Failed to receive response within 200 ms"));
    } else{
        bool alertOn = false;
        radio.read(&alertOn, sizeof(bool));
        
        if(alertOn){
            digitalWrite(led, HIGH);
        }
    }
    
    
    delay(1000); //try again 1 s later
}

