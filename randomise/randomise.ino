int led = 11;           // the pin that the LED is attached to
int brightness;

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
  Serial.begin(115200);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  brightness = random(0, 255);
  analogWrite(led, brightness);
  Serial.print("Brightness: ");
  Serial.println(brightness);
  delay(1000);
}

