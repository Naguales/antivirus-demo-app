#ifndef CIRCULARDIALPRIVATE_H
#define CIRCULARDIALPRIVATE_H

#include "CircularDial.h"

#include <QString>
#include <QRectF>
#include <QPen>

class CCirularDialPrivate
{
    Q_DECLARE_PUBLIC(CCirularDial)

protected:
	//! q-pointer.
    CCirularDial* q_ptr;

public:
    CCirularDialPrivate(CCirularDial* qPtr, int minimumAngle = 0, int maximumAngle = 360);
    virtual ~CCirularDialPrivate();
	
    void init();

    //! [Private slots]
    void q_updateValue();

    QRectF m_arcRect;
    QRectF m_valueRect;
    QColor m_arcColor;
    QColor m_defaultArcColor;
    QPen m_arcPen;
    QPen m_defaultArcPen;

    //! The m_startAngle and m_finishSpanAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
    //! Positive values for the angles mean counter-clockwise while negative values mean the clockwise direction.
    //! Zero degrees is at the 3 o'clock position.
    //! See QPainter::drawArc in Qt documentation.
    int m_startAngle { 0 };
    int m_finishSpanAngle { 5760 };
    int m_arcWidth { 3 };
    double m_angleSpan { 0.0 };
    double m_scaleFactor { 1.0 };
    bool m_isDrawText { false };
    QString m_valueString;
};

#endif // CIRCULARDIALPRIVATE_H
