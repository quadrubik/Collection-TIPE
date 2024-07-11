/*
   Code pour la lecture d'un capteur de débit et l'activation d'une vanne piloté par un relay
   Brancher le capteur de débit sur le pin 3 de l'arduino pour utiliser l'interruption associée au pin
*/

// Variable de l'application:
double flow;
const int RELAY_PIN = 5;  // sortie à connecter au relay
unsigned long lastTime;   // variable pour gérer le temps entre deux calculs du débit
unsigned pulse_count;     // variable pour compter le nombre d'impulsions recues entre deux calculs

// Fonction associée à l'interruption du capteur de débit
void pulse() {
  // Lorsque l'on recoit une impulsion, on incrémente la variable pulse_count
  pulse_count++;
}

void setup() {
  // On initialise les E/S
  pinMode(3, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // On démarre le port série
  Serial.begin(3600);

  // On assigne l'interruption
  attachInterrupt(digitalPinToInterrupt(3), pulse, RISING);

  lastTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  // Chaque seconde, calcule et écrit le débit
  if (currentTime - lastTime >= 1000) {
    lastTime = currentTime;

    flow = (pulse_count / 0.75);

    pulse_count = 0;

    Serial.print(currentTime);
    Serial.print("  ");
    Serial.println(flow, DEC);
  }

  // Si nécessaire on recoit une commande pour la vanne
  while (Serial.available()) {
    delay(3); // assure que l'on recoit tout le packet de donné
    
    char data[4];
    Serial.readBytes(data, 2);

    // Si la commande est '1' on ouvre la vanne sinon on la ferme
    if (data[0] == '1') {
      digitalWrite(RELAY_PIN, HIGH);
    }
    else {
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}
