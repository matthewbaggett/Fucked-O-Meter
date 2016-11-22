#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

int gauge_pin = 9;           // the pin that the gauge_pin is attached to
boolean sweep_direction = true;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// initialize the library instance:
EthernetClient client;

const char server[] = "pharmer.thru.io";
const char endpoint[] = "/v1/status";
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 5000;    // delay between updates, in milliseconds

const int inStringLen = 350;
char inString[inStringLen]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?

double previousPercentage = 0;

void hcf(){
  Serial.println("Halt and Catch Fire ordered.");
  for(;;)
      ;
}

void setGauge(double percent){
  double gauge_value = 2.55 * percent;
  Serial.print(F(" > Setting gauge to ")); Serial.print(percent); Serial.print(F("% ("));Serial.print(gauge_value);Serial.println(F(")"));
  analogWrite(gauge_pin, gauge_value);
}

void testGauge(){
  Serial.println(F("Testing Gauge"));
  for(int i = 1; i <= 2; i++){
    setGauge(100);
    delay(333);
    setGauge(0);
    delay(333);
  }
  Serial.println(F(" > [DONE]"));
}

void payAttentionToMe(){
  for(int i = 1; i <= 5; i++){
    setGauge(100);
    delay(200);
    setGauge(0);
    delay(200);
  }
}

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(gauge_pin, OUTPUT);
  Serial.begin(115200);
  
  // give the ethernet module time to boot up:
  delay(1000);
  Serial.println(F("Starting up..."));

  // start the Ethernet connection using a fixed IP address and DNS server:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failure to configure Ethernet using DHCP"));
    // no point in carrying on, so do nothing forevermore:
    hcf();
  }
  
  // print the Ethernet board/shield's IP address:
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());

  testGauge();
} 

String readPage() {
  //read the page, and capture & return everything between '<' and '>'
  Serial.println(F(" > Waiting for HTTP RESPONSE"));
  stringPos = 0;
  memset( &inString, 0, inStringLen ); //clear inString memory
  Serial.print(F(" > Waiting for Client to become available."));
  while(!client.available()){
    Serial.print(".");
    delay(10);
  }
  Serial.println();
  while(client.available() && client.connected()) {
    char c = client.read();
    inString[stringPos] = c;
    stringPos ++;
  }
  client.stop();
  client.flush();
  Serial.println(F(" > Connection Complete."));
  return inString;
}

// this method makes a HTTP connection to the server:
String httpRequest() {
  // if there's a successful connection:
  Serial.print(F("Connecting to "));
  Serial.println(server);
  if (client.connect(server, 80)) {
    Serial.println(F("connecting..."));
    // send the HTTP PUT request:
    client.print(F("GET "));client.print(endpoint); client.println(F(" HTTP/1.1"));
    client.print(F("Host: ")); client.println(server);
    client.println(F("User-Agent: fucked-o-meter"));
    client.println(F("Connection: close"));
    client.println();
    Serial.println(F(" > Sent HTTP Request"));

    lastConnectionTime = millis();
    return readPage();
  } else {
    Serial.println(F("connection failed"));
    Serial.println(F("disconnecting."));
    client.stop();
    return "";
  }
}

// the loop routine runs over and over again forever:
void loop()  { 
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    lastConnectionTime = millis();
    Serial.print(F("\n\n"));
    String httpResponse = httpRequest();
    Serial.print(F("HTTP Response ("));Serial.print(httpResponse.length());Serial.println(F(" Bytes)"));
    Serial.println(httpResponse);
    Serial.print(F("\n\n"));
    int indexOfBodyBreak = httpResponse.indexOf("\r\n\r\n") + 4;
    int bytesAfterBodyBreak = httpResponse.length() - indexOfBodyBreak;
    Serial.print(F("Index of body break: ")); Serial.println(indexOfBodyBreak);
    Serial.print(F("Bytes after body break: ")); Serial.println(bytesAfterBodyBreak);
   
    String httpResponseBody = httpResponse.substring(indexOfBodyBreak);
    Serial.print(F("HTTP Response Body ("));Serial.print(httpResponseBody.length());Serial.println(F(" Bytes)"));
    Serial.println(httpResponseBody);
    Serial.println(F("End."));

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(httpResponseBody);
    double percentageStatus = root["PercentageStatus"];
    Serial.print(F("Percentage Status: ")); Serial.print(percentageStatus); Serial.println(F("%"));
    if(percentageStatus != previousPercentage){
      payAttentionToMe();
      setGauge(percentageStatus);
      previousPercentage = percentageStatus;
    }
    
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}


