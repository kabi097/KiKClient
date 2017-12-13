#include "gra.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gra w;
    w.show();

    return a.exec();
}
