/*  Code pour récuperer les données d'une centrale inertielle MPU6050
   et les envoyées par le port série
   
    Librairie utilisée : https://github.com/ElectronicCats/mpu6050
    
   Nathan S. Lucie L.
*/
// On importe les librairies necessaires
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Variable pour choisir la frequence d'acquisition (Hz)
uint16_t SAMPLE_RATE = 200;

// Resolution de l'IMU (à accorder avec la fiche technique du mpu6050)
uint8_t resolution = 3;

// Variable pour suivre "l'etat" du systeme
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

  // On met à jour les offsets de calibration (à determiner)
  Serial.println("\nUpdating internal sensor offsets...");
  // -76  -2359 1688  0 0 0
  Serial.println("MPU 1");
  mpu.setXAccelOffset(1359);
  mpu.setYAccelOffset(1061);
  mpu.setZAccelOffset(4950);
  mpu.setXGyroOffset(-351);
  mpu.setYGyroOffset(-138);
  mpu.setZGyroOffset(-31);
  Serial.print(mpu.getXAccelOffset()); Serial.print("\t");
  Serial.print(mpu.getYAccelOffset()); Serial.print("\t");
  Serial.print(mpu.getZAccelOffset()); Serial.print("\t");
  Serial.print(mpu.getXGyroOffset()); Serial.print("\t");
  Serial.print(mpu.getYGyroOffset()); Serial.print("\t");
  Serial.print(mpu.getZGyroOffset()); Serial.print("\t");
  Serial.print("\n");

  Serial.print("\nSAMPLE_RATE : ");
  Serial.println(SAMPLE_RATE);

  // Petit message de fin du setup
  Serial.println("\nDone");
}

void loop() {
  // On récupère les données

  // Pour savoir si on doit prendre une nouvelle mesure, on fait la difference du temps actuel a celui
  // de la dernire mesure et on compare ca avec la fréquence d'acquisition désirée.
  long temps = micros();
  if (temps - lastSampling >= (1.0 / SAMPLE_RATE) * 1000000.0 )
  {
    // On récupère les données brut de l'arduino
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    char delimiter = ';';
    String trame =  String(temps) + delimiter + String(ax) + delimiter + String(ay) + delimiter + String(az) + delimiter + String(gx) + delimiter + String(gy) + delimiter + String(gz);

    // On envoie les données par le port série
    Serial.println(trame);

    // Finalement on met a jour le temps de la dernière mesure
    lastSampling = temps;
  }

}
