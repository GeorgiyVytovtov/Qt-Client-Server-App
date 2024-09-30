#include "network.h"

Network::Network() {
    socket_ = new QTcpSocket;
    file_ = new File;

    connect(socket_, &QTcpSocket::readyRead, this, &Network::readSocket);
    connect(socket_, &QTcpSocket::disconnected, this, &Network::discardSocket);
    connect(socket_, &QAbstractSocket::errorOccurred, this, &Network::displayError);

    socket_->connectToHost(QHostAddress::LocalHost,8080);

    if(socket_->waitForConnected()){
        qDebug()<<"Connected to Server";
    } else{
        qDebug()<<QString("The following error occurred: %1.").arg(socket_->errorString());
        exit(EXIT_FAILURE);
    }
}

Network::~Network() {
    if(socket_->isOpen()) {
        socket_->close();
    }
    delete file_;
}

void Network::readSocket() {
    QByteArray buffer;

    QDataStream socketStream(socket_);
    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction()) {
        QString message = QString("%1 :: Waiting for more data to come..").arg(socket_->socketDescriptor());
        qDebug()<<message;
        return;
    }

    QString header = buffer.mid(0,128);
    QString fileType = header.split(",")[0].split(":")[1];

    buffer = buffer.mid(128);

    if(fileType=="File") {
        QString fileName = header.split(",")[1].split(":")[1];
        QString size = header.split(",")[2].split(":")[1].split(";")[0];

        QString message = QString("INFO :: File from sd:%1 successfully stored on disk file with name:%2 File size:%3").arg(socket_->socketDescriptor()).arg(fileName).arg(size);
        QJsonDocument document = QJsonDocument::fromJson(buffer);
        file_->convertFileFromJsonToCsv(document);
        qDebug()<<message;
    }
}

void Network::discardSocket() {
    socket_->deleteLater();
    socket_=nullptr;
}

void Network::displayError(QAbstractSocket::SocketError socketError) {
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
        qDebug()<<QString("The following error occurred: %1.").arg(socket_->errorString());
        break;
    }
}

void Network::sendFile(QString fileName) {
    if(socket_) {
        if(socket_->isOpen()) {
            file_->createCvsFile(fileName);
            QJsonDocument data=file_->convertFileFromCsvToJson(fileName);

            QDataStream socketStream(socket_);
            socketStream.setVersion(QDataStream::Qt_5_15);

            QByteArray header;

            header.prepend(QString("fileType:File,fileName:%1,fileSize:%2;").arg(fileName).arg(data.toJson().size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = data.toJson();
            byteArray.prepend(header);

            socketStream << byteArray;
        }
        else {
            qDebug()<<"Socket doesn't seem to be opened";
        }
    } else {
        qDebug()<<"Not connected";
    }
}
