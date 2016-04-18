#ifndef MEX_PERFMON_H
#define MEX_PERFMON_H

#include <QMainWindow>
#include <mex_perfmonserver.h>
#include <QMessageBox>
#include <QtSql>

namespace Ui {
class MEX_PerfMon;
}

class MEX_PerfMon : public QMainWindow
{
    Q_OBJECT

public:
    explicit MEX_PerfMon(QWidget *parent = 0);
    ~MEX_PerfMon();

public slots:
    void newStatus(QString text);
    void newData(QString data);
    void getError(QString socketerror);

private slots:
    QSqlQuery executeQuery (QString sqlCommand, bool &ok);
    void closeDB();
    void updateTableView();
    void clearDB();

    void on_btnUpdate_clicked();

    void on_btnClear_clicked();

private:
    Ui::MEX_PerfMon *ui;
    // SQL-DB //
    QSqlDatabase db;
    QList<QString> timestampList;
    QString txnID;
    int listIterator;
    QDateTime oldTime;
    QDateTime newTime;
};

#endif // MEX_PERFMON_H
