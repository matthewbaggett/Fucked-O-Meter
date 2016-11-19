int gauge_pin = 9;           // the pin that the LED is attached to
int gauge_value;

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(gauge_pin, OUTPUT);
  Serial.begin(115200);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  gauge_value = random(0, 255);
  analogWrite(gauge_pin, gauge_value);
  Serial.print("Gauge: ");
  Serial.println(gauge_value);
  delay(1000);
}

