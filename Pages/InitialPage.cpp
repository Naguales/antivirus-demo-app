#include "InitialPage_p.h"

CInitialPagePrivate::CInitialPagePrivate(CInitialPage* qPtr)
    : q_ptr(qPtr)
{
    Q_Q(CInitialPage);

    m_scaleFactor = q->logicalDpiX() / CommonData::LogicalDpiRefValue;
}

CInitialPagePrivate::~CInitialPagePrivate()
{}

void CInitialPagePrivate::init()
{
    Q_Q(CInitialPage);

    createWidgets();
    createMainLayout();
    createConnections();

    q->setLayout(m_mainLayout);
}

void CInitialPagePrivate::createWidgets()
{
    Q_Q(CInitialPage);

    auto createIconToolButton = [&](const QString& iconPath){
        auto iconToolButton = new QToolButton(q);
        iconToolButton->setIcon(QIcon(iconPath));
        iconToolButton->setStyleSheet(CommonData::ToolButtonStyleSheet(m_scaleFactor));
        const int iIconToolButtonFixedSize = qRound(300 * m_scaleFactor);
        iconToolButton->setIconSize(QSize(iIconToolButtonFixedSize, iIconToolButtonFixedSize));
        iconToolButton->setFocusPolicy(Qt::NoFocus);
        return iconToolButton;
    };

    auto createActionPushButton = [&](const QString& buttonText){
        auto actionPushButton = new CGlowedButton(buttonText, CommonData::GlowColor(), q);
        actionPushButton->setStyleSheet(CommonData::SignificantButtonWithHoverStyleSheet(m_scaleFactor));
        actionPushButton->setFocusPolicy(Qt::NoFocus);
        const int iActionPushButtonWidth = qRound(220 * m_scaleFactor);
        actionPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionPushButton->setFixedWidth(iActionPushButtonWidth);
        return actionPushButton;
    };

    m_scanIconToolButton = createIconToolButton(QStringLiteral(":/Resources/ScanInitial"));

    m_scanPushButton = createActionPushButton(QObject::tr("Scan"));
    QFont scanPushButtonFont = m_scanPushButton->font();
    scanPushButtonFont.setPixelSize(qRound(scanPushButtonFont.pixelSize() * 1.2));
    m_scanPushButton->setFont(scanPushButtonFont);
}

void CInitialPagePrivate::createMainLayout()
{
    Q_Q(CInitialPage);

    m_mainLayout = new QVBoxLayout(q);
    m_mainLayout->addStretch(1);
    m_mainLayout->addWidget(m_scanIconToolButton, 0, Qt::AlignCenter);
    m_mainLayout->addStretch(1);
    m_mainLayout->addWidget(m_scanPushButton, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(qRound(70 * m_scaleFactor));
}

void CInitialPagePrivate::createConnections()
{
    Q_Q(CInitialPage);

    QObject::connect(m_scanPushButton, SIGNAL(clicked(bool)), q, SIGNAL(scanningStarted()));
}

CInitialPage::CInitialPage(QWidget* parent)
    : QWidget(parent),
    d_ptr(new CInitialPagePrivate(this))
{
    Q_D(CInitialPage);

    d->init();
}

CInitialPage::~CInitialPage()
{
    // Must define a non-inline destructor in the .cpp file, even if it is empty.
    // Else, a default one will be built in placed where CInitialPagePrivate is only forward declare,
    // leading to error in the destructor of QScopedPointer.
}

CInitialPage::CInitialPage(CInitialPagePrivate& dd, QWidget* parent)
    : QWidget(parent),
    d_ptr(&dd)
{
    Q_D(CInitialPage);

    d->init();
}

#include "moc_InitialPage.cpp"
