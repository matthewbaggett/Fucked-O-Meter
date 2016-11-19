#include <SPI.h>
#include <Ethernet.h>

int led = 11;           // the pin that the LED is attached to
int brightness;
boolean sweep_direction = true;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(10,0,0,222);

// fill in your Domain Name Server address here:
IPAddress myDns(10,0,0,1);

// initialize the library instance:
EthernetClient client;

char server[] = "www.arduino.cc";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 60*1000;  // delay between updates, in milliseconds


void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  
  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
} 

// the loop routine runs over and over again forever:
void loop()  { 
  if(sweep_direction == true){
    brightness = brightness + 1;
  }else{
    brightness = brightness - 1;
  }
  
  if(brightness > 255){
    brightness = 255;
    sweep_direction = false;
  }
  if(brightness == 0){
    brightness = 0;
    sweep_direction = true;
  }
  
  analogWrite(led, brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  delay(5);
}

