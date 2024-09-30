#ifndef SERVER_H
#define SERVER_H
#include <QDebug>
#include <QFile>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QFileInfo>
#include"file.h"

class Server: public QObject
{
    Q_OBJECT
public:
    Server();
    ~Server();
private slots:
    void newConnection();
    void appendToSocketList(QTcpSocket* socket);

    void readSocket();
    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);

    void sendFile(QTcpSocket* socket, QJsonDocument document);
private:
    QTcpServer* server_;
    QSet<QTcpSocket*> connection_set_;
    File* file_;
};

#endif // SERVER_H
