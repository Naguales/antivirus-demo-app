#include "SlidingStackedWidget_p.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

CSlidingStackedWidgetPrivate::CSlidingStackedWidgetPrivate(CSlidingStackedWidget* qPtr)
    : q_ptr(qPtr)
{}

CSlidingStackedWidgetPrivate::~CSlidingStackedWidgetPrivate()
{
    m_animationGroup.reset(nullptr);
}

void CSlidingStackedWidgetPrivate::q_animationDoneSlot()
{
    Q_Q(CSlidingStackedWidget);

    q->setCurrentIndex(m_nextWidgetIndex);
    q->widget(m_currentWidgetIndex)->hide();
    q->widget(m_currentWidgetIndex)->move(m_currentPoint);
    m_isAnimationActive = false;
    m_animationGroup.reset(nullptr);

    // Animate again if the current index was changed while is animation occurred.
    if (m_proposedCurrentWidgetIndex != q->currentIndex()) {
        q->slide(m_proposedCurrentWidgetIndex);
    } else {
        emit q->animationFinished();
    }
}

void CSlidingStackedWidgetPrivate::slideWidget(QWidget* newWidget)
{
    Q_Q(CSlidingStackedWidget);

    m_isAnimationActive = true;
    CommonData::EAnimationDirection animationDirection;
    int currentWidgetIndex = q->currentIndex(),
        nextWidgetIndex = q->indexOf(newWidget);

    if (currentWidgetIndex == nextWidgetIndex) {
        m_isAnimationActive = false;
        emit q->currentChanged(q->currentIndex());
        return;
    } else if (currentWidgetIndex < nextWidgetIndex) {
        animationDirection = m_isAnimationVertical ? CommonData::EAnimationDirection::eadTopToBottom : CommonData::EAnimationDirection::eadRightToLeft;
    } else {
        animationDirection = m_isAnimationVertical ? CommonData::EAnimationDirection::eadBottomToTop : CommonData::EAnimationDirection::eadLeftToRight;
    }

    // Calculate the shifts.
    int offsetX = q->frameRect().width(),
        offsetY = q->frameRect().height();

    // The following is important, to ensure that the new widget has correct geometry information when sliding in first time.
    q->widget(nextWidgetIndex)->setGeometry(0, 0, offsetX, offsetY);

    if (animationDirection == CommonData::EAnimationDirection::eadBottomToTop) {
        offsetX = 0;
        offsetY = -offsetY;
    } else if (animationDirection == CommonData::EAnimationDirection::eadTopToBottom) {
        offsetX = 0;
    } else if (animationDirection == CommonData::EAnimationDirection::eadRightToLeft) {
        offsetX = -offsetX;
        offsetY = 0;
    } else if (animationDirection == CommonData::EAnimationDirection::eadLeftToRight) {
        offsetY = 0;
    }

    // Reposition the next widget outside/aside of the display area.
    QPoint nextPoint = q->widget(nextWidgetIndex)->pos(),
           currentPoint = q->widget(currentWidgetIndex)->pos();
    m_currentPoint = currentPoint;

    q->widget(nextWidgetIndex)->move(nextPoint.x() - offsetX, nextPoint.y() - offsetY);

    // Make it visible/show.
    q->widget(nextWidgetIndex)->show();
    q->widget(nextWidgetIndex)->raise();

    // Animate both the current and next widgets to the side using animation framework.
    QPropertyAnimation* currentWidgetAnimation = new QPropertyAnimation(q->widget(currentWidgetIndex), "pos");
    currentWidgetAnimation->setDuration(m_animationSpeed);
    currentWidgetAnimation->setEasingCurve(m_animationType);
    currentWidgetAnimation->setStartValue(currentPoint);
    currentWidgetAnimation->setEndValue(QPoint(offsetX + currentPoint.x(), offsetY + currentPoint.y()));

    QPropertyAnimation* nextWidgetAnimation = new QPropertyAnimation(q->widget(nextWidgetIndex), "pos");
    nextWidgetAnimation->setDuration(m_animationSpeed);
    nextWidgetAnimation->setEasingCurve(m_animationType);
    nextWidgetAnimation->setStartValue(QPoint(-offsetX + nextPoint.x(), offsetY + nextPoint.y()));
    nextWidgetAnimation->setEndValue(nextPoint);

    m_animationGroup.reset(new QParallelAnimationGroup(q));

    m_animationGroup->addAnimation(currentWidgetAnimation);
    m_animationGroup->addAnimation(nextWidgetAnimation);

    QObject::connect(m_animationGroup.data(), SIGNAL(finished()), q, SLOT(q_animationDoneSlot()));

    m_nextWidgetIndex = nextWidgetIndex;
    m_currentWidgetIndex = currentWidgetIndex;
    m_isAnimationActive = true;
    m_animationGroup->start();
}

CSlidingStackedWidget::CSlidingStackedWidget(QWidget* parent)
    : QStackedWidget(parent),
      d_ptr(new CSlidingStackedWidgetPrivate(this))
{}

CSlidingStackedWidget::~CSlidingStackedWidget()
{
    // Must define a non-inline destructor in the .cpp file, even if it is empty.
    // Else, a default one will be built in placed where CSlidingStackedWidgetPrivate is only forward declare,
    // leading to error in the destructor of QScopedPointer.
}

CSlidingStackedWidget::CSlidingStackedWidget(CSlidingStackedWidgetPrivate& dd, QWidget* parent)
    : QStackedWidget(parent),
    d_ptr(&dd)
{}

QEasingCurve::Type CSlidingStackedWidget::animationType() const
{
    return d_func()->m_animationType;
}

void CSlidingStackedWidget::setAnimationType(QEasingCurve::Type animationType)
{
    Q_D(CSlidingStackedWidget);

    d->m_animationType = animationType;
    emit animationTypeChanged(animationType);
}

int CSlidingStackedWidget::animationSpeed() const
{
    return d_func()->m_animationSpeed;
}

void CSlidingStackedWidget::setAnimationSpeed(int animationSpeed)
{
    Q_D(CSlidingStackedWidget);

    d->m_animationSpeed = animationSpeed;
    emit animationSpeedChanged(animationSpeed);
}

bool CSlidingStackedWidget::isAnimationVertical() const
{
    return d_func()->m_isAnimationVertical;
}

void CSlidingStackedWidget::setIsAnimationVertical(bool isAnimationVertical)
{
    Q_D(CSlidingStackedWidget);

    d->m_isAnimationVertical = isAnimationVertical;
    emit isAnimationVerticalChanged(isAnimationVertical);
}

void CSlidingStackedWidget::slide(int widgetIndex)
{
    Q_D(CSlidingStackedWidget);

    d->m_proposedCurrentWidgetIndex = widgetIndex;
    if (!d->m_isAnimationActive) {
        widgetIndex = widgetIndex > count() - 1 ? widgetIndex % count() : (widgetIndex + count()) % count();
        d->slideWidget(widget(widgetIndex));
    }
}

#include "moc_SlidingStackedWidget.cpp"
