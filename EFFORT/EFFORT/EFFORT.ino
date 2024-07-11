/*  Code pour récuperer les données de jauges extensométrique
   et les envoyées par le port USB

    Librairie utilisée : https://github.com/bogde/HX711

    Avec cette librairie il y a quatre manières de récupérer les mesures:
      - scale.read()          renvoie la valeur brute lu par le capteur
      - scale.read_average(x) renvoie la valeur brute moyenné sur x mesure
      - scale.get_value(x)    renvoie la valeur brute moins la tare (moyenne sur x échantillons, 1 si non spécifié)
      - scale.get_units(x)    renvoie la valeur brute moins la tare et divisé par le gain de set_scale (même commentaire)

    Pour récupérer des valeurs calibrées, deux méthodes:
      - utiliser scale.tare() et scale.set_scale() pour appliquer une régression linéaire sur les données
      et récupérer les données avec get_units()
      - appliquer une régression aux données brutes à l'aide d'une nouvelle fonction, directement en arduino
      ou après réception des données sur python ou autre


    Il revient donc au même de faire (ne pas litéralement écrire comme ca, set_scale et set_offset ne sont à
    appeler qu'une seule fois dans setup):

      scale.set_scale(LOADCELL_DIVIDER);
      scale.set_offset(LOADCELL_OFFSET); (ou bien scale.tare())
      scale.get_units();

    et

      (scale.read() - LOADCELL_OFFSET) / LOADCELL_DIVIDER

    
    Ne surtout pas hésiter à aller lire la page de doc de la librairie !

    Nathan S.
*/
#include "HX711.h"

// Variable pour choisir la fréquence d'acquisition (Hz)
uint16_t SAMPLE_RATE = 100;

// Variable pour suivre "l'etat" du systeme
unsigned long lastSampling = 0;

// Renseigner les pins du capteur
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// On définit l'objet capteur
HX711 scale;

void setup() {
  // On initialise la connexion Serie USB
  Serial.begin(9600);
  delay(1000);
  Serial.println("Demo jauge extensométrique");


  // Puis on initialise la jauge
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(1); // avec 1 renvoie la valeur brute du capteur
  scale.tare();       // ou bien scale.set_offset(x) pour définir une valeur précise au lieu de prendre la valeur courante


  // Petit message de fin du setup
  Serial.println("Done");
}

void loop() {

  long temps = millis();

  // On vérifie que l'on a des données à lire et que l'on respecte la fréquence d'acquisition
  if (scale.is_ready() and (temps - lastSampling >= (1.0 / SAMPLE_RATE) * 1000.0 )) {
    // On récupère la valeur de la jauge
    long reading = scale.get_units();

    /* Si on voulait faire une régression manuellement:
      long reading = (scale.read() - LOADCELL_OFFSET ) / LOADCELL_DIVIDER
    */

    // Puis on l'envoie sur le port série
    char delimiter = ';';
    String trame =  String(temps) + delimiter + String(reading);
    Serial.println(trame);

    // Finalement on met a jour le temps de la dernière mesure
    lastSampling = temps;
  }
}
