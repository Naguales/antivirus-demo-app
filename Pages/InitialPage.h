#ifndef INITIALPAGE_H
#define INITIALPAGE_H

#include <QWidget>

class CInitialPagePrivate;
 
class CInitialPage : public QWidget
{
    Q_OBJECT
     
public:
    explicit CInitialPage(QWidget* parent = nullptr);
    ~CInitialPage();

Q_SIGNALS:
    void scanningStarted();

protected:
    CInitialPage(CInitialPagePrivate& dd, QWidget* parent = nullptr);

    //! d-pointer.
    const QScopedPointer<CInitialPagePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(CInitialPage)
    Q_DISABLE_COPY(CInitialPage)
};

#endif // INITIALPAGE_H
