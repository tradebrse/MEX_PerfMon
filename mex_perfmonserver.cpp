#include "mex_perfmonserver.h"
#include <iostream>
#include <QDebug>

static inline qint32 ArrayToInt(QByteArray source);

MEX_PerfMonServer::MEX_PerfMonServer()
{
connect(this, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

MEX_PerfMonServer::~MEX_PerfMonServer()
{
    delete socket;
}

void MEX_PerfMonServer::startServer()
{
    int port = 4444;
    if(this->listen(QHostAddress::Any, port))
    {
        emit changeStatus(" Listening to port " + QString::number(port) + "....");
    }
    else
    {
        emit changeStatus(" Could not start server.");
    }
}

void MEX_PerfMonServer::newConnection()
{

    while (this->hasPendingConnections())
    {
        socket = this->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        buffers.insert(socket, buffer);
        sizes.insert(socket, s);
   }

    emit changeStatus(" Client connected");
}

void MEX_PerfMonServer::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) //While can process data, process it
        {
            if (size == 0 && buffer->size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit sendData(data);
            }
        }
    }
}


void MEX_PerfMonServer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    socket->deleteLater();
    delete buffer;
    delete s;
    emit changeStatus(" Client disconnected");
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
