#ifndef CIRCULARDIAL_H
#define CIRCULARDIAL_H

#include <QDial>
#include <QColor>

class CCirularDialPrivate;
 
class CCirularDial : public QDial
{
    Q_OBJECT
     
    Q_PROPERTY(QColor arcColor READ arcColor WRITE setArcColor NOTIFY arcColorChanged)
    Q_PROPERTY(QColor defaultArcColor READ defaultArcColor WRITE setDefaultArcColor NOTIFY defaultArcColorChanged)
    Q_PROPERTY(double arcWidth READ arcWidth WRITE setArcWidth NOTIFY arcWidthChanged)
    Q_PROPERTY(double startAngle READ startAngle WRITE setStartAngle NOTIFY startAngleChanged)
    Q_PROPERTY(double finishAngle READ finishAngle WRITE setFinishAngle NOTIFY finishAngleChanged)
    Q_PROPERTY(bool isDrawText READ isDrawText WRITE setIsDrawText NOTIFY isDrawTextChanged)
     
public:
    explicit CCirularDial(QWidget* parent = nullptr, int minimumAngle = 0, int maximumAngle = 360);
    ~CCirularDial();

    QColor arcColor() const;
    void setArcColor(const QColor&);

    //! Color of moving arc of the dial.
    QColor defaultArcColor() const;
    void setDefaultArcColor(const QColor&);

    //! Color of static circle of the dial.
    int arcWidth() const;
    void setArcWidth(int widthPx);

    //! Angle of start arc movement.
    //! Positive values for the angles mean counter-clockwise while negative values mean the clockwise direction.
    //! Zero degrees is at the 3 o'clock position.
    int startAngle() const;
    void setStartAngle(int);

    //! Angle of finish arc movement.
    //! Positive values for the angles mean counter-clockwise while negative values mean the clockwise direction.
    //! Zero degrees is at the 3 o'clock position.
    int finishAngle() const;
    void setFinishAngle(int);

    bool isDrawText() const;
    void setIsDrawText(bool);

Q_SIGNALS:
    void arcColorChanged(const QColor&);
    void defaultArcColorChanged(const QColor&);
    void arcWidthChanged(int);
    void startAngleChanged(int);
    void finishAngleChanged(int);
    void isDrawTextChanged(bool);

protected:
    CCirularDial(CCirularDialPrivate& dd, QWidget* parent = nullptr);

    //! d-pointer.
    const QScopedPointer<CCirularDialPrivate> d_ptr;

private:
    Q_PRIVATE_SLOT(d_func(), void q_updateValue())

    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;

    Q_DECLARE_PRIVATE(CCirularDial)
    Q_DISABLE_COPY(CCirularDial)
};

#endif // CIRCULARDIAL_H
