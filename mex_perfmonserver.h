#ifndef MEX_PERFMONSERVER_H
#define MEX_PERFMONSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDataStream>

class MEX_PerfMonServer : public QTcpServer
{
    Q_OBJECT
public:
    MEX_PerfMonServer();
    ~MEX_PerfMonServer();
signals:
    void error(QString socketerror);
    void changeStatus(QString text);
    void sendData(QString data);

public slots:
    void startServer();
    void readyRead();
    void disconnected();
    void newConnection();

private:
    QTcpSocket* socket;
    qintptr socketDescriptor;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely
};

#endif // MEX_PERFMONSERVER_H
