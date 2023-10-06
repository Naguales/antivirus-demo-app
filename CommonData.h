#ifndef COMMONDATA_H
#define COMMONDATA_H

#include <QString>
#include <QColor>

namespace CommonData {

    enum class EPages : unsigned char {
        epInitial = 0,
        epScanProcess
    };

    enum class EScanningStage : unsigned char {
        essIdle = 0,
        essScanningBrowsers,
        essCheckingForSoftwareUpdates,
        essScanningMemory,
        essScanningStartupApps,
        essScanningFileSystem
    };

    //! This enumeration is used to define the animation direction.
    enum class EAnimationDirection : unsigned char {
        eadLeftToRight = 0,
        eadRightToLeft,
        eadTopToBottom,
        eadBottomToTop,
        eadAutomatic
    };

    inline constexpr int ButtonPaddingX = 5;
    inline constexpr int ButtonPaddingY = 5;
    inline constexpr int ButtonBorderWidth = 1;
    inline constexpr int ButtonBorderRadius = 3;

    // The horizontal resolution of the device in dots per inch corresponding to the 100% DPI setting.
    inline constexpr double LogicalDpiRefValue = 96.0;

    static QColor DominantColor()
    {
        static QColor dominantColor(45, 127, 249);
        return dominantColor;
    }

    static QColor HoverColor()
    {
        static QColor hoverColor(16, 80, 200);
        return hoverColor;
    }

    static QColor PressedColor()
    {
        static QColor pressedColor(20, 100, 230);
        return pressedColor;
    }

    static QColor GlowColor()
    {
        static QColor glowColor(16, 80, 200, 80);
        return glowColor;
    }

    static QString ActionPushButtonStyleSheet(double scaleFactor = 1.0)
    {
        QColor dominantColor = DominantColor(),
                hoverColor = HoverColor(),
                pressedColor = PressedColor();
        QString actionPushButtonStyleSheet = QString("QPushButton { font-weight: bold; margin: 0px; padding: %1 %2px; color: rgb(%3, %4, %5); background-color: white; border: %6px solid; border-radius: %7px; border-color: rgb(%8, %9, %10); }")
                .arg(QString::number(qRound(ButtonPaddingX * scaleFactor)),
                     QString::number(qRound(ButtonPaddingY * scaleFactor)),
                     QString::number(dominantColor.red()),
                     QString::number(dominantColor.green()),
                     QString::number(dominantColor.blue()),
                     QString::number(qRound(ButtonBorderWidth * scaleFactor)),
                     QString::number(qRound(ButtonBorderRadius * scaleFactor)),
                     QString::number(dominantColor.red()),
                     QString::number(dominantColor.green()),
                     QString::number(dominantColor.blue()));
        actionPushButtonStyleSheet.append(QString("QPushButton:hover { color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(hoverColor.red()), QString::number(hoverColor.green()), QString::number(hoverColor.blue())));
        actionPushButtonStyleSheet.append(QString("QPushButton:pressed { color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(pressedColor.red()), QString::number(pressedColor.green()), QString::number(pressedColor.blue())));
        actionPushButtonStyleSheet.append(QString("QPushButton:pressed:hover { color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(pressedColor.red()), QString::number(pressedColor.green()), QString::number(pressedColor.blue())));
        return actionPushButtonStyleSheet;
    }

    static QString SignificantButtonStyleSheet(double scaleFactor = 1.0)
    {
        QColor dominantColor = DominantColor();
        const QString significantButtonStyleSheet = QString("QPushButton { font-weight: bold; margin: 0px; padding: %1 %2px; color: white; background-color: rgb(%3, %4, %5); border: %6px solid; border-radius: %7px; border-color: rgb(%8, %9, %10); }")
                .arg(QString::number(qRound(ButtonPaddingX * scaleFactor)),
                     QString::number(qRound(ButtonPaddingY * scaleFactor)),
                     QString::number(dominantColor.red()),
                     QString::number(dominantColor.green()),
                     QString::number(dominantColor.blue()),
                     QString::number(qRound(ButtonBorderWidth * scaleFactor)),
                     QString::number(qRound(ButtonBorderRadius * scaleFactor)),
                     QString::number(dominantColor.red()),
                     QString::number(dominantColor.green()),
                     QString::number(dominantColor.blue()));
        return significantButtonStyleSheet;
    }

    static QString SignificantButtonWithHoverStyleSheet(double scaleFactor = 1.0)
    {
        QString significantButtonWithHoverStyleSheet = SignificantButtonStyleSheet(scaleFactor);
        const QColor hoverColor = HoverColor(),
                pressedColor = PressedColor();
        significantButtonWithHoverStyleSheet.append(QString("QPushButton:hover { background-color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(hoverColor.red()), QString::number(hoverColor.green()), QString::number(hoverColor.blue())));
        significantButtonWithHoverStyleSheet.append(QString("QPushButton:pressed { background-color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(pressedColor.red()), QString::number(pressedColor.green()), QString::number(pressedColor.blue())));
        significantButtonWithHoverStyleSheet.append(QString("QPushButton:pressed:hover { background-color: rgb(%1, %2, %3); }")
                                          .arg(QString::number(pressedColor.red()), QString::number(pressedColor.green()), QString::number(pressedColor.blue())));
        return significantButtonWithHoverStyleSheet;
    }

    static QString InertButtonStyleSheet(double scaleFactor = 1.0)
    {
        const QString inertButtonStyleSheet = QString("QPushButton { margin: 0px; padding: %1 %2px; color: rgb(59, 59, 59); border: none; }")
                .arg(QString::number(qRound(ButtonPaddingX * scaleFactor)),
                     QString::number(qRound(ButtonPaddingY * scaleFactor)));
        return inertButtonStyleSheet;
    }

    static QString ToolButtonStyleSheet(double scaleFactor = 1.0)
    {
        QString toolButtonStyleSheet = QString("QToolButton { margin: 0px; padding: %1 %2 px; border: none; }")
                .arg(QString::number(qRound(ButtonPaddingX * scaleFactor)),
                     QString::number(qRound(ButtonPaddingY * scaleFactor)));
        return toolButtonStyleSheet;
    }

} // namespace CommonData

#endif // COMMONDATA_H
