#include "formgallerywindow.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>

#include "pages/basicformpage.h"
#include "pages/dateandautocompletepage.h"
#include "pages/validationformpage.h"

FormGalleryWindow::FormGalleryWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
    wireUi();
    setWindowTitle(QStringLiteral("Qt Material 3 Widgets - Form Examples"));
}

FormGalleryWindow::~FormGalleryWindow() = default;

void FormGalleryWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QHBoxLayout(central);

    m_nav = new QListWidget(central);
    m_nav->addItem(QStringLiteral("Basic Form"));
    m_nav->addItem(QStringLiteral("Validation Form"));
    m_nav->addItem(QStringLiteral("Date & Autocomplete"));

    m_stack = new QStackedWidget(central);
    m_stack->addWidget(new BasicFormPage(m_stack));
    m_stack->addWidget(new ValidationFormPage(m_stack));
    m_stack->addWidget(new DateAndAutocompletePage(m_stack));

    layout->addWidget(m_nav, 0);
    layout->addWidget(m_stack, 1);

    setCentralWidget(central);
}

void FormGalleryWindow::wireUi()
{
    connect(m_nav, &QListWidget::currentRowChanged,
            m_stack, &QStackedWidget::setCurrentIndex);

    m_nav->setCurrentRow(0);
}
