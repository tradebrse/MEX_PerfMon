#include "mex_perfmon.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MEX_PerfMon w;
    w.show();

    return a.exec();
}
