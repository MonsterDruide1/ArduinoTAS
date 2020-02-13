void setup() 
  {
  Serial.begin(9600);
  DDRD &= ~(1 << PB3);
  }

int counter = 0;

void loop() 
  {
  byte X;

  // Eingangspin 8 einlesen und ausgeben
  X = (PIND & (1 << PB3))/8;
  if(X != 0){
    counter++;
  }

  if(counter==60){
    counter=0;
    Serial.println("ONE");
  }
  }
