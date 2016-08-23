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

    //Set clear database model
    on_btnClear_clicked();
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
    QString txnNS = data.split('-').at(2);


    ///WEITERARBEITEN : Datenbank überarbeiten für Nanosec, evtl umwandeln zu microsec
    //If new txnID is the same as in last data package, update the next column of that txnID
    if(newtxnID.remove(0,3) == txnID && listIterator < 6)
    {
        listIterator++;
        txnTS = txnTS.remove(0,3);
        txnNS = txnNS.remove(0,3);


        bool ok = false;
        QString sqlCommand = "UPDATE txnStamps SET duration" + QString::number(listIterator) + " = '" + txnNS + "ns' WHERE id = '" + txnID + "'";
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
    }
    //Else make a new entry for that txnID in the database
    else
    {
        txnID = newtxnID;
        listIterator = 0;
        txnTS = txnTS.remove(0,3);

        bool ok = false;
        QString sqlCommand = "INSERT INTO txnStamps (id, "+timestampList.at(listIterator)+") VALUES ('" + txnID + "', '" + txnTS + "') ";
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

void MEX_PerfMon::setEnabled()
{
    this->setEnabled();
}

void MEX_PerfMon::on_pushButton_clicked()
{
    if(ui->tvMonitor->model()->rowCount() > 0)
    {
        QList<QList<int>> durationList;// = new QList<QList<int>>;
        QList<int> duration1;//= new QList<int>();
        QList<int> duration2;//= new QList<int>();
        QList<int> duration3;//= new QList<int>();
        QList<int> duration4;// = new QList<int>();
        QList<int> duration5;//= new QList<int>();
        for (int row = 0 ; row < ui->tvMonitor->model()->rowCount() ; ++row) {
            for (int col = 0 ; col < ui->tvMonitor->model()->columnCount() ; ++col) {
                if(col == 2)
                {
                    duration1.append(ui->tvMonitor->model()->data(ui->tvMonitor->model()->index(row,col)).toString().remove("ns").toInt());
                }
                else if(col == 4)
                {
                    duration2.append(ui->tvMonitor->model()->data(ui->tvMonitor->model()->index(row,col)).toString().remove("ns").toInt());
                }
                else if(col == 6)
                {
                    duration3.append(ui->tvMonitor->model()->data(ui->tvMonitor->model()->index(row,col)).toString().remove("ns").toInt());
                }
                else if(col == 8)
                {
                    duration4.append(ui->tvMonitor->model()->data(ui->tvMonitor->model()->index(row,col)).toString().remove("ns").toInt());
                }
                else if(col == 10)
                {
                    duration5.append(ui->tvMonitor->model()->data(ui->tvMonitor->model()->index(row,col)).toString().remove("ns").toInt());
                }
            }
        }
        durationList.append(duration1);
        durationList.append(duration2);
        durationList.append(duration3);
        durationList.append(duration4);
        durationList.append(duration5);


        //Calculate average durations
        QList<int> lowestList; //= new QList<double>;
        QList<int> highestList; //= new QList<double>;
        QList<int> averageList; //= new QList<double>;
        int emptyCellCounter = 0;

        for (int j=0; j<durationList.size(); ++j)
        {
            int total = 0;
            int lowest = 999999999;
            int highest = 0;
            for (int i=0; i<durationList.at(j).size(); ++i)
            {
                //Duration list debug:
               // qDebug() << "dList debug[" << j <<"] ["<< i <<"] = " << QString::number(durationList.at(j).at(i));
                if(durationList.at(j).at(i) < lowest)
                {
                    if(durationList.at(j).at(i) != 0)
                    {
                        lowest = durationList.at(j).at(i);
                    }
                    else
                    {
                        emptyCellCounter++;
                    }
                }
                if(durationList.at(j).at(i) > highest)
                {
                    highest = durationList.at(j).at(i);
                }
                total += durationList.at(j).at(i);
            }
            lowestList.append(lowest);
            highestList.append(highest);
            averageList.append(total/(durationList.at(j).size()-emptyCellCounter));
        }

        MEX_PerfMonChart *perfMonChart = new MEX_PerfMonChart();
        perfMonChart->setAttribute(Qt::WA_DeleteOnClose);

        // connect(perfMonChart, SIGNAL(destroyed()), this, SLOT(updateTableView()));
        connect(this, SIGNAL(destroyed()), perfMonChart, SLOT(close()));
        perfMonChart->setChart(lowestList, highestList, averageList);
        perfMonChart->show();
        //lowest, highest, average list debug:
        //qDebug() << "l1" << lowestList.at(1) << "h1" << highestList.at(1) << "a1" << averageList.at(1);
    }
    else
    {
        QMessageBox::warning(0,"Error","No data available.");
    }
}

void MEX_PerfMon::closeEvent(QCloseEvent*)
{
    qApp->quit();
}
