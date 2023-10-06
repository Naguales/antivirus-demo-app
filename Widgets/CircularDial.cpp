#include "CircularDial_p.h"
#include "CommonData.h"
 
#include <QPainter>
#include <QLabel>
#include <QResizeEvent>

CCirularDialPrivate::CCirularDialPrivate(CCirularDial* qPtr, int minimumAngle, int maximumAngle)
    : q_ptr(qPtr),
      m_arcColor(CommonData::DominantColor()),
      m_defaultArcColor(196, 225, 255)
{
    Q_Q(CCirularDial);

    m_scaleFactor = q->logicalDpiX() / CommonData::LogicalDpiRefValue;
    m_arcWidth = qRound(m_arcWidth * m_scaleFactor);
    q->setRange(minimumAngle, maximumAngle);
}

CCirularDialPrivate::~CCirularDialPrivate()
{}

void CCirularDialPrivate::init()
{
    Q_Q(CCirularDial);

    QString styleSheet("QDial { background-color: transparent; }");
    q->setStyleSheet(styleSheet);

    q->setStartAngle(0);
    q->setFinishAngle(360);
    q->setMinimumSize(16, 16);
    q->setArcWidth(m_arcWidth);
    q->setArcColor(m_arcColor);
    q->setDefaultArcColor(m_defaultArcColor);

    QObject::connect(q, SIGNAL(valueChanged(int)), q, SLOT(q_updateValue()));
}

void CCirularDialPrivate::q_updateValue()
{
    Q_Q(CCirularDial);

    int value = q->value();

    // Get ratio between current value and maximum to calculate angle.
    double ratio = q->maximum() ? static_cast<double>(value) / q->maximum() : static_cast<double>(value);

    m_angleSpan = m_finishSpanAngle * ratio;
    m_valueString = QString::number(value);
}

CCirularDial::CCirularDial(QWidget* parent, int minimumAngle, int maximumAngle)
    : QDial(parent),
      d_ptr(new CCirularDialPrivate(this, minimumAngle, maximumAngle))
{
    Q_D(CCirularDial);

    d->init();
}

CCirularDial::~CCirularDial()
{
    // Must define a non-inline destructor in the .cpp file, even if it is empty.
    // Else, a default one will be built in placed where CCirularDialPrivate is only forward declare,
    // leading to error in the destructor of QScopedPointer.
}

CCirularDial::CCirularDial(CCirularDialPrivate& dd, QWidget* parent)
    : QDial(parent),
    d_ptr(&dd)
{
    Q_D(CCirularDial);

    d->init();
}

QColor CCirularDial::arcColor() const
{
    return d_func()->m_arcColor;
}

void CCirularDial::setArcColor(const QColor& color)
{
    Q_D(CCirularDial);

    d->m_arcColor = color;
    d->m_arcPen.setColor(color);
    emit arcColorChanged(color);
}

QColor CCirularDial::defaultArcColor() const
{
    return d_func()->m_defaultArcColor;
}

void CCirularDial::setDefaultArcColor(const QColor& color)
{
    Q_D(CCirularDial);

    d->m_defaultArcColor = color;
    d->m_defaultArcPen.setColor(color);
    emit defaultArcColorChanged(color);
}

int CCirularDial::arcWidth() const
{
    return d_func()->m_arcWidth;
}

void CCirularDial::setArcWidth(int widthPx)
{
    Q_D(CCirularDial);

    d->m_arcWidth = widthPx;
    d->m_arcRect = QRectF(d->m_arcWidth * 0.5, d->m_arcWidth * 0.5, QDial::width() - d->m_arcWidth, QDial::height() - d->m_arcWidth);
    d->m_arcPen.setWidth(d->m_arcWidth);
    d->m_defaultArcPen.setWidth(d->m_arcWidth);
    emit arcWidthChanged(widthPx);
}

int CCirularDial::startAngle() const
{
    // The startAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
    // See QPainter::drawArc in Qt documentation.
    return qRound(d_func()->m_startAngle / 16.0);
}

void CCirularDial::setStartAngle(int angle)
{
    Q_D(CCirularDial);

    // The startAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
    // See QPainter::drawArc in Qt documentation.
    d->m_startAngle = angle * 16;
    emit startAngleChanged(angle);
}

int CCirularDial::finishAngle() const
{
    // The spanAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
    // See QPainter::drawArc in Qt documentation.
    return qRound(d_func()->m_finishSpanAngle / 16.0);
}

void CCirularDial::setFinishAngle(int angle)
{
    Q_D(CCirularDial);

    // The spanAngle must be specified in 1/16th of a degree, i.e. a full circle equals 5760 (16 * 360).
    // See QPainter::drawArc in Qt documentation.
    d->m_finishSpanAngle = angle * 16;
    emit finishAngleChanged(angle);
}

bool CCirularDial::isDrawText() const
{
    return d_func()->m_isDrawText;
}

void CCirularDial::setIsDrawText(bool isDrawText)
{
    Q_D(CCirularDial);

    d->m_isDrawText = isDrawText;
    emit isDrawTextChanged(isDrawText);
}

void CCirularDial::paintEvent(QPaintEvent*)
{
    Q_D(CCirularDial);

    QPainter painter(this);

    // So that we can use the background color.
    // Otherwise the background is transparent.
    painter.setBackgroundMode(Qt::OpaqueMode);

    // Smooth out the circle.
    painter.setRenderHint(QPainter::Antialiasing);

    // Use background color.
    painter.setBrush(painter.background());

    // No border.
    painter.setPen(QPen(Qt::NoPen));

    // Draw background circle.
    painter.drawEllipse(QDial::rect());

    if (d->m_isDrawText) {
        // Get current pen before resetting so we have access to the color() method which returns the color from the stylesheet.
        QPen textPen = painter.pen();
        painter.setPen(textPen);
        painter.drawText(d->m_valueRect, Qt::AlignHCenter | Qt::AlignTop, d->m_valueString);
    }

    painter.setPen(d->m_defaultArcPen);
    painter.drawArc(d->m_arcRect, d->m_startAngle, d->m_finishSpanAngle);
    painter.setPen(d->m_arcPen);
    painter.drawArc(d->m_arcRect, d->m_startAngle, d->m_angleSpan);
}

void CCirularDial::resizeEvent(QResizeEvent* event)
{
    Q_D(CCirularDial);

    QDial::setMinimumSize(event->size());

    double width = QDial::width() - (2 * d->m_arcWidth);
    double height = width / 2;

    d->m_valueRect = QRectF(d->m_arcWidth, height, width, height);
    d->m_arcRect = QRectF(d->m_arcWidth * 0.5, d->m_arcWidth * 0.5, QDial::width() - d->m_arcWidth, QDial::height() - d->m_arcWidth);
}

#include "moc_CircularDial.cpp"
