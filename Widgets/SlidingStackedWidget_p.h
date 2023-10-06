#ifndef SLIDINGSTACKEDWIDGETPRIVATE_H
#define SLIDINGSTACKEDWIDGETPRIVATE_H

#include "SlidingStackedWidget.h"

#include <QParallelAnimationGroup>

class CSlidingStackedWidgetPrivate
{
    Q_DECLARE_PUBLIC(CSlidingStackedWidget)

protected:
    //! q-pointer.
    CSlidingStackedWidget* q_ptr;

public:
    CSlidingStackedWidgetPrivate(CSlidingStackedWidget* qPtr);
    virtual ~CSlidingStackedWidgetPrivate();

    //! [Private slots]
    void q_animationDoneSlot();

    void slideWidget(QWidget*);

    QScopedPointer<QParallelAnimationGroup> m_animationGroup { nullptr };
    QPoint m_currentPoint { 0, 0 };
    QEasingCurve::Type m_animationType { QEasingCurve::OutQuart };

    bool m_isAnimationVertical { false };
    bool m_isAnimationActive { false };
    int m_animationSpeed { 500 };
    int m_currentWidgetIndex { 0 };
    int m_proposedCurrentWidgetIndex { 0 };
    int m_nextWidgetIndex { 0 };
};

#endif // SLIDINGSTACKEDWIDGETPRIVATE_H
