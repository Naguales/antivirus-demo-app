#ifndef SCANNERMAINWINDOWPRIVATE_H
#define SCANNERMAINWINDOWPRIVATE_H

#include "ScannerMainWindow.h"
#include "Widgets/SlidingStackedWidget.h"
#include "Pages/InitialPage.h"
#include "Pages/ScanProcessPage.h"
#include "CommonData.h"

#include <map>

class CScannerMainWindowPrivate
{
    Q_DECLARE_PUBLIC(CScannerMainWindow)

protected:
	//! q-pointer.
    CScannerMainWindow* q_ptr;

public:
   CScannerMainWindowPrivate(CScannerMainWindow* qPtr);
    virtual ~CScannerMainWindowPrivate();
	
    void init();

    //! [Private slots]

    CSlidingStackedWidget* m_slidingStackedWidget { nullptr };
    CInitialPage* m_initialPage { nullptr };
    CScanProcessPage* m_scanProcessPage { nullptr };

    double m_scaleFactor { 1.0 };
    std::map<CommonData::EPages, int> m_pages;
    CommonData::EPages m_proposedPage { CommonData::EPages::epInitial };
    CommonData::EPages m_previousPage { CommonData::EPages::epInitial };

private:
    void createWidgets();
    void createConnections();

    void slideToInitialPage();
};

#endif // SCANNERMAINWINDOWPRIVATE_H
