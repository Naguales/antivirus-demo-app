#include "ScannerMainWindow_p.h"
#include "CommonData.h"

#include <QHBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>

CScannerMainWindowPrivate::CScannerMainWindowPrivate(CScannerMainWindow* qPtr)
    : q_ptr(qPtr)
{
    Q_Q(CScannerMainWindow);

    m_scaleFactor = q->logicalDpiX() / CommonData::LogicalDpiRefValue;
}

CScannerMainWindowPrivate::~CScannerMainWindowPrivate()
{}

void CScannerMainWindowPrivate::init()
{
    Q_Q(CScannerMainWindow);

    QPalette palette = QApplication::palette();
    palette.setColor(QPalette::Window, Qt::white);
    QApplication::setPalette(palette);
    q->setWindowIcon(QIcon(":/Resources/Shield"));

    createWidgets();
    createConnections();

    q->setCentralWidget(m_slidingStackedWidget);
    q->setMinimumSize(QSize(qRound(1000 * m_scaleFactor), qRound(618 * m_scaleFactor)));
    q->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // Set minimum size for the Scanner Main Window.
    const QSize preferredMinimumSize = QSize(qRound(1000 * m_scaleFactor), qRound(618 * m_scaleFactor));
    QDesktopWidget* pDesktop = QApplication::desktop();
    auto iWindowScreenNumber = pDesktop->screenNumber(q);
    auto screens = QGuiApplication::screens();
    const QRect availableGeometryRect = iWindowScreenNumber >= 0 && iWindowScreenNumber < screens.size() && screens[iWindowScreenNumber] ? screens[iWindowScreenNumber]->availableGeometry() : QRect();
    const QSize screenAvailableSize(availableGeometryRect.width(), availableGeometryRect.height());
    if (!availableGeometryRect.isNull() && (preferredMinimumSize.width() > screenAvailableSize.width() || qRound(preferredMinimumSize.height() * 1.07) > screenAvailableSize.height())) {
        q->setMinimumSize(screenAvailableSize);
        q->showMaximized();
    } else {
        q->setMinimumSize(preferredMinimumSize);
        q->resize(preferredMinimumSize);
    }
}

void CScannerMainWindowPrivate::createWidgets()
{
    Q_Q(CScannerMainWindow);

    m_slidingStackedWidget = new CSlidingStackedWidget(q);
    m_initialPage = new CInitialPage(q);
    m_scanProcessPage = new CScanProcessPage(q);

    m_pages[CommonData::EPages::epInitial] = m_slidingStackedWidget->addWidget(m_initialPage);
    m_pages[CommonData::EPages::epScanProcess] = m_slidingStackedWidget->addWidget(m_scanProcessPage);
}

void CScannerMainWindowPrivate::createConnections()
{
    Q_Q(CScannerMainWindow);

    QObject::connect(m_initialPage, SIGNAL(scanningStarted()), q, SLOT(startScanning()));
    QObject::connect(m_slidingStackedWidget, &CSlidingStackedWidget::animationFinished, [&](){
        if (m_proposedPage == CommonData::EPages::epScanProcess && m_previousPage == CommonData::EPages::epInitial) {
            m_scanProcessPage->startScanning();
        }
    });
    QObject::connect(m_scanProcessPage, &CScanProcessPage::scanningCancelled, [&](){
        m_previousPage = CommonData::EPages::epScanProcess;
        slideToInitialPage();
    });
    QObject::connect(m_scanProcessPage, &CScanProcessPage::scanningDone, [&](){
        m_previousPage = CommonData::EPages::epScanProcess;
        slideToInitialPage();
    });
}

void CScannerMainWindowPrivate::slideToInitialPage()
{
    auto initialPage = m_pages.find(CommonData::EPages::epInitial);
    if (initialPage != std::end(m_pages) && initialPage->second >= 0 && initialPage->second < m_slidingStackedWidget->count()) {
        m_proposedPage = CommonData::EPages::epInitial;
        m_slidingStackedWidget->slide(initialPage->second);
    }
}

CScannerMainWindow::CScannerMainWindow(QWidget* parent)
    : QMainWindow(parent),
    d_ptr(new CScannerMainWindowPrivate(this))
{
    Q_D(CScannerMainWindow);

    d->init();
}

CScannerMainWindow::~CScannerMainWindow()
{
    // Must define a non-inline destructor in the .cpp file, even if it is empty.
    // Else, a default one will be built in placed where CScannerMainWindowPrivate is only forward declare,
    // leading to error in the destructor of QScopedPointer.
}

CScannerMainWindow::CScannerMainWindow(CScannerMainWindowPrivate& dd, QWidget* parent)
    : QMainWindow(parent),
    d_ptr(&dd)
{
    Q_D(CScannerMainWindow);

    d->init();
}

void CScannerMainWindow::startScanning()
{
    Q_D(CScannerMainWindow);

    auto scanProcessPage = d->m_pages.find(CommonData::EPages::epScanProcess);
    if (scanProcessPage != std::end(d->m_pages) && scanProcessPage->second >= 0 && scanProcessPage->second < d->m_slidingStackedWidget->count()) {
        d->m_proposedPage = CommonData::EPages::epScanProcess;
        d->m_previousPage = CommonData::EPages::epInitial;
        d->m_slidingStackedWidget->slide(scanProcessPage->second);
    }
}

#include "moc_ScannerMainWindow.cpp"
