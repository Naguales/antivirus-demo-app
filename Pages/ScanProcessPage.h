#ifndef SCANPROCESSPAGE_H
#define SCANPROCESSPAGE_H

#include <QWidget>

class CScanProcessPagePrivate;
 
class CScanProcessPage : public QWidget
{
    Q_OBJECT
     
public:
    explicit CScanProcessPage(QWidget* parent = nullptr);
    ~CScanProcessPage();

public Q_SLOTS:
    void startScanning();

Q_SIGNALS:
    void scanningCancelled();
    void scanningFinished();
    void scanningDone();

protected:
    CScanProcessPage(CScanProcessPagePrivate& dd, QWidget* parent = nullptr);

    //! d-pointer.
    const QScopedPointer<CScanProcessPagePrivate> d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void q_onTimerTick())
    Q_PRIVATE_SLOT(d_func(), void q_dropScanningResults())
    Q_PRIVATE_SLOT(d_func(), void q_finishScanning())
    Q_PRIVATE_SLOT(d_func(), void q_doneScanning())

    Q_DECLARE_PRIVATE(CScanProcessPage)
    Q_DISABLE_COPY(CScanProcessPage)
};

#endif // SCANPROCESSPAGE_H
