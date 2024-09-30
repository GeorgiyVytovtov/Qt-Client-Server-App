#include<QTextStream>
#include<QCoreApplication>
#include<iostream>
#include"file.h"
#include"network.h"
#include"nonblockingcli.h"


#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QObject>
#include <QDebug>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    Network network;

    NonBlockingCLI cli;

    QObject::connect(&cli, &NonBlockingCLI::line_is_read, &network, &Network::sendFile);

    qDebug()<< "Enter name file: ";

    return a.exec();
}
