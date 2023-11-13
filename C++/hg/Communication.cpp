#include "Communication.h"
#include <chrono>
#include <thread>

// Constructeur
Communication::Communication(QObject *parent) : QObject(parent)
{
	// Sp�cifie le nom du port s�rie (v�rifie le nom du port Arduino sur ton ordinateur)
	portSerie.setPortName("COM4"); // Remplace port "COMX" par le nom de ton port s�rie

	// Configure le d�bit en bauds, les bits de donn�es, la parit�, le nombre de bits d'arr�t, etc.
	portSerie.setBaudRate(QSerialPort::Baud9600);
	portSerie.setDataBits(QSerialPort::Data8);
	portSerie.setParity(QSerialPort::NoParity);
	portSerie.setStopBits(QSerialPort::OneStop);
	portSerie.setFlowControl(QSerialPort::NoFlowControl);

	connect(&portSerie, SIGNAL(readyRead()), this, SLOT(onReadyRead())); // Relie signal au slot lorsque readyRead est �mit

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

	// Ouverture du port s�rie
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
	portSerie.close(); // On ferme le port s�rie
	database.close(); // On ferme la BDD
}

void Communication::onReadyRead()
{
	QByteArray newData = portSerie.readAll(); // On prend tout les donn�es dispo
	nmeaDataBuffer += QString(newData); // On additionne les donn�es re�ues

	while (nmeaDataBuffer.contains("$GPGGA")) 
	{
		int start = nmeaDataBuffer.indexOf("$GPGGA"); // Recherche le d�but d'une trame
		nmeaDataBuffer = nmeaDataBuffer.mid(start); // �liminer tout avant la trame

		int end = nmeaDataBuffer.indexOf("\r\n"); // Rechercher un s�parateur de ligne (fin d'une trame)
		if (end != -1)  // Si c'est pas la fin
		{ 
			QString nmeaString = nmeaDataBuffer.left(end); // On va prendre tout le reste de la trane �  part de end donne�s
			nmeaDataBuffer = nmeaDataBuffer.mid(end + 2); // �liminer la trame trait�e et le s�parateur de ligne (\r\n)

			// Maintenant, nmeaString contient une trame GPS compl�te
			QStringList fields = nmeaString.split(','); // S�parer toutes les infos avec une virgule

			// La taille de tout les donn�es qui nous int�ressent
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
			// La trame n'est pas encore compl�te, attendez plus de donn�es.
			break;
		}
	}

}