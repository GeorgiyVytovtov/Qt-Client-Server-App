#include "file.h"

bool File::startsWithVowel(const QString& str) {
    QString vowels = "AEIOUaeiou";
    return vowels.contains(str[0]);
}

QString File::replaceOddNumbersWithHash(const QString& str, int& replaceCount) {
    QString result = str;
    for (int i = 0; i < result.length(); ++i) {
        if (result[i].isDigit()) {
            int digit = result[i].digitValue();
            if (digit % 2 != 0) {
                result[i] = '#';
                ++replaceCount;
            }
        }
    }
    return result;
}

QJsonDocument File::reviewFile(QByteArray& jsonData)
{
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = document.object();
    QJsonArray jsonArray = jsonObject["data"].toArray();

    QJsonArray modifiedArray;
    int removedObjectsCount = 0;
    int replacedSymbolsCount = 0;

    for (const QJsonValue &value : jsonArray) {
        QJsonObject obj = value.toObject();
        bool removeObject = false;

        for (const QString &key : obj.keys()) {
            if (startsWithVowel(obj[key].toString())) {
                removeObject = true;
                break;
            }
        }

        if (!removeObject) {
            for (const QString &key : obj.keys()) {
                QString modifiedValue = replaceOddNumbersWithHash(obj[key].toString(), replacedSymbolsCount);
                obj[key] = modifiedValue;
            }
            modifiedArray.append(obj);
        } else {
            ++removedObjectsCount;
        }
    }

    QJsonObject statistics;
    statistics["removed_objects_count"] = removedObjectsCount;
    statistics["replaced_symbols_count"] = replacedSymbolsCount;

    jsonObject["data"] = modifiedArray;
    jsonObject["statistics"] = statistics;

    return QJsonDocument(jsonObject);
}


