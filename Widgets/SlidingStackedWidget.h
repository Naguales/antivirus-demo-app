#ifndef SLIDINGSTACKEDWIDGET_H
#define SLIDINGSTACKEDWIDGET_H

#include "CommonData.h"

#include <QStackedWidget>
#include <QEasingCurve>

class CSlidingStackedWidgetPrivate;

//! The CSlidingStackedWidget class provides a stack of widgets where only one widget is visible at a time.
//! It's derived from QtStackedWidget and allows smooth side shifting of widgets,
//! in addition to the original hard switching from one to another as offered by QStackedWidget itself.
class CSlidingStackedWidget : public QStackedWidget
{
    Q_OBJECT

    Q_PROPERTY(QEasingCurve::Type animationType READ animationType WRITE setAnimationType NOTIFY animationTypeChanged)
    Q_PROPERTY(int animationSpeed READ animationSpeed WRITE setAnimationSpeed NOTIFY animationSpeedChanged)
    Q_PROPERTY(bool isAnimationVertical READ isAnimationVertical WRITE setIsAnimationVertical NOTIFY isAnimationVerticalChanged)

public:
    explicit CSlidingStackedWidget(QWidget* parent);
    ~CSlidingStackedWidget();

    //! The easing curves type for controlling animation.
    QEasingCurve::Type animationType() const;
    void setAnimationType(QEasingCurve::Type animationtype);

    //! The animation speed in milliseconds.
    int animationSpeed() const;
    void setAnimationSpeed(int);

    bool isAnimationVertical() const;
    void setIsAnimationVertical(bool);

public Q_SLOTS:
    //! Slides to a widget with the specified index.
    void slide(int);

Q_SIGNALS:
    void animationFinished();
    void animationTypeChanged(QEasingCurve::Type);
    void animationSpeedChanged(int);
    void isAnimationVerticalChanged(bool);

protected:
    CSlidingStackedWidget(CSlidingStackedWidgetPrivate& dd, QWidget* parent = nullptr);

    //! d-pointer.
    const QScopedPointer<CSlidingStackedWidgetPrivate> d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void q_animationDoneSlot())

    Q_DECLARE_PRIVATE(CSlidingStackedWidget)
    Q_DISABLE_COPY(CSlidingStackedWidget)
};

#endif // SLIDINGSTACKEDWIDGET_H
