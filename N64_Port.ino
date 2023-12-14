/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/             
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/                             

#include <Arduino.h>
#include <BLEDevice.h>              // Put all this shit in a hex file
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define DATA_SIZE 4     // 26 bytes is a lower than RX FIFO size (127 bytes) 
#define BAUD 1000000    // Any baudrate from 300 to 115200
#define TEST_UART 1     // Serial1 will be used for the loopback testing with different RX FIFO FULL values
#define RXPIN 4         // GPIO 4 => RX for Serial1
#define TXPIN 5         // GPIO 5 => TX for Serial1

uint8_t buts1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
// volatile declaration will avoid any compiler optimization when reading variable values
volatile size_t sent_bytes = 0, received_bytes = 0;

byte bits[4] = {55, 55, 55, 55};
byte hex_five[5] = {4, 4, 52, 52, 52};
byte hex_zero[5] = {4, 4, 4, 4, 52};
byte hex_three[4] = {4, 4, 52, 55};
byte all_together_now[13] = {4, 4, 52, 52, 4, 4, 4, 4, 4, 4, 4, 52, 63}; // STATUS BITS

byte start[18] = {4, 52, 4, 4, 
                         4, 4, 4, 4, 4,     // first, 2nd, 3rd & last bit must be low
                         4, 4, 4, 4,        
                         4, 4, 4, 4, 4};    // first & last bit must be low

byte button_bytes[18] = {4, 4, 4, 4, 
                  4, 4, 4, 4, 4,     
                  4, 4, 4, 4,        
                  4, 4, 4, 4, 4};
                  
byte a[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 4,                              // PUT ALL THIS SHIT IN A H FILE
              4, 4, 4, 4,        
              4, 4, 4, 4, 4};
              
byte b[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 4,        
              4, 4, 4, 4, 4};
              
byte z[18] = {4, 7, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 4,        
              4, 4, 4, 4, 4};

byte Cup[18] = {4, 4, 4, 4, 
              4, 4, 7, 4, 4,     
              4, 4, 4, 4,        
              4, 4, 4, 4, 4};
              
byte Right[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 4,    // x bits    
              4, 4, 4, 4, 4};
              
byte Left[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 4,    
              4, 4, 4, 4, 4};
              
byte Down[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 7,        
              4, 4, 4, 4, 4}; 
              
byte Up[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};

byte RUp[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 52,      
              55, 55, 55, 4, 4};
byte LUp[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};
byte RDown[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 7,        
              4, 4, 4, 4, 4};
byte LDown[18] = {4, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 7,        
              4, 4, 4, 4, 4};
/////////////////////////////////
byte ARight[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 4,    // x bits    
              4, 4, 4, 4, 4};
              
byte ALeft[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 4,    
              4, 4, 4, 4, 4};
              
byte ADown[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 7,        
              4, 4, 4, 4, 4}; 
              
byte AUp[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};

byte ARUp[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 52,      
              55, 55, 55, 4, 4};
byte ALUp[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};
byte ARDown[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 7,        
              4, 4, 4, 4, 4};
byte ALDown[18] = {7, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 7,        
              4, 4, 4, 4, 4};
/////////////////////////////
byte BRight[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 4,    // x bits    
              4, 4, 4, 4, 4};
              
byte BLeft[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 4,    
              4, 4, 4, 4, 4};
              
byte BDown[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 7,        
              4, 4, 4, 4, 4}; 
              
byte BUp[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 4,     
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};

byte BRUp[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 52,      
              55, 55, 55, 4, 4};
byte BLUp[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 7,                         
              4, 4, 4, 52,      
              55, 55, 55, 4, 4};
byte BRDown[18] = {52, 4, 4, 4, 
              4, 4, 4, 4, 52,      
              55, 55, 55, 7,        
              4, 4, 4, 4, 4};
byte BLDown[18] = {52, 4, 4, 4,     
              4, 4, 4, 4, 7,                         
              4, 4, 4, 7,        
              4, 4, 4, 4, 4};
////////////////////////////
byte CRight[18] = {4, 4, 4, 4, 
              4, 4, 7, 4, 52,      
              55, 55, 55, 4,    // x bits    
              4, 4, 4, 4, 4};
byte CLeft[18] = {4, 4, 4, 4, 
              4, 4, 7, 4, 7,                         
              4, 4, 4, 4,    
              4, 4, 4, 4, 4};

volatile bool go = false;
bool STOP = true;
bool polled = false;
volatile int inc = 0;

std::string rxValue = "";

void ARDUINO_ISR_ATTR isr() { 
    ++inc;                      // starting at 0
    go = true;
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
     
      rxValue = pCharacteristic->getValue();      // rxValue declaration

      //if (rxValue.length() > 0){           // if something recieved
        /*Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");*/
     // }
  }
};


void setup() {
 //Serial.begin(115200)
  pinMode(9, INPUT);                          
  attachInterrupt(9, isr, FALLING);
 // Serial.println("Starting BLE work!");
  Serial1.begin(BAUD, SERIAL_6N1, RXPIN, TXPIN);
  

  BLEDevice::init("N64 Port");
  
  BLEServer *pServer = BLEDevice::createServer();
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(      // characteristic
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("n64 port");
  pCharacteristic->setCallbacks(new MyCallbacks());     // rx callback function
  
  pService->start();
  
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  
  pAdvertising->addServiceUUID(SERVICE_UUID);
  
  pAdvertising->setScanResponse(true);
  
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  
  pAdvertising->setMinPreferred(0x12);
  
  BLEDevice::startAdvertising();

  
  
}

void loop() {   //                  THE INTERRUPT CODE RUNS 50uS AFTER FALL ( code at this rate )
  if(go && STOP){     
          Serial1.write(all_together_now, 13); // figure out how to not retrigger interrupt, thats why we get repeats
          go = false;
          STOP = false;     // 25 + 9 = 34 ( add an extra for when it falls for a poll request )
  }
  if (rxValue.length() > 0){
    if((inc >= 35 && !polled && !STOP) || (inc >= 9 && polled)){         // skip # of edges after serial write + the next n64 poll
            if(rxValue == "b") Serial1.write(b, 18);  // 9 + 36 = 45 for n64 command + controller bytes
            if(rxValue == "Cup") Serial1.write(Cup, 18);
            if(rxValue == "a") Serial1.write(a, 18);         // b, y, sl, st, u, d, l, r, a, x, l, r
            if(rxValue == "z") Serial1.write(z, 18);
            if(rxValue == "start") Serial1.write(start, 18);
            if(rxValue == "right") Serial1.write(Right, 18);    
            if(rxValue == "left") Serial1.write(Left, 18);
            if(rxValue == "up") Serial1.write(Up, 18);      // single button combinations
            if(rxValue == "down") Serial1.write(Down, 18);
        
            if(rxValue == "rightUP") Serial1.write(RUp, 18);     // diagonal button combinations
            if(rxValue == "leftUP") Serial1.write(LUp, 18); 
            if(rxValue == "leftDown") Serial1.write(LDown, 18);  
            if(rxValue == "rightDown") Serial1.write(RDown, 18);
        ///////////////////////
            if(rxValue == "AUP") Serial1.write(AUp, 18);
            if(rxValue == "Adown") Serial1.write(ADown, 18);
            if(rxValue == "Aleft") Serial1.write(ALeft, 18); // all A directionals
            if(rxValue == "ARight") Serial1.write(ARight, 18);
        
            if(rxValue == "ALup") Serial1.write(ALUp, 18);
            if(rxValue == "ARup") Serial1.write(ARUp, 18);
            if(rxValue == "ALDown") Serial1.write(ALDown, 18);
            if(rxValue == "ARDown") Serial1.write(ARDown, 18);
        ////////////////
            if(rxValue == "BUp") Serial1.write(BUp, 18);  // all B directionals
            if(rxValue == "BDown") Serial1.write(BDown, 18);
            if(rxValue == "BLeft") Serial1.write(BLeft, 18);
            if(rxValue == "BRight") Serial1.write(BRight, 18);
        
            if(rxValue == "BLUp") Serial1.write(BLUp, 18);
            if(rxValue == "BRUp") Serial1.write(BRUp, 18);
            if(rxValue == "BLDown") Serial1.write(BLDown, 18);
            if(rxValue == "BRDown") Serial1.write(BRDown, 18);
            ////////////////                                                                            
            if(rxValue == "CLeft") Serial1.write(CLeft, 18);    // Cup directionals
            if(rxValue == "CRight") Serial1.write(CRight, 18);
            ///////
            if(rxValue == "None Pressed") Serial1.write(button_bytes, 18);
            //Serial1.write(button_bytes0, 18);                 // first poll is 35, second is 46,        // PROBLEM SOLVED : Program was not waiting for serial transmission to finish, miscounting falling edges, fucked up poll response timing  
            Serial1.flush();         
           // read_Controller(); 
            //start = true;                            
            inc = 0;
            polled = true;              // THE PROBLEM : putting read controller in there fucks shit up( commented out original lines )
          }
   }
    else{     // if nothing recieved
       if((inc >= 35 && !polled && !STOP) || (inc >= 9 && polled)){
          Serial1.write(button_bytes, 18);
          Serial1.flush();         
          inc = 0;
          polled = true;         
       }
   }
  
 //if(Serial1.available() == 4) digitalWrite(3, HIGH);
 // skip 2 triggers after status byte send
 // skip 3 triggers after controller bytes send
}
