#ifndef INITIALPAGEPRIVATE_H
#define INITIALPAGEPRIVATE_H

#include "InitialPage.h"
#include "Widgets/GlowedButton.h"
#include "CommonData.h"

#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

class CInitialPagePrivate
{
    Q_DECLARE_PUBLIC(CInitialPage)

protected:
	//! q-pointer.
    CInitialPage* q_ptr;

public:
    CInitialPagePrivate(CInitialPage* qPtr);
    virtual ~CInitialPagePrivate();
	
    void init();

    QToolButton* m_scanIconToolButton { nullptr };
    CGlowedButton* m_scanPushButton { nullptr };

    QVBoxLayout* m_mainLayout { nullptr };

    double m_scaleFactor { 1.0 };

private:
    void createWidgets();
    void createMainLayout();
    void createConnections();
};

#endif // INITIALPAGEPRIVATE_H
