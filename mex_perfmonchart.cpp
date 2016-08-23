#include "mex_perfmonchart.h"
#include "ui_mex_perfmonchart.h"

using namespace QtCharts;

MEX_PerfMonChart::MEX_PerfMonChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MEX_PerfMonChart)
{
    ui->setupUi(this);
}

MEX_PerfMonChart::~MEX_PerfMonChart()
{
    delete ui;
}

void MEX_PerfMonChart::setChart(QList<int> lowestList, QList<int> highestList, QList<int> averageList)
{
    //Set layout
    QGridLayout *layout = new QGridLayout(this);

    QBarSet *low = new QBarSet("Lowest");
    QBarSet *high = new QBarSet("Highest");
    QBarSet *average = new QBarSet("Average");

    ///-1 entfernen um letzten chart anzuzeigen - for (int i=0; i<(lowestList.size()-1); ++i)
    for (int i=0; i<(lowestList.size()); ++i)
    {
        //Chart list debug:
        //qDebug() <<"avglist at(" << i  << ")" << averageList.at(i);
        *low << lowestList.at(i);
        *high << highestList.at(i);
        *average<< averageList.at(i);
    }

    QBarSeries* series = new QBarSeries();
    series->append(low);
    series->append(high);
    series->append(average);

    QChart *chart = new QChart();
    chart->addSeries(series);

    QStringList durations;
    durations << "duration1" << "duration2" << "duration3" << "duration4" << "duration5";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(durations);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chart->setTitle("Durations in nanoseconds");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    layout->addWidget(chartView,0,0);
}
