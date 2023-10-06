#include "ScanProcessPage_p.h"

#include <QStyle>
#include <QGridLayout>
#include <QLocale>

#include <algorithm>

CScanProcessPagePrivate::CScanProcessPagePrivate(CScanProcessPage* qPtr)
    : q_ptr(qPtr)
{
    Q_Q(CScanProcessPage);

    m_scaleFactor = q->logicalDpiX() / CommonData::LogicalDpiRefValue;
}

CScanProcessPagePrivate::~CScanProcessPagePrivate()
{
    m_scanningMovie.reset(nullptr);
}

void CScanProcessPagePrivate::init()
{
    Q_Q(CScanProcessPage);

    createWidgets();
    createMainLayout();
    createConnections();

    q->setLayout(m_mainLayout);
}

void CScanProcessPagePrivate::q_onTimerTick()
{
    Q_Q(CScanProcessPage);

    constexpr unsigned int intemsCountPerStep = 480;
    ++m_elapsedTimerTickCount;
    m_scannedItems += intemsCountPerStep;
    m_currentStageScannedItems += intemsCountPerStep;

    auto currentTime = std::chrono::high_resolution_clock::now();
    int elapsedTimeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastMeasurementTime).count());
    m_lastMeasurementTime = currentTime;
    m_elapsedTime = m_elapsedTime.addMSecs(elapsedTimeMs);

    m_scannedItemsLabel->setText(QLocale::system().toString(m_scannedItems));
    m_elapsedTimeLabel->setText(elapsedTimeToString(m_elapsedTime));
    if (!(m_elapsedTimerTickCount % 3)) {
        m_scanInProgressDotCount = m_scanInProgressDotCount >= m_scanInProgressMaxDotCount ? 0 : ++m_scanInProgressDotCount;
        QString scanInProgressText = m_scanInProgressDotCount ? QString("%1%2").arg(m_scanInProgressText).arg('.', static_cast<int>(m_scanInProgressDotCount), QLatin1Char('.')) : m_scanInProgressText;
        m_scanningProcessStatusLabel->setText(scanInProgressText);
    }

    if (m_scanningStages.find(m_currentScanningStage) != std::end(m_scanningStages)) {
        if (m_currentStageScannedItems >= std::get<3>(m_scanningStages[m_currentScanningStage])) {
            if (std::get<2>(m_scanningStages[m_currentScanningStage])) {
                std::get<2>(m_scanningStages[m_currentScanningStage])->setValue(100);
            }
            m_currentStageScannedItems -= std::get<3>(m_scanningStages[m_currentScanningStage]);
            finishStage(std::get<0>(m_scanningStages[m_currentScanningStage]), std::get<1>(m_scanningStages[m_currentScanningStage]), std::get<2>(m_scanningStages[m_currentScanningStage]));
            auto nextStage = m_scanningStages.find(std::get<4>(m_scanningStages[m_currentScanningStage]));
            if (nextStage != std::end(m_scanningStages)) {
                startStage(std::get<0>(nextStage->second), std::get<1>(nextStage->second), std::get<2>(nextStage->second), nextStage->first);
            } else {
                emit q->scanningFinished();
            }
        } else {
            if (std::get<2>(m_scanningStages[m_currentScanningStage])) {
                std::get<2>(m_scanningStages[m_currentScanningStage])->setValue(std::get<3>(m_scanningStages[m_currentScanningStage]) > 0
                                                                                ? qRound(m_currentStageScannedItems * 360.0 / std::get<3>(m_scanningStages[m_currentScanningStage])) : 360);
            }
        }
    }
}

void CScanProcessPagePrivate::q_dropScanningResults()
{
    Q_Q(CScanProcessPage);

    m_elapsedTimer->stop();
    m_scanningMovie->stop();
    dropScanningProcessState();
    emit q->scanningCancelled();
}

void CScanProcessPagePrivate::q_finishScanning()
{
    m_currentScanningStage = CommonData::EScanningStage::essIdle;
    m_elapsedTimer->stop();
    m_scanningMovie->stop();
    m_scanFinishDateTime = QDateTime::currentDateTime();
    m_pauseResumeScanningPushButton->setVisible(false);
    m_cancelScanningPushButton->setVisible(false);
    m_donePushButton->setVisible(true);
    m_scanningProcessMetricsWidget->setVisible(false);
    m_scanningSummaryReportWidget->setVisible(true);
    breakScanningMovie();

    // Fill scan summary report widgets.
    m_scanningFullDateTimeSummaryReportLabel->setText(QLocale::system().toString(m_scanFinishDateTime));
    m_scanTimeSummaryReportLabel->setText(elapsedTimeToString(m_elapsedTime));
    m_scannedItemsSummaryReportLabel->setText(QLocale::system().toString(m_scannedItems));
    m_threatsDetectedLabel->setText(QString::number(m_detections));
    m_potentiallyUnwantedProgramsDetectedLabel->setText(QString::number(m_potentiallyUnwantedProgramsDetected));
    m_detectionsResolvedLabel->setText(QString::number(m_detectionsResolved));
    m_detectionsIgnoredLabel->setText(QString::number(m_detectionsIgnored));
    QIcon scanSuccessIcon(":/Resources/ScanSuccess");
    m_scanningMovieLabel->setPixmap(scanSuccessIcon.pixmap(qRound(300 * m_scaleFactor)));
}

void CScanProcessPagePrivate::q_doneScanning()
{
    Q_Q(CScanProcessPage);

    dropScanningProcessState();
    emit q->scanningDone();
}

void CScanProcessPagePrivate::createWidgets()
{
    Q_Q(CScanProcessPage);

    auto createStatusToolButton = [&](const QString& iconPath = QStringLiteral(":/Resources/Point")){
        auto statusToolButton = new QToolButton(q);
        statusToolButton->setIcon(QIcon(iconPath));
        statusToolButton->setStyleSheet(CommonData::ToolButtonStyleSheet(m_scaleFactor));
        const int iStatusToolButtonFixedSize = qRound(32 * m_scaleFactor);
        statusToolButton->setIconSize(QSize(iStatusToolButtonFixedSize, iStatusToolButtonFixedSize));
        statusToolButton->setFocusPolicy(Qt::NoFocus);
        return statusToolButton;
    };

    auto createProcessStageInfoPushButton = [&](const QString& buttonText){
        auto processStageInfoPushButton = new QPushButton(buttonText, q);
        processStageInfoPushButton->setStyleSheet(CommonData::InertButtonStyleSheet(m_scaleFactor));
        processStageInfoPushButton->setFocusPolicy(Qt::NoFocus);
        const int defaultFrameWidth = qRound(q->style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * m_scaleFactor);
        const int iProcessStageInfoPushButtonWidth = q->fontMetrics().boundingRect(processStageInfoPushButton->text()).width() + defaultFrameWidth * 2 + qRound(22 * m_scaleFactor) * 2;
        processStageInfoPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        processStageInfoPushButton->setFixedWidth(iProcessStageInfoPushButtonWidth);
        return processStageInfoPushButton;
    };

    auto createCirularDial = [&](){
        auto cirularDial = new CCirularDial(q);
        cirularDial->setStartAngle(90);
        cirularDial->setFinishAngle(-360);
        const int iCirularDialFixedSize = qRound(32 * m_scaleFactor);
        cirularDial->setFixedSize(iCirularDialFixedSize, iCirularDialFixedSize);
        cirularDial->setValue(0);
        cirularDial->setVisible(false);
        return cirularDial;
    };

    auto createHeaderTextLabel = [&](const QString& labelText, QWidget* parent) {
        auto headerTextLabel = new QLabel(labelText, parent);
        headerTextLabel->setStyleSheet(QString("QLabel { margin: %1px; }").arg(QString::number(qRound(7 * m_scaleFactor))));
        QFont headerTextLabelFont = headerTextLabel->font();
        headerTextLabelFont.setPixelSize(qRound(headerTextLabelFont.pixelSize() * 1.6));
        headerTextLabel->setFont(headerTextLabelFont);
        return headerTextLabel;
    };

    auto createTextLabel = [&](const QString& labelText, QWidget* parent) {
        auto textLabel = new QLabel(labelText, parent);
        textLabel->setStyleSheet(QString("QLabel { color: rgb(59, 59, 59); margin: %1px; }").arg(QString::number(qRound(7 * m_scaleFactor))));
        return textLabel;
    };

    auto createCountLabel = [&](const QString& labelCount, QWidget* parent) {
        auto countLabel = new QLabel(labelCount, parent);
        countLabel->setStyleSheet(QString("QLabel { font-weight: bold; margin: %1px;}").arg(QString::number(qRound(7 * m_scaleFactor))));
        return countLabel;
    };

    auto createActionPushButton = [&](const QString& buttonText, const QString& styleSheet){
        auto actionPushButton = new CGlowedButton(buttonText, CommonData::GlowColor(), q);
        actionPushButton->setStyleSheet(styleSheet);
        actionPushButton->setFocusPolicy(Qt::NoFocus);
        const int iActionPushButtonWidth = qRound(120 * m_scaleFactor);
        actionPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionPushButton->setFixedWidth(iActionPushButtonWidth);
        return actionPushButton;
    };

    auto createHorizontslLine = [&](QWidget* parent){
        auto horizontalLine = new QFrame(parent);
        horizontalLine->setFrameStyle(QFrame::HLine | QFrame::Plain);
        horizontalLine->setLineWidth(qRound(1 * m_scaleFactor));
        horizontalLine->setStyleSheet("QFrame { color: rgb(220, 220, 220); } ");
        return horizontalLine;
    };

    m_scanningBrowsersStatusToolButton = createStatusToolButton();
    m_scanningBrowsersPushButton = createProcessStageInfoPushButton(QObject::tr("Scanning browsers"));
    m_scanningBrowsersCirularDial = createCirularDial();

    m_checkingForSoftwareUpdatesStatusToolButton = createStatusToolButton();
    m_checkingForSoftwareUpdatesPushButton = createProcessStageInfoPushButton(QObject::tr("Checking for software updates"));
    m_checkingForSoftwareUpdatesCirularDial = createCirularDial();

    m_scanningMemoryStatusToolButton = createStatusToolButton();
    m_scanningMemoryPushButton = createProcessStageInfoPushButton(QObject::tr("Scanning memory"));
    m_scanningMemoryCirularDial = createCirularDial();

    m_scanningStartupAppsStatusToolButton = createStatusToolButton();
    m_scanningStartupAppsPushButton = createProcessStageInfoPushButton(QObject::tr("Scanning startup apps"));
    m_scanningStartupAppsCirularDial = createCirularDial();

    m_scanningFileSystemStatusToolButton = createStatusToolButton();
    m_scanningFileSystemPushButton = createProcessStageInfoPushButton(QObject::tr("Scanning file system"));
    m_scanningFileSystemCirularDial = createCirularDial();

    // Scanning process widgets.
    m_scanningProcessMetricsWidget = new QWidget(q);
    m_scanningProcessStatusLabel = createHeaderTextLabel(m_scanInProgressText, m_scanningProcessMetricsWidget);

    m_scannedItemsTextLabel = createTextLabel(QObject::tr("Scanned items"), m_scanningProcessMetricsWidget);
    m_scannedItemsLabel = createCountLabel(QLocale::system().toString(m_scannedItems), m_scanningProcessMetricsWidget);
    m_elapsedTimeTextLabel = createTextLabel(QObject::tr("Elapsed time"), m_scanningProcessMetricsWidget);
    m_elapsedTimeLabel = createCountLabel(elapsedTimeToString(m_elapsedTime), m_scanningProcessMetricsWidget);

    m_detectionsTextLabel = createTextLabel(QObject::tr("Detections"), m_scanningProcessMetricsWidget);
    m_detectionsLabel = createCountLabel(QString::number(m_detections), m_scanningProcessMetricsWidget);
    m_detectionsHorizontalLine = createHorizontslLine(m_scanningProcessMetricsWidget);

    m_scanningMovieLabel = new QLabel(q);

    m_pauseResumeScanningPushButton = createActionPushButton(QObject::tr("Pause"), CommonData::ActionPushButtonStyleSheet(m_scaleFactor));
    m_pauseResumeScanningPushButton->setDisabled(true);
    m_cancelScanningPushButton = createActionPushButton(QObject::tr("Cancel"), CommonData::ActionPushButtonStyleSheet(m_scaleFactor));
    m_cancelScanningPushButton->setDisabled(true);

    // Scan summary report widgets.
    m_scanningSummaryReportWidget = new QWidget(q);
    m_scanningSummaryReportWidget->setVisible(false);
    m_scanningSummaryReportLabel = createHeaderTextLabel(QObject::tr("Scan summary"), m_scanningSummaryReportWidget);
    m_scanningFullDateTimeSummaryReportLabel = createTextLabel(QLocale::system().toString(QDateTime::currentDateTime()), m_scanningSummaryReportWidget);
    m_scanningFullDateTimeSummaryReportLabel->setStyleSheet("QLabel { color: rgb(191, 191, 191); }");

    m_scanTimeSummaryReportTextLabel = createTextLabel(QObject::tr("Scan time"), m_scanningSummaryReportWidget);
    m_scanTimeSummaryReportLabel = createCountLabel(elapsedTimeToString(m_elapsedTime), m_scanningSummaryReportWidget);
    m_scannedItemsSummaryReportTextLabel = createTextLabel(QObject::tr("Scanned items"), m_scanningSummaryReportWidget);
    m_scannedItemsSummaryReportLabel = createCountLabel(QLocale::system().toString(m_scannedItems), m_scanningSummaryReportWidget);
    m_scannedItemsSummaryReportHorizontalLine = createHorizontslLine(m_scanningSummaryReportWidget);

    m_threatsDetectedTextLabel = createTextLabel(QObject::tr("Detected threats"), m_scanningSummaryReportWidget);
    m_threatsDetectedLabel = createCountLabel(QString::number(m_detections), m_scanningSummaryReportWidget);
    m_potentiallyUnwantedProgramsDetectedTextLabel = createTextLabel(QObject::tr("Detected potentially unwanted programs"), m_scanningSummaryReportWidget);
    m_potentiallyUnwantedProgramsDetectedLabel = createCountLabel(QString::number(m_potentiallyUnwantedProgramsDetected), m_scanningSummaryReportWidget);
    m_potentiallyUnwantedProgramsDetectedHorizontalLine = createHorizontslLine(m_scanningSummaryReportWidget);

    m_detectionsResolvedTextLabel = createTextLabel(QObject::tr("Resolved detections"), m_scanningSummaryReportWidget);
    m_detectionsResolvedLabel = createCountLabel(QString::number(m_detectionsResolved), m_scanningSummaryReportWidget);
    m_detectionsIgnoredTextLabel = createTextLabel(QObject::tr("Ignored detections"), m_scanningSummaryReportWidget);
    m_detectionsIgnoredLabel = createCountLabel(QString::number(m_detectionsIgnored), m_scanningSummaryReportWidget);

    m_donePushButton = createActionPushButton(QObject::tr("Done"), CommonData::SignificantButtonWithHoverStyleSheet(m_scaleFactor));
    m_donePushButton->setFixedWidth(qRound(160 * m_scaleFactor));
    m_donePushButton->setVisible(false);

    m_elapsedTimer = new QTimer(q);
    m_elapsedTimer->setInterval(300);

    m_scanningStages = {
        { CommonData::EScanningStage::essScanningBrowsers,
          { m_scanningBrowsersStatusToolButton, m_scanningBrowsersPushButton, m_scanningBrowsersCirularDial, 0, CommonData::EScanningStage::essCheckingForSoftwareUpdates } },

        { CommonData::EScanningStage::essCheckingForSoftwareUpdates,
          { m_checkingForSoftwareUpdatesStatusToolButton, m_checkingForSoftwareUpdatesPushButton, m_checkingForSoftwareUpdatesCirularDial, 0, CommonData::EScanningStage::essScanningMemory } },

        { CommonData::EScanningStage::essScanningMemory,
          { m_scanningMemoryStatusToolButton, m_scanningMemoryPushButton, m_scanningMemoryCirularDial, 0, CommonData::EScanningStage::essScanningStartupApps } },

        { CommonData::EScanningStage::essScanningStartupApps,
          { m_scanningStartupAppsStatusToolButton, m_scanningStartupAppsPushButton, m_scanningStartupAppsCirularDial, 0, CommonData::EScanningStage::essScanningFileSystem } },

        { CommonData::EScanningStage::essScanningFileSystem,
          { m_scanningFileSystemStatusToolButton, m_scanningFileSystemPushButton, m_scanningFileSystemCirularDial, 0, CommonData::EScanningStage::essIdle } },
    };
}

void CScanProcessPagePrivate::createMainLayout()
{
    Q_Q(CScanProcessPage);

    m_mainLayout = new QVBoxLayout(q);

    auto scanningStagesHBoxLayout = new QHBoxLayout;

    auto scanningBrowsersHBoxLayout = new QHBoxLayout;
    scanningBrowsersHBoxLayout->addWidget(m_scanningBrowsersStatusToolButton);
    scanningBrowsersHBoxLayout->addWidget(m_scanningBrowsersCirularDial);
    auto scanningBrowsersVBoxLayout = new QVBoxLayout;
    scanningBrowsersVBoxLayout->addLayout(scanningBrowsersHBoxLayout);
    scanningBrowsersVBoxLayout->addWidget(m_scanningBrowsersPushButton, 0, Qt::AlignCenter);

    auto checkingForSoftwareUpdatesHBoxLayout = new QHBoxLayout;
    checkingForSoftwareUpdatesHBoxLayout->addWidget(m_checkingForSoftwareUpdatesStatusToolButton);
    checkingForSoftwareUpdatesHBoxLayout->addWidget(m_checkingForSoftwareUpdatesCirularDial);
    auto checkingForSoftwareUpdatesVBoxLayout = new QVBoxLayout;
    checkingForSoftwareUpdatesVBoxLayout->addLayout(checkingForSoftwareUpdatesHBoxLayout);
    checkingForSoftwareUpdatesVBoxLayout->addWidget(m_checkingForSoftwareUpdatesPushButton, 0, Qt::AlignCenter);

    auto scanningMemoryHBoxLayout = new QHBoxLayout;
    scanningMemoryHBoxLayout->addWidget(m_scanningMemoryStatusToolButton);
    scanningMemoryHBoxLayout->addWidget(m_scanningMemoryCirularDial);
    auto scanningMemoryVBoxLayout = new QVBoxLayout;
    scanningMemoryVBoxLayout->addLayout(scanningMemoryHBoxLayout);
    scanningMemoryVBoxLayout->addWidget(m_scanningMemoryPushButton, 0, Qt::AlignCenter);

    auto scanningStartupAppsHBoxLayout = new QHBoxLayout;
    scanningStartupAppsHBoxLayout->addWidget(m_scanningStartupAppsStatusToolButton);
    scanningStartupAppsHBoxLayout->addWidget(m_scanningStartupAppsCirularDial);
    auto scanningStartupAppsVBoxLayout = new QVBoxLayout;
    scanningStartupAppsVBoxLayout->addLayout(scanningStartupAppsHBoxLayout);
    scanningStartupAppsVBoxLayout->addWidget(m_scanningStartupAppsPushButton, 0, Qt::AlignCenter);

    auto scanningFileSystemHBoxLayout = new QHBoxLayout;
    scanningFileSystemHBoxLayout->addWidget(m_scanningFileSystemStatusToolButton);
    scanningFileSystemHBoxLayout->addWidget(m_scanningFileSystemCirularDial);
    auto scanningFileSystemVBoxLayout = new QVBoxLayout;
    scanningFileSystemVBoxLayout->addLayout(scanningFileSystemHBoxLayout);
    scanningFileSystemVBoxLayout->addWidget(m_scanningFileSystemPushButton, 0, Qt::AlignCenter);

    scanningStagesHBoxLayout->addLayout(scanningBrowsersVBoxLayout);
    scanningStagesHBoxLayout->addStretch(1);
    scanningStagesHBoxLayout->addLayout(checkingForSoftwareUpdatesVBoxLayout);
    scanningStagesHBoxLayout->addStretch(1);
    scanningStagesHBoxLayout->addLayout(scanningMemoryVBoxLayout);
    scanningStagesHBoxLayout->addStretch(1);
    scanningStagesHBoxLayout->addLayout(scanningStartupAppsVBoxLayout);
    scanningStagesHBoxLayout->addStretch(1);
    scanningStagesHBoxLayout->addLayout(scanningFileSystemVBoxLayout);

    // Scanning process widgets.
    auto scanningMetricsGridLayout = new QGridLayout;
    scanningMetricsGridLayout->addWidget(m_scannedItemsTextLabel, 0, 0, Qt::AlignLeft);
    scanningMetricsGridLayout->addWidget(m_scannedItemsLabel, 0, 1, Qt::AlignRight);
    scanningMetricsGridLayout->addWidget(m_elapsedTimeTextLabel, 1, 0, Qt::AlignLeft);
    scanningMetricsGridLayout->addWidget(m_elapsedTimeLabel, 1, 1, Qt::AlignRight);
    scanningMetricsGridLayout->addWidget(m_detectionsHorizontalLine, 2, 0, 1, 2);
    scanningMetricsGridLayout->addWidget(m_detectionsTextLabel, 3, 0, Qt::AlignLeft);
    scanningMetricsGridLayout->addWidget(m_detectionsLabel, 3, 1, Qt::AlignRight);

    auto scanningMetricsVBoxLayout = new QVBoxLayout;
    scanningMetricsVBoxLayout->addWidget(m_scanningProcessStatusLabel);
    scanningMetricsVBoxLayout->addStretch(1);
    scanningMetricsVBoxLayout->addLayout(scanningMetricsGridLayout);
    scanningMetricsVBoxLayout->addStretch(1);
    m_scanningProcessMetricsWidget->setLayout(scanningMetricsVBoxLayout);

    //! Scan summary report widgets.
    auto scanningSummaryReportGridLayout = new QGridLayout;
    scanningSummaryReportGridLayout->addWidget(m_scanTimeSummaryReportTextLabel, 0, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_scanTimeSummaryReportLabel, 0, 1, Qt::AlignRight);
    scanningSummaryReportGridLayout->addWidget(m_scannedItemsSummaryReportTextLabel, 1, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_scannedItemsSummaryReportLabel, 1, 1, Qt::AlignRight);
    scanningSummaryReportGridLayout->addWidget(m_scannedItemsSummaryReportHorizontalLine, 2, 0, 1, 2);
    scanningSummaryReportGridLayout->addWidget(m_threatsDetectedTextLabel, 3, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_threatsDetectedLabel, 3, 1, Qt::AlignRight);
    scanningSummaryReportGridLayout->addWidget(m_potentiallyUnwantedProgramsDetectedTextLabel, 4, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_potentiallyUnwantedProgramsDetectedLabel, 4, 1, Qt::AlignRight);
    scanningSummaryReportGridLayout->addWidget(m_potentiallyUnwantedProgramsDetectedHorizontalLine, 5, 0, 1, 2);
    scanningSummaryReportGridLayout->addWidget(m_detectionsResolvedTextLabel, 6, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_detectionsResolvedLabel, 6, 1, Qt::AlignRight);
    scanningSummaryReportGridLayout->addWidget(m_detectionsIgnoredTextLabel, 7, 0, Qt::AlignLeft);
    scanningSummaryReportGridLayout->addWidget(m_detectionsIgnoredLabel, 7, 1, Qt::AlignRight);

    auto scanningSummaryReportVBoxLayout = new QVBoxLayout;
    scanningSummaryReportVBoxLayout->addWidget(m_scanningSummaryReportLabel, 0, Qt::AlignCenter);
    scanningSummaryReportVBoxLayout->addWidget(m_scanningFullDateTimeSummaryReportLabel, 0, Qt::AlignCenter);
    scanningSummaryReportVBoxLayout->addStretch(1);
    scanningSummaryReportVBoxLayout->addLayout(scanningSummaryReportGridLayout);
    scanningSummaryReportVBoxLayout->addStretch(1);
    m_scanningSummaryReportWidget->setLayout(scanningSummaryReportVBoxLayout);

    auto scanningMetricsHBoxLayout = new QHBoxLayout;
    scanningMetricsHBoxLayout->addWidget(m_scanningMovieLabel, 0, Qt::AlignCenter);
    scanningMetricsHBoxLayout->addSpacing(qRound(70 * m_scaleFactor));
    scanningMetricsHBoxLayout->addWidget(m_scanningProcessMetricsWidget);
    scanningMetricsHBoxLayout->addWidget(m_scanningSummaryReportWidget);

    auto actionButtonsHBoxLayout = new QHBoxLayout;
    actionButtonsHBoxLayout->addStretch(1);
    actionButtonsHBoxLayout->addWidget(m_pauseResumeScanningPushButton);
    actionButtonsHBoxLayout->addWidget(m_cancelScanningPushButton);
    actionButtonsHBoxLayout->addWidget(m_donePushButton);
    actionButtonsHBoxLayout->addStretch(1);

    m_mainLayout->addLayout(scanningStagesHBoxLayout);
    m_mainLayout->addStretch(1);
    m_mainLayout->addLayout(scanningMetricsHBoxLayout);
    m_mainLayout->addStretch(1);
    m_mainLayout->addLayout(actionButtonsHBoxLayout);
    m_mainLayout->addSpacing(qRound(30 * m_scaleFactor));
}

void CScanProcessPagePrivate::createConnections()
{
    Q_Q(CScanProcessPage);

    QObject::connect(m_elapsedTimer, SIGNAL(timeout()), q, SLOT(q_onTimerTick()));
    QObject::connect(m_pauseResumeScanningPushButton, &QPushButton::clicked, [&](){
        if (m_isScanningWasPaused) {
            resumeScanning();
        } else {
            pauseScanning();
        }
        m_isScanningWasPaused = !m_isScanningWasPaused;
    });
    QObject::connect(m_cancelScanningPushButton, SIGNAL(clicked(bool)), q, SLOT(q_dropScanningResults()));
    QObject::connect(q, SIGNAL(scanningFinished()), q, SLOT(q_finishScanning()));
    QObject::connect(m_donePushButton, SIGNAL(clicked(bool)), q, SLOT(q_doneScanning()));
}

QString CScanProcessPagePrivate::elapsedTimeToString(const QTime& time)
{
    QString sTime;
    if (time.hour() > 0) {
        sTime.append(QString("%1 h ").arg(time.hour()));
    }
    if (time.minute() > 0) {
        sTime.append(QString("%1 min ").arg(time.minute()));
    }
    sTime.append(QString("%1 s").arg(time.second()));
    return sTime;
}

void CScanProcessPagePrivate::createScanningMovie()
{
    m_scanningMovie.reset(new QMovie(":/Resources/Scanning"));
    m_scanningMovie->setScaledSize(QSize(qRound(300 * m_scaleFactor), qRound(300 * m_scaleFactor)));
    if (m_scanningMovieLabel) {
        m_scanningMovieLabel->setMovie(m_scanningMovie.data());
    }
}

void CScanProcessPagePrivate::breakScanningMovie()
{
    m_scanningMovie.reset(nullptr);
    if (m_scanningMovieLabel) {
        m_scanningMovieLabel->setMovie(nullptr);
    }
}

void CScanProcessPagePrivate::setFakeData()
{
    m_totalItemsCount = 120000;

    qulonglong scanningBrowsersTotalItemsCount { 0 },
               checkingForSoftwareUpdatesTotalItemsCount { 0 },
               scanningMemoryTotalItemsCount { 0 },
               scanningStartupAppsTotalItemsCount { 0 };

    if (m_scanningStages.find(CommonData::EScanningStage::essScanningBrowsers) != std::end(m_scanningStages)) {
        scanningBrowsersTotalItemsCount = qRound(m_totalItemsCount * 0.07);
        std::get<3>(m_scanningStages[CommonData::EScanningStage::essScanningBrowsers]) = scanningBrowsersTotalItemsCount;
    }
    if (m_scanningStages.find(CommonData::EScanningStage::essCheckingForSoftwareUpdates) != std::end(m_scanningStages)) {
        checkingForSoftwareUpdatesTotalItemsCount = qRound(m_totalItemsCount * 0.07);
        std::get<3>(m_scanningStages[CommonData::EScanningStage::essCheckingForSoftwareUpdates]) = checkingForSoftwareUpdatesTotalItemsCount;
    }
    if (m_scanningStages.find(CommonData::EScanningStage::essScanningMemory) != std::end(m_scanningStages)) {
        scanningMemoryTotalItemsCount = qRound(m_totalItemsCount * 0.09);
        std::get<3>(m_scanningStages[CommonData::EScanningStage::essScanningMemory]) = scanningMemoryTotalItemsCount;
    }
    if (m_scanningStages.find(CommonData::EScanningStage::essScanningStartupApps) != std::end(m_scanningStages)) {
        scanningStartupAppsTotalItemsCount = qRound(m_totalItemsCount * 0.07);
        std::get<3>(m_scanningStages[CommonData::EScanningStage::essScanningStartupApps]) = scanningStartupAppsTotalItemsCount;
    }
    if (m_scanningStages.find(CommonData::EScanningStage::essScanningFileSystem) != std::end(m_scanningStages)) {
        std::get<3>(m_scanningStages[CommonData::EScanningStage::essScanningFileSystem]) = m_totalItemsCount - scanningBrowsersTotalItemsCount
                - checkingForSoftwareUpdatesTotalItemsCount - scanningMemoryTotalItemsCount - scanningStartupAppsTotalItemsCount;
    }
}

void CScanProcessPagePrivate::pauseScanning()
{
    m_elapsedTimer->stop();
    if (m_scanningMovie) {
        m_scanningMovie->stop();
    }
    m_pauseResumeScanningPushButton->setText(QObject::tr("Resume"));
    m_scanningProcessStatusLabel->setText(QObject::tr("Scanning paused"));
}

void CScanProcessPagePrivate::resumeScanning()
{
    m_pauseResumeScanningPushButton->setText(QObject::tr("Pause"));
    QString scanInProgressText = m_scanInProgressDotCount ? QString("%1%2").arg(m_scanInProgressText).arg('.', static_cast<int>(m_scanInProgressDotCount), QLatin1Char('.')) : m_scanInProgressText;
    m_scanningProcessStatusLabel->setText(scanInProgressText);
    m_lastMeasurementTime = std::chrono::high_resolution_clock::now();
    m_elapsedTimer->start();
    if (m_scanningMovie) {
        m_scanningMovie->start();
    }
}

void CScanProcessPagePrivate::startStage(QToolButton* statusToolButton, QPushButton* infoPushButton, CCirularDial* cirularDial, CommonData::EScanningStage scanningStage)
{
    if (statusToolButton) {
        statusToolButton->setVisible(false);
    }
    if (cirularDial) {
        cirularDial->setVisible(true);
    }
    if (infoPushButton) {
        infoPushButton->setStyleSheet(CommonData::SignificantButtonStyleSheet(m_scaleFactor));
    }
    m_currentScanningStage = scanningStage;
}

void CScanProcessPagePrivate::finishStage(QToolButton* statusToolButton, QPushButton* infoPushButton, CCirularDial* cirularDial)
{
    if (statusToolButton) {
        statusToolButton->setVisible(true);
        statusToolButton->setIcon(QIcon(":/Resources/Success"));
    }
    if (cirularDial) {
        cirularDial->setVisible(false);
    }
    if (infoPushButton) {
        infoPushButton->setStyleSheet(CommonData::InertButtonStyleSheet(m_scaleFactor));
    }
}

void CScanProcessPagePrivate::dropScanningStagesStates()
{
    m_currentScanningStage = CommonData::EScanningStage::essIdle;
    for (auto& scanningStage : m_scanningStages) {
        if (std::get<0>(scanningStage.second)) {
            std::get<0>(scanningStage.second)->setIcon(QIcon(":/Resources/Point"));
            std::get<0>(scanningStage.second)->setVisible(true);
        }
        if (std::get<1>(scanningStage.second)) {
            std::get<1>(scanningStage.second)->setStyleSheet(CommonData::InertButtonStyleSheet(m_scaleFactor));
        }
        if (std::get<2>(scanningStage.second)) {
            std::get<2>(scanningStage.second)->setValue(0);
            std::get<2>(scanningStage.second)->setVisible(false);
        }
        if (std::get<3>(scanningStage.second)) {
            std::get<3>(scanningStage.second) = 0;
        }
    }
}

void CScanProcessPagePrivate::dropScanningMetrics()
{
    m_isScanningWasPaused = false;
    m_elapsedTime = QTime(0, 0);
    m_elapsedTimerTickCount = 0;
    m_scannedItems = 0;
    m_currentStageScannedItems = 0;
    m_detections = 0;
    m_totalItemsCount = 0;
    m_scanInProgressDotCount = 0;
    m_potentiallyUnwantedProgramsDetected = 0;
    m_detectionsResolved = 0;
    m_detectionsIgnored = 0;
}

void CScanProcessPagePrivate::dropScanningUIState()
{
    m_pauseResumeScanningPushButton->setVisible(true);
    m_pauseResumeScanningPushButton->setEnabled(true);
    m_cancelScanningPushButton->setEnabled(true);
    m_cancelScanningPushButton->setVisible(true);
    m_scanningSummaryReportWidget->setVisible(false);
    m_donePushButton->setVisible(false);
    m_scanningProcessMetricsWidget->setVisible(true);

    m_scannedItemsLabel->setText(QLocale::system().toString(m_scannedItems));
    m_elapsedTimeLabel->setText(elapsedTimeToString(m_elapsedTime));
    m_detectionsLabel->setText(QString::number(m_detections));
    m_scanningProcessStatusLabel->setText(m_scanInProgressText);
    breakScanningMovie();
}

void CScanProcessPagePrivate::dropScanningProcessState()
{
    dropScanningMetrics();
    dropScanningStagesStates();
    dropScanningUIState();
}

CScanProcessPage::CScanProcessPage(QWidget* parent)
    : QWidget(parent),
    d_ptr(new CScanProcessPagePrivate(this))
{
    Q_D(CScanProcessPage);

    d->init();
}

CScanProcessPage::~CScanProcessPage()
{
    // Must define a non-inline destructor in the .cpp file, even if it is empty.
    // Else, a default one will be built in placed where CScanProcessPagePrivate is only forward declare,
    // leading to error in the destructor of QScopedPointer.
}

CScanProcessPage::CScanProcessPage(CScanProcessPagePrivate& dd, QWidget* parent)
    : QWidget(parent),
    d_ptr(&dd)
{
    Q_D(CScanProcessPage);

    d->init();
}

void CScanProcessPage::startScanning()
{
    Q_D(CScanProcessPage);

    d->dropScanningProcessState();
    d->createScanningMovie();
    d->setFakeData();
    d->m_lastMeasurementTime = std::chrono::high_resolution_clock::now();
    d->m_elapsedTimer->start();
    if (d->m_scanningMovie) {
        d->m_scanningMovie->start();
    }

    auto initialStage = d->m_scanningStages.find(CommonData::EScanningStage::essScanningBrowsers);
    if (initialStage != std::end(d->m_scanningStages)) {
        d->startStage(std::get<0>(initialStage->second), std::get<1>(initialStage->second), std::get<2>(initialStage->second), initialStage->first);
    }
}

#include "moc_ScanProcessPage.cpp"
