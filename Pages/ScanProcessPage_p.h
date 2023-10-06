#ifndef SCANPROCESSPAGEPRIVATE_H
#define SCANPROCESSPAGEPRIVATE_H

#include "ScanProcessPage.h"
#include "Widgets/CircularDial.h"
#include "Widgets/GlowedButton.h"
#include "CommonData.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QMovie>
#include <QFrame>
#include <QVBoxLayout>
#include <QTime>
#include <QDateTime>
#include <QTimer>

#include <chrono>
#include <tuple>
#include <map>

class CScanProcessPagePrivate
{
    Q_DECLARE_PUBLIC(CScanProcessPage)

protected:
	//! q-pointer.
    CScanProcessPage* q_ptr;

public:
    CScanProcessPagePrivate(CScanProcessPage* qPtr);
    virtual ~CScanProcessPagePrivate();
	
    void init();

    //! [Private slots]
    void q_onTimerTick();
    void q_dropScanningResults();
    void q_finishScanning();
    void q_doneScanning();

    QToolButton* m_scanningBrowsersStatusToolButton { nullptr };
    QPushButton* m_scanningBrowsersPushButton { nullptr };
    CCirularDial* m_scanningBrowsersCirularDial { nullptr };

    QToolButton* m_checkingForSoftwareUpdatesStatusToolButton { nullptr };
    QPushButton* m_checkingForSoftwareUpdatesPushButton { nullptr };
    CCirularDial* m_checkingForSoftwareUpdatesCirularDial { nullptr };

    QToolButton* m_scanningMemoryStatusToolButton { nullptr };
    QPushButton* m_scanningMemoryPushButton { nullptr };
    CCirularDial* m_scanningMemoryCirularDial { nullptr };

    QToolButton* m_scanningStartupAppsStatusToolButton { nullptr };
    QPushButton* m_scanningStartupAppsPushButton { nullptr };
    CCirularDial* m_scanningStartupAppsCirularDial { nullptr };

    QToolButton* m_scanningFileSystemStatusToolButton { nullptr };
    QPushButton* m_scanningFileSystemPushButton { nullptr };
    CCirularDial* m_scanningFileSystemCirularDial { nullptr };

    //! Scanning process widgets.
    QWidget* m_scanningProcessMetricsWidget { nullptr };
    QLabel* m_scanningProcessStatusLabel { nullptr };
    QLabel* m_scannedItemsTextLabel { nullptr };
    QLabel* m_scannedItemsLabel { nullptr };
    QLabel* m_elapsedTimeTextLabel { nullptr };
    QLabel* m_elapsedTimeLabel { nullptr };
    QLabel* m_detectionsTextLabel { nullptr };
    QLabel* m_detectionsLabel { nullptr };
    QFrame* m_detectionsHorizontalLine { nullptr };

    CGlowedButton* m_pauseResumeScanningPushButton { nullptr };
    CGlowedButton* m_cancelScanningPushButton { nullptr };

    //! Scan summary report widgets.
    QWidget* m_scanningSummaryReportWidget { nullptr };
    QLabel* m_scanningSummaryReportLabel { nullptr };
    QLabel* m_scanningFullDateTimeSummaryReportLabel { nullptr };
    QLabel* m_scanTimeSummaryReportTextLabel { nullptr };
    QLabel* m_scanTimeSummaryReportLabel { nullptr };
    QLabel* m_scannedItemsSummaryReportTextLabel { nullptr };
    QLabel* m_scannedItemsSummaryReportLabel { nullptr };
    QFrame* m_scannedItemsSummaryReportHorizontalLine { nullptr };
    QLabel* m_threatsDetectedTextLabel { nullptr };
    QLabel* m_threatsDetectedLabel { nullptr };
    QLabel* m_potentiallyUnwantedProgramsDetectedTextLabel { nullptr };
    QLabel* m_potentiallyUnwantedProgramsDetectedLabel { nullptr };
    QFrame* m_potentiallyUnwantedProgramsDetectedHorizontalLine { nullptr };
    QLabel* m_detectionsResolvedTextLabel { nullptr };
    QLabel* m_detectionsResolvedLabel { nullptr };
    QLabel* m_detectionsIgnoredTextLabel { nullptr };
    QLabel* m_detectionsIgnoredLabel { nullptr };

    CGlowedButton* m_donePushButton { nullptr };

    QScopedPointer<QMovie> m_scanningMovie { nullptr };
    QLabel* m_scanningMovieLabel { nullptr };

    QVBoxLayout* m_mainLayout { nullptr };

    QTimer* m_elapsedTimer { nullptr };

    QString m_scanInProgressText { QObject::tr("Scan in progress") };
    QString m_scanFullDateTimeFormat { "MM/dd/yy h:mm:ss ap" };
    QTime m_elapsedTime { 0, 0 };
    QDateTime m_scanFinishDateTime;
    double m_scaleFactor { 1.0 };
    qulonglong m_totalItemsCount { 0 };
    qulonglong m_scannedItems { 0 };
    qulonglong m_currentStageScannedItems { 0 };
    qulonglong m_elapsedTimerTickCount { 0 };
    ulong m_detections { 0 };
    ulong m_potentiallyUnwantedProgramsDetected { 0 };
    ulong m_detectionsResolved { 0 };
    ulong m_detectionsIgnored { 0 };
    CommonData::EScanningStage m_currentScanningStage { CommonData::EScanningStage::essIdle };
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastMeasurementTime;
    bool m_isScanningWasPaused { false };
    unsigned char m_scanInProgressMaxDotCount { 3 };
    unsigned char m_scanInProgressDotCount { 0 };

    //! Contains scanning stages data.
    //! In value:
    //!     qulonglong - total items count on a stage.
    //!     CommonData::EScanningStage - the next stage.
    std::map<CommonData::EScanningStage, std::tuple<QToolButton*, QPushButton*, CCirularDial*, qulonglong, CommonData::EScanningStage>> m_scanningStages;

private:
    void createWidgets();
    void createMainLayout();
    void createConnections();

    QString elapsedTimeToString(const QTime&);
    void createScanningMovie();
    void breakScanningMovie();

    void setFakeData();
    void startScanning();
    void pauseScanning();
    void resumeScanning();

    void startStage(QToolButton* statusToolButton, QPushButton* infoPushButton, CCirularDial* cirularDial, CommonData::EScanningStage scanningStage);
    void finishStage(QToolButton* statusToolButton, QPushButton* infoPushButton, CCirularDial* cirularDial);

    void dropScanningStagesStates();
    void dropScanningMetrics();
    void dropScanningUIState();
    void dropScanningProcessState();
};

#endif // SCANPROCESSPAGEPRIVATE_H
