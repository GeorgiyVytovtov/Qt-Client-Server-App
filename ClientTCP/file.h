#ifndef FILE_H
#define FILE_H
#include<QString>
#include<QByteArray>
#include<QFile>
#include<QTextStream>
#include<random>
#include<chrono>
#include<time.h>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>

class File
{
private:
    QString generateRandStr();
    std::mt19937 rng_;
    QString file_name_;
    void printStatistics(QJsonObject statistics);
public:
    File();
    void createCvsFile(const QString fileName, int row=1024, int column=6);
    QJsonDocument convertFileFromCsvToJson(const QString fileName);
    void convertFileFromJsonToCsv(QJsonDocument& document);
};

#endif // FILE_H
