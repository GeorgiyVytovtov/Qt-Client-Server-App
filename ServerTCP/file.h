#ifndef FILE_H
#define FILE_H
#include <QString>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class File
{
public:
    QJsonDocument reviewFile(QByteArray& jsonData);
private:
    bool startsWithVowel(const QString& str);
    QString replaceOddNumbersWithHash(const QString& str, int& replaceCount);
};

#endif // FILE_H
