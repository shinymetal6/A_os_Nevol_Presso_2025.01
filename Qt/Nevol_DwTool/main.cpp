#include "nevol_dwtool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Nevol_DwTool w;
    w.show();
    return a.exec();
}
