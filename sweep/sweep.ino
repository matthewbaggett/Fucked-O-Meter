int gauge_pin = 9;           // the pin that the gauge_pin is attached to
int gauge_value;
boolean sweep_direction = true;

void setup()  { 
  // declare pin 9 to be an output:
  pinMode(gauge_pin, OUTPUT);
  Serial.begin(115200);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  if(sweep_direction == true){
    gauge_value = gauge_value + 1;
  }else{
    gauge_value = gauge_value - 1;
  }
  
  if(gauge_value > 255){
    gauge_value = 255;
    sweep_direction = false;
  }
  if(gauge_value == 0){
    gauge_value = 0;
    sweep_direction = true;
  }
  
  analogWrite(gauge_pin, gauge_value);
  Serial.print("gauge_value: ");
  Serial.println(gauge_value);
  delay(50);
}

