#ifndef MEX_PERFMONCHART_H
#define MEX_PERFMONCHART_H

#include <QWidget>
#include <QtCharts>

namespace Ui {
class MEX_PerfMonChart;
}

class MEX_PerfMonChart : public QWidget
{
    Q_OBJECT

public:
    explicit MEX_PerfMonChart(QWidget *parent = 0);
    ~MEX_PerfMonChart();
    void setChart(QList<int> lowestList, QList<int> highestList, QList<int> averageList);

private:
    Ui::MEX_PerfMonChart *ui;
};

#endif // MEX_PERFMONCHART_H
