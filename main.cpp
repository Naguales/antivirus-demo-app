#include "ScannerMainWindow.h"
#include "CommonData.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Software change pixel font size, some controls font size doesn't change automatically on high DPI.
    QFont font = a.font();
    QFontMetrics fontMetrics = a.fontMetrics();
    font.setPixelSize(qRound(14 * fontMetrics.fontDpi() / CommonData::LogicalDpiRefValue));
    a.setFont(font);

    CScannerMainWindow w;
    w.show();
    return a.exec();
}
