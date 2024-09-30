#include "file.h"

File::File() {
    rng_.seed(time(0));
}

void File::createCvsFile(const QString fileName, int row, int column){
    QFile file(fileName+".csv");
    file.open(QFile::WriteOnly);
    QTextStream streamWrite(&file);
    for(int i=0;i<row;i++) {
        for(int j=0;j<column;j++) {
            streamWrite<<generateRandStr();
            if(j!=column-1) {
                streamWrite<<";";
            }
        }
        streamWrite<<"\n";
    }
    file.close();
}

QString File::generateRandStr() {
    QString symbols = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString outputRandStr;

    int ramdom;
    for(int i=0;i<8;i++){
        ramdom=rng_()%(symbols.size()-1);
        outputRandStr.append(symbols[ramdom]);
    }
    return outputRandStr;
}


QJsonDocument File::convertFileFromCsvToJson(const QString fileName) {
    QJsonObject data;
    this->file_name_=fileName;
    QFile file(fileName+".csv");
    file.open(QFile::ReadOnly|QFile::Text);

    QTextStream streamRead(&file);
    QVariantMap rowMap;
    QJsonArray arrayData;
    while(!streamRead.atEnd()) {
        QString line=streamRead.readLine();
        rowMap.clear();
        int i=0;

        for(QString item:line.split(";")) {
            rowMap.insert("column"+QString::number(i), item);
            i++;
        }
        arrayData.append(QJsonValue::fromVariant(rowMap));
    }
    data["data"]=arrayData;

    file.close();
    return QJsonDocument(data);
}

void File::convertFileFromJsonToCsv(QJsonDocument& document) {
    QJsonObject jsonObject = document.object();
    QJsonArray jsonArray = jsonObject["data"].toArray();

    QFile file(file_name_+"_new.csv");
    file.open(QFile::WriteOnly);
    QTextStream streamWrite(&file);

    for (const QJsonValue &value : jsonArray) {
        QJsonObject obj = value.toObject();
        for (const QString &key : obj.keys()) {
             streamWrite<<obj[key].toString()+";";
        }
        streamWrite<<"\n";
    }
    file.close();

    printStatistics(jsonObject["statistics"].toObject());
}


void File::printStatistics(QJsonObject statistics) {
    for (const QString &key : statistics.keys()) {
        qDebug()<<key<<" "<<statistics[key].toInt();
    }
}







