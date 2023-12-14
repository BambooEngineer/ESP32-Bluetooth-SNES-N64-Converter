/*
 * What is 8N1 serial?
8-N-1 is a common shorthand notation for a serial port parameter 
setting or configuration in asynchronous mode, in which there is one start bit, 
eight (8) data bits, no (N) parity bit, and one (1) stop bit. As such, 
8-N-1 is the most common configuration for PC serial communications today.
 */

#include <Arduino.h>

#define lat 6
#define clk 7
#define dat 8

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
              
                                                   
// WORKS : the last 1 bit on the right blends with the High stop bit( hard to notice )
volatile bool go = false;
bool STOP = true;
bool polled = false;
volatile int inc = 0;

//bool start = false;

                // Serial1.write() outputs to LSB-MSB order
// n64 pair 01 : 52 WORKS, its just that the 1 bit after the 0 bit blends after the high uart stop bit 
// n64 pair 10 : 7
// n64 pair 00 : 4
// n64 pair 11 : 55 THEY ALL WORK, tested good when doing combinations


// previously :   and go = true was in the isr              FIRST WORKING N64 SERIAL CODE
/*if(go && STOP){     
    Serial1.write(all_together_now, 13); // figure out how to not retrigger interrupt, thats why we get repeats
    go = false;
    STOP = false;
}*/

void ARDUINO_ISR_ATTR isr() { 
    ++inc;                      // starting at 0
    go = true;
}


void setup() {
  // UART0 will be used to log information into Serial Monitor
  pinMode(3, OUTPUT);
  pinMode(lat, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dat, INPUT);
  digitalWrite(lat, 1);
  digitalWrite(clk, 1);
  
  pinMode(9, INPUT);                          
  attachInterrupt(9, isr, FALLING);
 // attachInterrupt(6, isr0, FALLING);

  Serial1.begin(BAUD, SERIAL_6N1, RXPIN, TXPIN); // Rx = 4, Tx = 5 will work for ESP32, S2, S3 and C3

  // Binary : 1 = 1 uS after the falling start bit, 4uS LOW after
  // DOING 21 showed that Serial1.write() outputs to LSB-MSB order
  
}

void loop() {   //                  THE INTERRUPT CODE RUNS 50uS AFTER FALL ( code at this rate )
          
  if(go && STOP){     
    Serial1.write(all_together_now, 13); // figure out how to not retrigger interrupt, thats why we get repeats
    go = false;
    STOP = false;     // 25 + 9 = 34 ( add an extra for when it falls for a poll request )
  }
  if((inc >= 35 && !polled && !STOP) || (inc >= 9 && polled)){         // skip # of edges after serial write + the next n64 poll
    if(!buts1[0] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(b, 18);  // 9 + 36 = 45 for n64 command + controller bytes
    if(!buts1[1] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(Cup, 18);
    if(!buts1[8] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(a, 18);         // b, y, sl, st, u, d, l, r, a, x, l, r
    if(!buts1[9] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(z, 18);
    if(!buts1[3]) Serial1.write(start, 18);
    if(!buts1[7] && buts1[4] && buts1[5] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(Right, 18);    
    if(!buts1[6] && buts1[4] && buts1[5] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(Left, 18);
    if(!buts1[4] && buts1[6] && buts1[7] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(Up, 18);      // single button combinations
    if(!buts1[5] && buts1[6] && buts1[7] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(Down, 18);

    if(!buts1[7] && !buts1[4] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(RUp, 18);     // diagonal button combinations
    if(!buts1[6] && !buts1[4] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(LUp, 18); 
    if(!buts1[6] && !buts1[5] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(LDown, 18);  
    if(!buts1[7] && !buts1[5] && buts1[8] && buts1[0] && buts1[1]) Serial1.write(RDown, 18);
///////////////////////
    if(!buts1[8] && !buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(AUp, 18);
    if(!buts1[8] && buts1[4] && !buts1[5] && buts1[6] && buts1[7]) Serial1.write(ADown, 18);
    if(!buts1[8] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) Serial1.write(ALeft, 18); // all A directionals
    if(!buts1[8] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) Serial1.write(ARight, 18);

    if(!buts1[8] && !buts1[4] && buts1[5] && !buts1[6] && buts1[7]) Serial1.write(ALUp, 18);
    if(!buts1[8] && !buts1[4] && buts1[5] && buts1[6] && !buts1[7]) Serial1.write(ARUp, 18);
    if(!buts1[8] && buts1[4] && !buts1[5] && !buts1[6] && buts1[7]) Serial1.write(ALDown, 18);
    if(!buts1[8] && buts1[4] && !buts1[5] && buts1[6] && !buts1[7]) Serial1.write(ARDown, 18);
////////////////
    if(!buts1[0] && !buts1[4] && buts1[5] && buts1[6] && buts1[7]) Serial1.write(BUp, 18);  // all B directionals
    if(!buts1[0] && buts1[4] && !buts1[5] && buts1[6] && buts1[7]) Serial1.write(BDown, 18);
    if(!buts1[0] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) Serial1.write(BLeft, 18);
    if(!buts1[0] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) Serial1.write(BRight, 18);

    if(!buts1[0] && !buts1[4] && buts1[5] && !buts1[6] && buts1[7]) Serial1.write(BLUp, 18);
    if(!buts1[0] && !buts1[4] && buts1[5] && buts1[6] && !buts1[7]) Serial1.write(BRUp, 18);
    if(!buts1[0] && buts1[4] && !buts1[5] && !buts1[6] && buts1[7]) Serial1.write(BLDown, 18);
    if(!buts1[0] && buts1[4] && !buts1[5] && buts1[6] && !buts1[7]) Serial1.write(BRDown, 18);
    ////////////////                                                                            
    if(!buts1[1] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) Serial1.write(CLeft, 18);    // Cup directionals
    if(!buts1[1] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) Serial1.write(CRight, 18);
    ///////
    if(buts1[0] && buts1[5] && buts1[1] && buts1[4] && buts1[6] && buts1[8] && buts1[9] && buts1[3] && buts1[7]) Serial1.write(button_bytes, 18);
    //Serial1.write(button_bytes0, 18);                 // first poll is 35, second is 46,        // PROBLEM SOLVED : Program was not waiting for serial transmission to finish, miscounting falling edges, fucked up poll response timing  
    Serial1.flush();         
    read_Controller(); 
    //start = true;                            
    inc = 0;
    polled = true;              // THE PROBLEM : putting read controller in there fucks shit up( commented out original lines )
  }                             // works with just serial write line above read controlelr without if statements and read controller
  
 //if(Serial1.available() == 4) digitalWrite(3, HIGH);
 // skip 2 triggers after status byte send
 // skip 3 triggers after controller bytes send
}

void read_Controller(){               // waiting for download bug is when pin 9 is held low on startup !
   
  digitalWrite(lat, 1); 
  delayMicroseconds(4);
  digitalWrite(clk, 1);
  delayMicroseconds(4);
  digitalWrite(lat, 0);
  delayMicroseconds(4);
  for(int i = 0;i<16;++i){
    digitalWrite(clk, 0);
    delayMicroseconds(4);
    if(!digitalRead(dat)){
      buts1[i] = 0;
      //if(i == 0) Serial.println("B Pressed");       // shit works, test code
     // if(i == 11) Serial.println("R Pressed");
    }
    else{
      buts1[i] = 1;
    }
    digitalWrite(clk, 1);
    delayMicroseconds(4);
  }
  digitalWrite(lat, 1);
  delayMicroseconds(4);
  
}




 /*
  * Took a similar method someone used on a PIC32
--------
Got it to work on the ESP32 But a big problem was the esp32 lag,
Figured out how to work around the ESP32 lag which is fixable,
but right now we dont have time to fix so instead we are working
around.
--------
The n64 console cuts some slack, N64 controllers are much Faster
--------

      N64 controller polls every 30mS
  */

  
