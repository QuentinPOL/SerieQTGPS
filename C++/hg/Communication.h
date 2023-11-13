#pragma once

#include <QtCore/QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

class Communication : public QObject
{
	Q_OBJECT
public:
	Communication(QObject *parent = 0);
	~Communication();

	QSerialPort portSerie; // Création du port serie
	QString nmeaDataBuffer; // Tampon pour collecter les caractères entrants
	QSqlDatabase database; // Base de donnée

	int isOpen; // Pour savoir si la bdd et open

public slots:
	void onReadyRead(); // Lire les données reçu du port COM (GPS)
};



