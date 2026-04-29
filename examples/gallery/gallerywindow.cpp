#include "gallerywindow.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "pages/buttonspage.h"
#include "pages/datapage.h"
#include "pages/inputspage.h"
#include "pages/navigationpage.h"
#include "pages/selectionpage.h"
#include "pages/surfacespage.h"
#include "pages/progressindicatorspage.h"
#include "pages/navigationadvancedpage.h"
#include "pages/advancedinputspage.h"
#include "pages/advanceddatapage.h"
#include "themecontrols/themetoolbar.h"
#include <QAbstractScrollArea>
#include <QFrame>
#include <QScrollArea>
#include <QSizePolicy>

namespace {

QWidget* scrollablePage(QWidget* page, QWidget* parent)
{
    auto* scroll = new QScrollArea(parent);
    scroll->setWidget(page);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    page->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    return scroll;
}

}

GalleryWindow::GalleryWindow(QWidget* parent) : QMainWindow(parent), m_tabs(new QTabWidget(this))
{
    setWindowTitle(QStringLiteral("qt-material3-widgets gallery"));
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->addWidget(new ThemeToolbar(central));
    layout->addWidget(m_tabs, 1);
    m_tabs->addTab(scrollablePage(new ButtonsPage(this), m_tabs), QStringLiteral("Buttons"));
    m_tabs->addTab(scrollablePage(new SelectionPage(this), m_tabs), QStringLiteral("Selection"));
    m_tabs->addTab(scrollablePage(new SurfacesPage(this), m_tabs), QStringLiteral("Surfaces"));
    m_tabs->addTab(scrollablePage(new InputsPage(this), m_tabs), QStringLiteral("Inputs"));
    m_tabs->addTab(scrollablePage(new NavigationPage(this), m_tabs), QStringLiteral("Navigation"));
    m_tabs->addTab(scrollablePage(new DataPage(this), m_tabs), QStringLiteral("Data"));
    m_tabs->addTab(scrollablePage(new AdvancedInputsPage(this), m_tabs),QStringLiteral("Advanced inputs"));
    m_tabs->addTab(scrollablePage(new AdvancedDataPage(this), m_tabs),QStringLiteral("Advanced data"));
    m_tabs->addTab(scrollablePage(new NavigationAdvancedPage(this), m_tabs),QStringLiteral("Advanced navigation"));
    m_tabs->addTab(scrollablePage(new QtMaterialGallery::ProgressIndicatorsPage(this), m_tabs), QStringLiteral("Progress"));

    setCentralWidget(central);
}
GalleryWindow::~GalleryWindow() = default;
