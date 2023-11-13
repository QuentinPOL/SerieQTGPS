# SerieQTGPS
Server C++ en  lien avec l'application web pour insérer les données GPS en  bdd

INFO : IL Y AUSSI UN AUTRE REPOSITORY QUI CONTIENT LE SITE WEB.
VOICI LE LIEN DU REPOSITORY : https://github.com/HURTELJoris/El-projet-gps

## 1) LE SERVEUR EST CONNECTER SUR LE PORT COM3 ET IL DECOUPE LES TRAMES GPS DE TYPE GPGGA ET LES INSERTS EN  BASE DE DONNEE

-----------------


## 2) LA BASE DE DONNÉES

il est possible d'accéder à la base de données en utilisant le couple identifiant/mot de passe : root/root.
Voici la composition de la base de données :


Lawrence     	
      
      └── user  
        ├── idUser : int (clé primaire)  
        ├── nom : varchar(30)  
        ├── email : varchar(300)  
        └── passwd : varchar (30)  
        └── isAdmin : tinyint (1) 

        
      └── GPS  
        ├── BateauID : int (clé primaire)  
        ├── Date : date 
        ├── Heure : time
        ├── Latitude : varchar (100) 
        ├── Longitude : varchar (100) 
        └── Vitesse : int
        └── VitesseMoyenne : float

-----------------

## 3° ORGANISATION DU CODE

* ./SimulateurGPS_Serie_Via_Arduino 
    Contient un fichier avec le code arduino qui va permettre de simuler les trames  GPGGA sur le port COMX DU PC

* ./C++
  RT.sln, fichier de la solution du projet qui  permet son fonctionnement

* ./hg
    *Communication.cpp* -> fichier de déclaration de la classe communication
    *Communication.h* -> fichier d'entête de la classe communication
    *main.cpp* -> fichier principal
    *hg.vcxproj.filters* -> fichier de configuration de la solution
    *hg.vcxproj* -> fichier de configuration de la solution
    *hg.vcxproj.user -> fichier de configuration de la solution
