#include "navigationadvancedpage.h"

#include <QLabel>
#include <QPushButton>
#include <QShortcut>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

NavigationAdvancedPage::NavigationAdvancedPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    auto* title = new QLabel(tr("0.9 — Desktop navigation"), this);
    layout->addWidget(title);

    auto* explanation = new QLabel(
        tr("Breadcrumbs navigate a hierarchy; the command palette provides keyboard-first navigation to the same destinations."),
        this);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    auto* breadcrumb = new QtMaterial::QtMaterialBreadcrumb(this);
    breadcrumb->setItems({
        tr("Workspace"),
        tr("Requirements"),
        tr("REQ-42")
    });
    layout->addWidget(breadcrumb);

    auto* currentContext = new QLabel(tr("Current context: REQ-42"), this);
    layout->addWidget(currentContext);

    connect(
        breadcrumb,
        &QtMaterial::QtMaterialBreadcrumb::activated,
        this,
        [currentContext](int, const QString& text) {
            currentContext->setText(
                QObject::tr("Current context: %1").arg(text));
        });

    auto* model = new QStringListModel({
        tr("Go to Workspace"),
        tr("Go to Requirements"),
        tr("Open REQ-42"),
        tr("Show requirement search")
    }, this);

    auto* palette = new QtMaterial::QtMaterialCommandPalette(this);
    palette->setSourceModel(model);

    connect(
        palette,
        &QtMaterial::QtMaterialCommandPalette::commandActivated,
        this,
        [breadcrumb, currentContext](const QModelIndex& index) {
            switch (index.row()) {
            case 0:
                breadcrumb->setCurrentIndex(0);
                currentContext->setText(QObject::tr("Current context: Workspace"));
                break;
            case 1:
                breadcrumb->setCurrentIndex(1);
                currentContext->setText(QObject::tr("Current context: Requirements"));
                break;
            case 2:
                breadcrumb->setCurrentIndex(2);
                currentContext->setText(QObject::tr("Current context: REQ-42"));
                break;
            case 3:
                currentContext->setText(QObject::tr("Current context: Requirement search"));
                break;
            default:
                break;
            }
        });

    const auto openCommandPalette = [palette]() {
        palette->setQuery(QString());
        palette->show();
        palette->raise();
        palette->activateWindow();
    };

    auto* openPalette =
        new QPushButton(tr("Commands (Ctrl+K)"), this);
    connect(
        openPalette,
        &QPushButton::clicked,
        this,
        openCommandPalette);
    layout->addWidget(openPalette);

    auto* shortcut =
        new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_K), this);
    connect(
        shortcut,
        &QShortcut::activated,
        this,
        openCommandPalette);

    layout->addStretch(1);
}
