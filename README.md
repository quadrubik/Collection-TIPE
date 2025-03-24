Ce repo contient différents codes ayant été utilisés dans des TIPES en 2024.

Les codes sont les suivants :
- Code Arduino pour la réception des données d'un accéléromètre et enregistrement sur carte SD
- Même chose en envoi direct par le port série des données
- Code Python pour l'analyse de données d'un accéléromètre enregistrées au format csv

- Code Arduino et Python pour la réception et l'enregistrement dans un fichier des données reçues par une jauges d'extensométrie

- Code Arduino pour la lecture d'un capteur de débit et le contrôle d'une vanne


Les codes MPU6050_brut et EFFORT fonctionnent avec leur code python, je les ai testé.

J'ai essayé de commenter le plus possible les codes sans pour autant les rendre illisibles. 

Les codes pythons sont au format notebook jupyter ce qui permet des codes en blocs plus lisibles.

Dans tous les cas, ces codes ne sont pas l'approche la plus directe pour faire fonctionner les capteurs mais ils permettent tous une intégration avec python relativement simple.

Autant que possible j'ai aussi essayé de garder la même logique entre les différents codes, la réception arduino entre EFFORT et IMU ainsi que la structure du code entre MPU6050_brut et EFFORT sont essentiellement respectivement les mêmes.
Pour ce qui est du code du capteur de débit et de la vanne, j'ai le code arduino mais pas le python, ce dernier était trop appliqué et consistait en grande partie à du calcul plus qu'à de la communication (et un asservissement sans doute trop avancé pour juste débuter en arduino).
