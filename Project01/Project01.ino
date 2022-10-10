#define N_LEDS 2

#define BUTTR 2
#define BUTTY 3
#define LEDR 12
#define LEDY 13

int ledArray[N_LEDS];

void setup() {
  Serial.begin(9600);
  pinMode(BUTTR, INPUT);
  pinMode(BUTTY, INPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDY, OUTPUT);
  setArray();
}

void loop() {
  
  controls();
  
}

int setArray() {
  for(int i = LEDR; i < N_LEDS + LEDR; i++) {
    ledArray[i-LEDR] = i; 
  }
}

void controls() {
  if(digitalRead(BUTTR)) {
    digitalWrite(LEDR, HIGH);
  } else if(digitalRead(BUTTY)) {
    digitalWrite(LEDY, HIGH);
  } else {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDY, LOW);
  }
}
