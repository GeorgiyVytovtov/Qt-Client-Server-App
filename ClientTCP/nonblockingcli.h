#ifndef NONBLOCKINGCLI_H
#define NONBLOCKINGCLI_H

#include <QObject>
#include <QThread>
#include <QString>

class NonBlockingCLI : public QObject
{
    Q_OBJECT
public:
    explicit NonBlockingCLI(QObject* parent = nullptr);

private:
    QThread thread_;

signals:
    void line_is_read(QString line);

private slots:
    void read_stdin();
};

#endif // NONBLOCKINGCLI_H
