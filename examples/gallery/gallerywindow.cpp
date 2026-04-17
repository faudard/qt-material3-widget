#include "gallerywindow.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "pages/buttonspage.h"
#include "pages/inputspage.h"
#include "pages/selectionpage.h"
#include "pages/surfacespage.h"
#include "themecontrols/themetoolbar.h"
GalleryWindow::GalleryWindow(QWidget* parent) : QMainWindow(parent), m_tabs(new QTabWidget(this))
{
    setWindowTitle(QStringLiteral("qt-material3-widgets gallery"));
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->addWidget(new ThemeToolbar(central));
    layout->addWidget(m_tabs, 1);
    m_tabs->addTab(new ButtonsPage(central), QStringLiteral("Buttons"));
    m_tabs->addTab(new SelectionPage(central), QStringLiteral("Selection"));
    m_tabs->addTab(new SurfacesPage(central), QStringLiteral("Surfaces"));
    m_tabs->addTab(new InputsPage(central), QStringLiteral("Inputs"));
    setCentralWidget(central);
}
GalleryWindow::~GalleryWindow() = default;
