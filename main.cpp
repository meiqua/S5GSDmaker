#include "selectui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    selectUI w;
    w.show();

    return a.exec();
}
