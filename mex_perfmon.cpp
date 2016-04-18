#include "mex_perfmon.h"
#include "ui_mex_perfmon.h"
#include <QDebug>

MEX_PerfMon::MEX_PerfMon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MEX_PerfMon)
{
    ui->setupUi(this);

    timestampList.append("tsGetOrder");
    timestampList.append("tsSortOB");
    timestampList.append("tsMatch");
    timestampList.append("tsAddOrder");
    timestampList.append("tsBroadcastOrders");
    timestampList.append("tsEndOfProcess");

    txnID = "";
    listIterator = 0;

    //Set SQL DB path
    QString dbPath = QApplication::applicationDirPath() + "/perfmon.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "perfmon_connection");
    db.setDatabaseName(dbPath);

    MEX_PerfMonServer* server = new MEX_PerfMonServer();
    connect(server, &MEX_PerfMonServer::changeStatus, this, &MEX_PerfMon::newStatus);
    connect(server, &MEX_PerfMonServer::sendData, this, &MEX_PerfMon::newData);
    connect(server, &MEX_PerfMonServer::error, this, &MEX_PerfMon::getError);
    server->startServer();
}

MEX_PerfMon::~MEX_PerfMon()
{
    closeDB();
    delete ui;
}

void MEX_PerfMon::newStatus(QString text)
{
    ui->lblStatus->setText(text);
}

void MEX_PerfMon::newData(QString data)
{
    QString newtxnID = data.split('-').at(0);
    QString txnTS = data.split('-').at(1);

    if(newtxnID.remove(0,3) == txnID && listIterator < 6)
    {
        listIterator++;
        txnTS = txnTS.remove(0,3);
        newTime = QDateTime::fromString(txnTS, "hh:mm:ss.zzz");

        bool ok = false;
        QString sqlCommand = "UPDATE txnStamps SET diff" + QString::number(listIterator) + " = '" + QString::number(oldTime.time().msecsTo(newTime.time())) + "ms' WHERE id = '" + txnID + "'";
        executeQuery(sqlCommand, ok);
        if (!ok)
        {
            //Error while executing SQL-Statement
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Could not execute query.");
            messageBox.show();
        }


        bool ok2 = false;
        QString sqlCommand2 = "UPDATE txnStamps SET " + timestampList.at(listIterator) + " = '" + txnTS + "' WHERE id = '" + txnID + "'";
        executeQuery(sqlCommand2, ok2);
        if (!ok2)
        {
            //Error while executing SQL-Statement
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Could not execute query.");
            messageBox.show();
        }

        //Set new to old time for next iteration
        oldTime = newTime;
    }
    else
    {
        txnID = newtxnID;
        listIterator = 0;
        txnTS = txnTS.remove(0,3);
        oldTime = QDateTime::fromString(txnTS, "hh:mm:ss.zzz");

        bool ok = false;
        QString sqlCommand = "INSERT INTO txnStamps (id, "+timestampList.at(listIterator)+") VALUES ('" + txnID + "', '" + txnTS + "ms') ";
        executeQuery(sqlCommand, ok);
        if (!ok)
        {
            //Error while executing SQL-Statement
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Could not execute query.");
            messageBox.show();
        }
    }

    //Get current database into view

    updateTableView();
}

void MEX_PerfMon::getError(QString socketerror)
{
    QMessageBox::warning(0, "Error", socketerror);
}

void MEX_PerfMon::updateTableView()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    bool ok = false;
    QString sqlCommand = "SELECT * FROM txnStamps";
    QSqlQuery qry = executeQuery(sqlCommand, ok);
    if (!ok)
    {
        //Error while executing SQL-Statement
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        messageBox.show();
    }
    else
    {
        model->setQuery(qry);
        ui->tvMonitor->setModel(model);
    }

}

//SQL DATABASE / CONNECTION
QSqlQuery MEX_PerfMon::executeQuery (QString sqlCommand, bool &ok)
{
    if (!db.open())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No database connection.");
        messageBox.show();
        QSqlQuery emptyQuery;
        return emptyQuery;
    } else
    {
        QSqlQuery query(db);
        ok = query.exec(sqlCommand);
        return query;
    }
}

void MEX_PerfMon::closeDB()
{
    //Get connection name
    QString connection;
    connection = db.connectionName();
    //Close connection
    db.close();
    db = QSqlDatabase();
    //remove old connection
    db.removeDatabase(connection);
}

void MEX_PerfMon::on_btnUpdate_clicked()
{
    updateTableView();
}

void MEX_PerfMon::clearDB()
{
    bool ok = false;
    QString sqlCommand = "DELETE FROM txnStamps";
    executeQuery(sqlCommand, ok);
    if (!ok)
    {
        //Error while executing SQL-Statement
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        messageBox.show();
    }
}


void MEX_PerfMon::on_btnClear_clicked()
{
    clearDB();
    updateTableView();
}
