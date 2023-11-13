#include "Communication.h"
#include <chrono>
#include <thread>

// Constructeur
Communication::Communication(QObject *parent) : QObject(parent)
{
	// Spécifie le nom du port série (vérifie le nom du port Arduino sur ton ordinateur)
	portSerie.setPortName("COM4"); // Remplace port "COMX" par le nom de ton port série

	// Configure le débit en bauds, les bits de données, la parité, le nombre de bits d'arrêt, etc.
	portSerie.setBaudRate(QSerialPort::Baud9600);
	portSerie.setDataBits(QSerialPort::Data8);
	portSerie.setParity(QSerialPort::NoParity);
	portSerie.setStopBits(QSerialPort::OneStop);
	portSerie.setFlowControl(QSerialPort::NoFlowControl);

	connect(&portSerie, SIGNAL(readyRead()), this, SLOT(onReadyRead())); // Relie signal au slot lorsque readyRead est émit

	// [Connexion BDD]
	database = QSqlDatabase::addDatabase("QMYSQL");

	database.setHostName("192.168.64.213");
	database.setUserName("root");
	database.setPassword("root");
	database.setDatabaseName("Lawrence");

	if (database.open())
	{
		isOpen = 1;
		qDebug() << "Connexion reussie a " + QString::fromStdString(database.hostName().toStdString());
	}
	else
	{
		qDebug() <<  "Probleme de connexion a " + QString::fromStdString(database.hostName().toStdString());
	}

	// Ouverture du port série
	if (portSerie.open(QIODevice::ReadOnly)) {
		qDebug() << "Port serie ouvert.";
	}
	else {
		qDebug() << "Impossible d ouvrir le port serie.";
	}
}

// Destructeur
Communication::~Communication()
{
	portSerie.close(); // On ferme le port série
	database.close(); // On ferme la BDD
}

void Communication::onReadyRead()
{
	QByteArray newData = portSerie.readAll(); // On prend tout les données dispo
	nmeaDataBuffer += QString(newData); // On additionne les données reçues

	while (nmeaDataBuffer.contains("$GPGGA")) 
	{
		int start = nmeaDataBuffer.indexOf("$GPGGA"); // Recherche le début d'une trame
		nmeaDataBuffer = nmeaDataBuffer.mid(start); // Éliminer tout avant la trame

		int end = nmeaDataBuffer.indexOf("\r\n"); // Rechercher un séparateur de ligne (fin d'une trame)
		if (end != -1)  // Si c'est pas la fin
		{ 
			QString nmeaString = nmeaDataBuffer.left(end); // On va prendre tout le reste de la trane à  part de end donneés
			nmeaDataBuffer = nmeaDataBuffer.mid(end + 2); // Éliminer la trame traitée et le séparateur de ligne (\r\n)

			// Maintenant, nmeaString contient une trame GPS complète
			QStringList fields = nmeaString.split(','); // Séparer toutes les infos avec une virgule

			// La taille de tout les données qui nous intéressent
			if (fields.size() >= 15) {
				QString time = fields[1]; // Heure (HHMMSS)

				QString latitude = fields[2]; // Latitude (DDMM.MMMM)
				QString latitudeDirection = fields[3]; // Direction de la latitude (N/S)

				QString longitude = fields[4]; // Longitude (DDDMM.MMMM)
				QString longitudeDirection = fields[5]; // Direction de la longitude (E/W)

				// Ensuite on les affiches dans la console
				qDebug() << "longitude : " << longitude << longitudeDirection;
				qDebug() << "latitude : " << latitude << latitudeDirection;
				qDebug() << "heure : " << time << endl;

				if (isOpen == 1) // Si la bdd est ouverte
				{
					QDate currentDate = QDate::currentDate();
					QString formattedDate = currentDate.toString("yyyy-MM-dd");

					QSqlQuery query;
					query.prepare("INSERT INTO GPS (Date, Heure, Latitude, Longitude) VALUES (:dateValue, :heureValue, :latitudeValue, :longitudeValue)");
					query.bindValue(":dateValue", formattedDate);
					query.bindValue(":heureValue", time);
					query.bindValue(":latitudeValue", latitude + latitudeDirection);
					query.bindValue(":longitudeValue", longitude + longitudeDirection);

					if (query.exec()) 
					{
						qDebug() << "Donnees inserees avec succes !";

						std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //pause 3s
					}
					else 
					{
						qDebug() << "Echec de l'insertion : " << query.lastError().text();
					}
				}
				else
				{
					qDebug() << "BDD PAS OUVERTE";
 				}
			}
		}
		else 
		{
			// La trame n'est pas encore complète, attendez plus de données.
			break;
		}
	}

}