#ifndef NETWORK_H
#define NETWORK_H

#include <QHostAddress>
#include <QTcpSocket>
#include<QFile>
#include<QFileInfo>
#include<QDataStream>
#include"file.h"

class Network: public QObject
{
    Q_OBJECT
public:
    Network();
    ~Network();
private slots:
    void readSocket();
    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);
public slots:
    void sendFile(QString fileName);
private:
    QTcpSocket* socket_;
    File* file_;
};

#endif // NETWORK_H
