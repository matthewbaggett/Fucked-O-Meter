#include <SPI.h>
#include <Ethernet.h>

int gauge_pin = 9;           // the pin that the gauge_pin is attached to
int gauge_value;
boolean sweep_direction = true;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// initialize the library instance:
EthernetClient client;

char server[] = "pharmer.thru.io";
char server_ip[] = "138.68.175.210";

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 2000;  // delay between updates, in milliseconds

String response;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(gauge_pin, OUTPUT);
  Serial.begin(115200);
  
  
  // give the ethernet module time to boot up:
  delay(1000);
  Serial.println("Starting up...");

  // start the Ethernet connection using a fixed IP address and DNS server:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failure to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
} 


String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory
  while(true){

    if (client.available()) {
      char c = client.read();

      if (c == '{ ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
      }else if(startRead){

        if(c != '}'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //got what we need here! We can disconnect now
          startRead = false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");
          Serial.println(inString);
          return inString;
        }
      }
    }
  }
}


// this method makes a HTTP connection to the server:
String httpRequest() {
  // if there's a successful connection:
  Serial.print("Connecting to ");
  Serial.println(server_ip);
  if (client.connect(server_ip, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("GET /v1/status HTTP/1.1");
    client.print("Host: "); client.println(server);
    client.println("User-Agent: fucked-o-meter");
    client.println("Connection: close");
    client.println();

    lastConnectionTime = millis();
    return readPage();
  } else {
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
    return "";
  }
}


// the loop routine runs over and over again forever:
void loop()  { 
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the cliserverent:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    lastConnectionTime = millis();
    Serial.println("HTTP Response:");
    Serial.println(httpRequest());
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
  
  delay(333);
}


