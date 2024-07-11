/*  Code pour récuperer les données d'une centrale inertielle MPU6050
   et les enregistrer sur une carte SD
   
    Librairie utilisée : https://github.com/ElectronicCats/mpu6050
    
   De Nathan S. pour Lucie L.
*/
// On importe les librairies necessaires
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#include <SD.h>
#include <SPI.h>

// Cette variable stock le pin sur lequel est connecté la carte sd
// !!!!!!!!!!!!!!!!!  A CHANGER !!!!!!!!!!!!!!!!!
const uint8_t sdPin = 10;
const uint8_t buttonPin = 8;
const uint8_t ledPin = 9;

// Variable pour choisir la frequence d'acquisition (Hz)
uint16_t SAMPLE_RATE = 200;

//Resolution de l'IMU
uint8_t resolution = 3;

// Active ou non l'envoie de données sur le port série
bool sendSerial = false;

// Variable contenant le fichier de sortie
File fichier;

// Variable pour suivre "l'etat" du systeme, par exemple si il doit enregistrer ou non
bool recording = false;
unsigned long lastSampling = 0;

// We create the mpu object
MPU6050 mpu;

uint8_t mpu_FS_SEL, mpu_AFS_SEL;

// Variables pour stocker les differentes accélérations
int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  // Tout d'abord on démarre la connexion série
  Serial.begin(115200);
  delay(1000);
  Serial.println("Serial Start\n");

  // On initialise le pin du boutton et de la led
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Puis on démarre l'IMU
  Wire.begin();
  Serial.println("Initializing I2C devices...");
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing devices connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 numéro 1 connection successful") : F("MPU6050 numéro 1 connection failed"));

  // On met la résolution voulue puis on récupère la valeur enregistré sur la puce (on espère celle que l'on veut)
  mpu.setFullScaleGyroRange(resolution);
  mpu.setFullScaleAccelRange(resolution);
  mpu_FS_SEL = mpu.getFullScaleGyroRange();
  mpu_AFS_SEL = mpu.getFullScaleAccelRange();
  Serial.print("\nFS_SEL : ");
  Serial.println(mpu_FS_SEL);
  Serial.print("AFS_SEL : ");
  Serial.println(mpu_AFS_SEL);

  Serial.println("\nUpdating internal sensor offsets...");
  // -76  -2359 1688  0 0 0
  Serial.println("MPU 1");
  Serial.print(mpu.getXAccelOffset()); Serial.print("\t"); // -76
  Serial.print(mpu.getYAccelOffset()); Serial.print("\t"); // -2359
  Serial.print(mpu.getZAccelOffset()); Serial.print("\t"); // 1688
  Serial.print(mpu.getXGyroOffset()); Serial.print("\t"); // 0
  Serial.print(mpu.getYGyroOffset()); Serial.print("\t"); // 0
  Serial.print(mpu.getZGyroOffset()); Serial.print("\t"); // 0
  Serial.print("\n");
  mpu.setXAccelOffset(1359);
  mpu.setYAccelOffset(1061);
  mpu.setZAccelOffset(4950);
  mpu.setXGyroOffset(-351);
  mpu.setYGyroOffset(-138);
  mpu.setZGyroOffset(-31);
  Serial.print(mpu.getXAccelOffset()); Serial.print("\t"); // -76
  Serial.print(mpu.getYAccelOffset()); Serial.print("\t"); // -2359
  Serial.print(mpu.getZAccelOffset()); Serial.print("\t"); // 1688
  Serial.print(mpu.getXGyroOffset()); Serial.print("\t"); // 0
  Serial.print(mpu.getYGyroOffset()); Serial.print("\t"); // 0
  Serial.print(mpu.getZGyroOffset()); Serial.print("\t"); // 0
  Serial.print("\n");

  Serial.print("\nSAMPLE_RATE : ");
  Serial.println(SAMPLE_RATE);

  // On initialise maintenant la carte SD
  if (!SD.begin(sdPin)) {
    Serial.println("\nCard failed, or not present");
  }
  else {
    Serial.println("\nCard successfuly initialized.");
  }

  // Petit message de fin
  Serial.println("\nDone");
}

void loop() {
  // On check si le bouton a été préssé
  if (digitalRead(buttonPin) == HIGH) {
    // On attend que le bouton soit relaché
    while (digitalRead(buttonPin) == HIGH) {

    }
    delay(500);
    // Premier cas, on enregistrait déja donc on arrête l'enregistrement
    if (recording) {
      Serial.println("Stop recording");
      digitalWrite(ledPin, LOW);
      recording = false;
      fichier.close();
    }
    // Deuxième cas on lance l'enregistrement
    else {
      recording = true;

      // On trouve un nom de fichier de la forme file{n} ou n est à définir pour ne pas remplacer un fichier déja existant
      uint8_t i = 0;
      char nom_du_fichier[20];
      sprintf(nom_du_fichier, "file%03d.csv", i);
      while (SD.exists(nom_du_fichier)) {
        i++;
        sprintf(nom_du_fichier, "file%03d.csv", i);
      }
      fichier.close();
      fichier = SD.open(nom_du_fichier, FILE_WRITE);
      fichier.println("Temps;ax;ay;az");

      // On allume la led pour signaler qu'on enregistre
      digitalWrite(ledPin, HIGH);

      Serial.print("Nom du nouveau fichier: ");
      Serial.println(nom_du_fichier);
      Serial.println("Start recording");
    }
  }

  // On récupère les données

  // Pour savoir si on doit prendre une nouvelle mesure, on fait la difference du temps actuel a celui
  // de la dernire mesure et on compare ca avec la fréquence d'acquisition désirée.
  long temps = micros();
  if (temps - lastSampling >= (1.0 / SAMPLE_RATE) * 1000000.0 )
  {
    // On récupère les données brut de l'arduino
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    char delimiter = ';';
    String trame1 =  String(temps) + delimiter + String(ax) + delimiter + String(ay) + delimiter + String(az);
    String trame2 = delimiter + String(gx) + delimiter + String(gy) + delimiter + String(gz);

    // On envoie les données par le port série
    if (sendSerial) {
      Serial.print(trame1);
      Serial.println(trame2)
    }

    // On enregistre les données sur la carte SD si nécessaire
    if (recording) {
      // On verifie que le fichier de sortie est bien ouvert
      if (fichier) {
        fichier.print(trame1);
        fichier.println(trame2);
      } else {
        Serial.println("! Erreur fichier, stop recording !");
      }
    }

    // Finalement on met a jour le temps de la dernière mesure
    lastSampling = temps;
  }

}
