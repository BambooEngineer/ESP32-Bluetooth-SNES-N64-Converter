/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

#define lat 13 // white 13
#define clk 12 // blue  12                  // contains lots of class overriding ! Gotta recap C++ pointer objs + new + :: class symbol
#define dat 14 // green 14

uint8_t buts1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
String newValue="";


// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic; // pointer object
static BLEAdvertisedDevice* myDevice;




static void notifyCallback(                               // callback function
  BLERemoteCharacteristic* pBLERemoteCharacteristic,      // another characteristic obj *** i think i remember pointer obj 
  uint8_t* pData,                                         // being able to access attributes easier with -> rather than .attrb
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.write(pData, length);
    Serial.println();
}

class MyClientCallback : public BLEClientCallbacks {// class creation MyClientCallback, funcs of BLEClientCallbacks(onConnect & onDis)
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {                                          // connectToServer function creation
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());    // using BLEAdvertisedDevice* obj
    /////
    BLEClient*  pClient  = BLEDevice::createClient();             // BLEClient* obj creation & create client class func used
    /////
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());          // set ClientCallback class param ? research this code line

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    /////
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);  // BLERemoteService* obj creation from client servUUID !!!
    /////
    
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());       // disconnect if the service UUID was not found
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

/////////////////////////////////////////////////////////////////////////////////////////////////
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);// client obj ---> Remoteservice obj ----> Characteristic obj
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");       // disconnect if the characteristic UUID not found
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");
///////////////////////////////////////////////////////////////////////////////////////////////////
    if(pRemoteCharacteristic->canRead()) {                        // Read the value of the characteristic.
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }
//////////////////////////////////////////////////////////////////////////////////////////////////
    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);    // call notifyCallback function 

    connected = true;
    return true;
}



/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {    // advertisedDevice callbacks class creation
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {     // ble advertised device obj not a pointer, attribute accessed with .
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  
  pinMode(lat, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dat, INPUT);
  digitalWrite(lat, 1);
  digitalWrite(clk, 1);
  
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());    // new + advertised device callbacks class = statement
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.                              // for some reason VIN CANT BE TOUCHED WHEN UPLOADING 
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  
  if (doConnect == true) {                                                // variable
    if (connectToServer()) {                                              // bool function 
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  
  if (connected) {          // there can be an update in the future for not needing button combos but the combos make themselves 
     
    //Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    if(!buts1[0] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="b";  // 9 + 36 = 45 for n64 command + controller bytes
    if(!buts1[1] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="Cup";
    if(!buts1[8] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="a";        // b, y, sl, st, u, d, l, r, a, x, l, r
    if(!buts1[9] && buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="z";
    if(!buts1[3]) newValue ="start";
    if(!buts1[7] && buts1[4] && buts1[5] && buts1[8] && buts1[0] && buts1[1]) newValue ="right";  
    if(!buts1[6] && buts1[4] && buts1[5] && buts1[8] && buts1[0] && buts1[1]) newValue ="left";
    if(!buts1[4] && buts1[6] && buts1[7] && buts1[8] && buts1[0] && buts1[1]) newValue ="up";      // single button combinations
    if(!buts1[5] && buts1[6] && buts1[7] && buts1[8] && buts1[0] && buts1[1]) newValue ="down";

    if(!buts1[7] && !buts1[4] && buts1[8] && buts1[0] && buts1[1]) newValue ="rightUP";     // diagonal button combinations
    if(!buts1[6] && !buts1[4] && buts1[8] && buts1[0] && buts1[1]) newValue ="leftUP";
    if(!buts1[6] && !buts1[5] && buts1[8] && buts1[0] && buts1[1]) newValue ="leftDown"; 
    if(!buts1[7] && !buts1[5] && buts1[8] && buts1[0] && buts1[1]) newValue ="rightDown";
///////////////////////
    if(!buts1[8] && !buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="AUP";
    if(!buts1[8] && buts1[4] && !buts1[5] && buts1[6] && buts1[7]) newValue ="Adown";
    if(!buts1[8] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) newValue ="Aleft";
    if(!buts1[8] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) newValue ="ARight";

    if(!buts1[8] && !buts1[4] && buts1[5] && !buts1[6] && buts1[7]) newValue ="ALup";
    if(!buts1[8] && !buts1[4] && buts1[5] && buts1[6] && !buts1[7]) newValue ="ARup";
    if(!buts1[8] && buts1[4] && !buts1[5] && !buts1[6] && buts1[7]) newValue ="ALDown";
    if(!buts1[8] && buts1[4] && !buts1[5] && buts1[6] && !buts1[7]) newValue ="ARDown";
////////////////
    if(!buts1[0] && !buts1[4] && buts1[5] && buts1[6] && buts1[7]) newValue ="BUp";
    if(!buts1[0] && buts1[4] && !buts1[5] && buts1[6] && buts1[7]) newValue ="BDown";
    if(!buts1[0] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) newValue ="BLeft";
    if(!buts1[0] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) newValue ="BRight";

    if(!buts1[0] && !buts1[4] && buts1[5] && !buts1[6] && buts1[7]) newValue ="BLUp";   // have string be any of these 
    if(!buts1[0] && !buts1[4] && buts1[5] && buts1[6] && !buts1[7]) newValue ="BRUp";   // on port side write length & all combos
    if(!buts1[0] && buts1[4] && !buts1[5] && !buts1[6] && buts1[7]) newValue ="BLDown";
    if(!buts1[0] && buts1[4] && !buts1[5] && buts1[6] && !buts1[7]) newValue ="BRDown";
    ////////////////                                                                            
    if(!buts1[1] && buts1[4] && buts1[5] && !buts1[6] && buts1[7]) newValue ="CLeft";    // Cup directionals
    if(!buts1[1] && buts1[4] && buts1[5] && buts1[6] && !buts1[7]) newValue ="CRight";
    ///////
    if(buts1[0] && buts1[5] && buts1[1] && buts1[4] && buts1[6] && buts1[8] && buts1[9] && buts1[3] && buts1[7]) newValue ="None Pressed";
    read_Controller();
    // Set the characteristic's value to be the array of bytes that is actually a string.
                                                                              // have string be any of these    
                                                                              // on port side write length & all combos
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }
  
  else if(doScan){                   // else if connected
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  //delay(1000); // Delay a second between loops. // rid of delay after test
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
