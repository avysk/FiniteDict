#include <QApplication>

#include "finitedict.h"

int main(int argc, char *argv[])
{
        QApplication app(argc, argv);
        FiniteDict finitedict;
        finitedict.show();
        return finitedict.exec();
}
