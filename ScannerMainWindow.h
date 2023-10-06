#ifndef SCANNERMAINWINDOW_H
#define SCANNERMAINWINDOW_H

#include <QMainWindow>

class CScannerMainWindowPrivate;

class CScannerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CScannerMainWindow(QWidget *parent = nullptr);
    ~CScannerMainWindow();

public Q_SLOTS:
    void startScanning();

protected:
     CScannerMainWindow( CScannerMainWindowPrivate& dd, QWidget* parent = nullptr);

    //! d-pointer.
    const QScopedPointer< CScannerMainWindowPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(CScannerMainWindow)
    Q_DISABLE_COPY(CScannerMainWindow)
};
#endif // SCANNERMAINWINDOW_H
