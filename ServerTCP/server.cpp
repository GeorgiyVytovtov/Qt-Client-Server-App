#include "server.h"


Server::Server() {
    server_ = new QTcpServer();
    file_ = new File;

    if(server_->listen(QHostAddress::Any, 8080)) {
        connect(server_, &QTcpServer::newConnection, this, &Server::newConnection);
        qDebug()<<"Server is listening...";
    } else {
        qDebug()<<QString("Unable to start the server: %1.").arg(server_->errorString());
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    foreach (QTcpSocket* socket, connection_set_) {
        socket->close();
        socket->deleteLater();
    }

    server_->close();
    server_->deleteLater();
    delete file_;
}

void Server::newConnection() {
    while (server_->hasPendingConnections()) {
        appendToSocketList(server_->nextPendingConnection());
    }
}

void Server::appendToSocketList(QTcpSocket* socket)
{
    connection_set_.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &Server::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &Server::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &Server::displayError);
    qDebug()<<QString("INFO :: Client with sockd:%1 has just entered the room").arg(socket->socketDescriptor());
}

void Server::readSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket->socketDescriptor());
        qDebug()<<message;
        return;
    }

    QString header = buffer.mid(0,128);
    QString fileType = header.split(",")[0].split(":")[1];

    buffer = buffer.mid(128);

    if(fileType=="File"){
        QString fileName = header.split(",")[1].split(":")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        QString message = QString("INFO :: File from sd:%1 successfully received file with name:%2 File size:%3").arg(socket->socketDescriptor()).arg(fileName).arg(size);

        QJsonDocument document=file_->reviewFile(buffer);

        sendFile(socket,document);
        qDebug()<<message;
    }
}

void Server::discardSocket() {
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set_.find(socket);
    if (it != connection_set_.end()) {
        qDebug()<<QString("INFO :: A client has just left the room").arg(socket->socketDescriptor());
        connection_set_.remove(*it);
    }
    socket->deleteLater();
}

void Server::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<"The host was not found. Please check the host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<"The connection was refused by the peer. Make sure QTCPServer is running, and check that the host name and port settings are correct.";
        break;
    default:
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        qDebug()<<QString("The following error occurred: %1.").arg(socket->errorString());
        break;
    }
}

void Server::sendFile(QTcpSocket* socket, QJsonDocument document)
{
    if(socket) {
        if(socket->isOpen()) {
            QString dataDocument = document.toJson();

            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_15);

            QByteArray header;

            header.prepend(QString("fileType:File,fileName:%1,fileSize:%2;").arg("newFile").arg(dataDocument.size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = dataDocument.toUtf8();
            byteArray.prepend(header);

            socketStream.setVersion(QDataStream::Qt_5_15);
            socketStream << byteArray;
        } else {
            qDebug()<<"Socket doesn't seem to be opened";
        }
    } else {
        qDebug()<<"Not connected";
    }
}
