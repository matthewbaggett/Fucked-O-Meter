int led = 11;           // the pin that the LED is attached to
int brightness;
boolean sweep_direction = true;

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
  Serial.begin(115200);
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

