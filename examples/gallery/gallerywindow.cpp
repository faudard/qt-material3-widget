#include "gallerywindow.h"

#include "gallerycatalog.h"
#include "pages/advanceddatapage.h"
#include "pages/advancedinputspage.h"
#include "pages/buttonspage.h"
#include "pages/datapage.h"
#include "pages/inputspage.h"
#include "pages/navigationadvancedpage.h"
#include "pages/navigationpage.h"
#include "pages/progressindicatorspage.h"
#include "pages/selectionpage.h"
#include "pages/surfacespage.h"
#include "themecontrols/themetoolbar.h"

#include <QAbstractButton>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTabBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

namespace {

QWidget* scrollablePage(QWidget* page, QWidget* parent)
{
    auto* scroll = new QScrollArea(parent);
    scroll->setWidget(page);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    const int minimumPageHeight = page->minimumSizeHint().height();
    if (minimumPageHeight > 0) {
        page->setMinimumHeight(minimumPageHeight);
    }
    scroll->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    return scroll;
}

QString normalizedClassName(const QWidget* widget)
{
    if (!widget || !widget->metaObject()) {
        return QString();
    }
    return QString::fromLatin1(widget->metaObject()->className());
}

bool matchesWidgetType(const QWidget* widget, const QString& widgetType)
{
    const QString className = normalizedClassName(widget);
    return className == widgetType || className.endsWith(QStringLiteral("::") + widgetType);
}

QString uiObjectName(const QString& id)
{
    QString result = id;
    result.replace(QLatin1Char('.'), QLatin1Char('_'));
    result.replace(QLatin1Char('-'), QLatin1Char('_'));
    return result;
}

} // namespace

GalleryWindow::GalleryWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_tabs(new QTabWidget(this))
{
    setWindowTitle(QStringLiteral("Qt Material 3 — Gallery 2.0"));
    resize(1280, 820);

    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);
    root->addWidget(new ThemeToolbar(central));

    auto* body = new QHBoxLayout;
    body->setSpacing(10);
    root->addLayout(body, 1);

    auto* navigationFrame = new QFrame(central);
    navigationFrame->setFrameShape(QFrame::StyledPanel);
    navigationFrame->setMinimumWidth(240);
    navigationFrame->setMaximumWidth(320);
    auto* navigationLayout = new QVBoxLayout(navigationFrame);
    auto* navigationTitle = new QLabel(QStringLiteral("Components"), navigationFrame);
    QFont navigationFont = navigationTitle->font();
    navigationFont.setBold(true);
    navigationTitle->setFont(navigationFont);
    navigationLayout->addWidget(navigationTitle);

    m_search = new QLineEdit(navigationFrame);
    m_search->setPlaceholderText(QStringLiteral("Search 60 components, routes, types…"));
    m_search->setClearButtonEnabled(true);
    m_search->setAccessibleName(QStringLiteral("Search Gallery components"));
    navigationLayout->addWidget(m_search);

    m_navigation = new QTreeWidget(navigationFrame);
    m_navigation->setHeaderHidden(true);
    m_navigation->setRootIsDecorated(true);
    m_navigation->setAccessibleName(QStringLiteral("Gallery component navigation"));
    navigationLayout->addWidget(m_navigation, 1);
    body->addWidget(navigationFrame);

    m_tabs->addTab(scrollablePage(new ButtonsPage(this), m_tabs), QStringLiteral("Buttons"));
    m_tabs->addTab(scrollablePage(new SelectionPage(this), m_tabs), QStringLiteral("Selection"));
    m_tabs->addTab(scrollablePage(new SurfacesPage(this), m_tabs), QStringLiteral("Surfaces"));
    m_tabs->addTab(scrollablePage(new InputsPage(this), m_tabs), QStringLiteral("Inputs"));
    m_tabs->addTab(scrollablePage(new NavigationPage(this), m_tabs), QStringLiteral("Navigation"));
    m_tabs->addTab(scrollablePage(new DataPage(this), m_tabs), QStringLiteral("Data"));
    m_tabs->addTab(scrollablePage(new AdvancedInputsPage(this), m_tabs), QStringLiteral("Advanced inputs"));
    m_tabs->addTab(scrollablePage(new AdvancedDataPage(this), m_tabs), QStringLiteral("Advanced data"));
    m_tabs->addTab(scrollablePage(new NavigationAdvancedPage(this), m_tabs), QStringLiteral("Advanced navigation"));
    m_tabs->addTab(scrollablePage(new QtMaterialGallery::ProgressIndicatorsPage(this), m_tabs), QStringLiteral("Progress"));
    m_tabs->tabBar()->hide();
    body->addWidget(m_tabs, 1);

    auto* inspector = new QFrame(central);
    inspector->setFrameShape(QFrame::StyledPanel);
    inspector->setMinimumWidth(300);
    inspector->setMaximumWidth(390);
    auto* inspectorLayout = new QVBoxLayout(inspector);

    auto* inspectorTitle = new QLabel(QStringLiteral("Live inspector"), inspector);
    QFont inspectorFont = inspectorTitle->font();
    inspectorFont.setBold(true);
    inspectorTitle->setFont(inspectorFont);
    inspectorLayout->addWidget(inspectorTitle);

    m_route = new QLineEdit(inspector);
    m_route->setPlaceholderText(QStringLiteral("/buttons/filled"));
    m_route->setAccessibleName(QStringLiteral("Gallery deep link"));
    inspectorLayout->addWidget(m_route);

    m_state = new QComboBox(inspector);
    m_state->addItems({
        QStringLiteral("Default"),
        QStringLiteral("Hover"),
        QStringLiteral("Focus"),
        QStringLiteral("Pressed"),
        QStringLiteral("Selected"),
        QStringLiteral("Disabled"),
        QStringLiteral("Error")
    });
    m_state->setAccessibleName(QStringLiteral("State preview"));
    inspectorLayout->addWidget(m_state);

    m_properties = new QTableWidget(inspector);
    m_properties->setColumnCount(2);
    m_properties->setHorizontalHeaderLabels({QStringLiteral("Property"), QStringLiteral("Value")});
    m_properties->horizontalHeader()->setStretchLastSection(true);
    m_properties->verticalHeader()->hide();
    m_properties->setAlternatingRowColors(true);
    m_properties->setAccessibleName(QStringLiteral("Live component properties"));
    inspectorLayout->addWidget(m_properties, 1);

    auto* snippets = new QTabWidget(inspector);
    m_cppSnippet = new QPlainTextEdit(snippets);
    m_cppSnippet->setReadOnly(true);
    m_uiSnippet = new QPlainTextEdit(snippets);
    m_uiSnippet->setReadOnly(true);
    snippets->addTab(m_cppSnippet, QStringLiteral("C++"));
    snippets->addTab(m_uiSnippet, QStringLiteral(".ui"));
    inspectorLayout->addWidget(snippets);

    auto* copyRow = new QHBoxLayout;
    auto* copyCpp = new QPushButton(QStringLiteral("Copy C++"), inspector);
    auto* copyUi = new QPushButton(QStringLiteral("Copy .ui"), inspector);
    copyRow->addWidget(copyCpp);
    copyRow->addWidget(copyUi);
    inspectorLayout->addLayout(copyRow);
    body->addWidget(inspector);

    setCentralWidget(central);

    connect(m_search, &QLineEdit::textChanged, this, [this](const QString& text) {
        rebuildNavigation(text);
    });
    connect(m_navigation, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem* item, int) {
        if (!item) return;
        const QVariant value = item->data(0, Qt::UserRole);
        if (!value.isValid()) return;
        const int index = value.toInt();
        const auto& catalog = galleryComponentCatalog();
        if (index >= 0 && index < catalog.size()) navigateToComponent(catalog.at(index));
    });
    connect(m_navigation, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int) {
        if (!item) return;
        const QVariant value = item->data(0, Qt::UserRole);
        if (!value.isValid()) return;
        const int index = value.toInt();
        const auto& catalog = galleryComponentCatalog();
        if (index >= 0 && index < catalog.size()) navigateToComponent(catalog.at(index));
    });
    connect(m_route, &QLineEdit::returnPressed, this, [this]() {
        navigateToRoute(m_route->text());
    });
    connect(m_state, &QComboBox::currentTextChanged, this, [this](const QString& state) {
        applyStatePreview(state);
    });
    connect(m_properties, &QTableWidget::cellChanged, this, &GalleryWindow::writeEditedProperty);
    connect(copyCpp, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_cppSnippet->toPlainText());
    });
    connect(copyUi, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_uiSnippet->toPlainText());
    });

    rebuildNavigation();
    if (!galleryComponentCatalog().isEmpty()) {
        navigateToComponent(galleryComponentCatalog().first());
    }
}

GalleryWindow::~GalleryWindow() = default;

void GalleryWindow::rebuildNavigation(const QString& filter)
{
    const QString needle = filter.trimmed().toLower();
    m_navigation->clear();

    QMap<QString, QTreeWidgetItem*> families;
    const auto& catalog = galleryComponentCatalog();
    for (int i = 0; i < catalog.size(); ++i) {
        const auto& entry = catalog.at(i);
        const QString haystack =
            (entry.name + QLatin1Char(' ') + entry.family + QLatin1Char(' ')
             + entry.route + QLatin1Char(' ') + entry.widgetType).toLower();
        if (!needle.isEmpty() && !haystack.contains(needle)) {
            continue;
        }

        QTreeWidgetItem* family = families.value(entry.family, nullptr);
        if (!family) {
            family = new QTreeWidgetItem(m_navigation, {entry.family});
            QFont font = family->font(0);
            font.setBold(true);
            family->setFont(0, font);
            families.insert(entry.family, family);
        }

        auto* item = new QTreeWidgetItem(family, {entry.name});
        item->setToolTip(0, entry.route + QStringLiteral("\n") + entry.widgetType);
        item->setData(0, Qt::UserRole, i);
    }

    m_navigation->expandAll();
}

bool GalleryWindow::navigateToRoute(const QString& requestedRoute)
{
    QString route = requestedRoute.trimmed();
    if (route.startsWith(QStringLiteral("gallery://"))) {
        route = route.mid(QStringLiteral("gallery://").size());
        if (!route.startsWith(QLatin1Char('/'))) route.prepend(QLatin1Char('/'));
    }
    if (!route.startsWith(QLatin1Char('/'))) route.prepend(QLatin1Char('/'));

    const auto& catalog = galleryComponentCatalog();
    for (const auto& entry : catalog) {
        if (entry.route == route) {
            navigateToComponent(entry);
            return true;
        }
    }
    for (const auto& entry : catalog) {
        if (entry.route.startsWith(route)) {
            navigateToComponent(entry);
            return true;
        }
    }
    return false;
}

QWidget* GalleryWindow::findComponentWidget(const QString& widgetType, int* tabIndex) const
{
    for (int i = 0; i < m_tabs->count(); ++i) {
        QWidget* page = m_tabs->widget(i);
        if (matchesWidgetType(page, widgetType)) {
            if (tabIndex) *tabIndex = i;
            return page;
        }
        const auto widgets = page->findChildren<QWidget*>();
        for (QWidget* widget : widgets) {
            if (matchesWidgetType(widget, widgetType)) {
                if (tabIndex) *tabIndex = i;
                return widget;
            }
        }
    }
    if (tabIndex) *tabIndex = -1;
    return nullptr;
}

void GalleryWindow::navigateToComponent(const GalleryComponentEntry& entry)
{
    int tabIndex = -1;
    m_target = findComponentWidget(entry.widgetType, &tabIndex);
    if (tabIndex < 0) {
        tabIndex = galleryTabForRoute(entry.route);
    }
    if (tabIndex >= 0 && tabIndex < m_tabs->count()) {
        m_tabs->setCurrentIndex(tabIndex);
    }

    if (m_target) {
        if (auto* scroll = qobject_cast<QScrollArea*>(m_tabs->widget(tabIndex))) {
            scroll->ensureWidgetVisible(m_target, 24, 24);
        }
    }

    m_currentComponentId = entry.id;
    m_route->setText(entry.route);
    m_state->setCurrentIndex(0);

    const QString cpp = QStringLiteral(
        "#include <%1>\n\n"
        "auto* component = new QtMaterial::%2(parent);\n"
        "// Configure the component, then add it to your layout.\n")
        .arg(entry.publicHeader, entry.widgetType);
    m_cppSnippet->setPlainText(cpp);

    const QString objectName = uiObjectName(entry.id);
    const QString ui = QStringLiteral(
        "<widget class=\"%1\" name=\"%2\"/>\n\n"
        "<customwidgets>\n"
        " <customwidget>\n"
        "  <class>%1</class>\n"
        "  <extends>QWidget</extends>\n"
        "  <header>%3</header>\n"
        " </customwidget>\n"
        "</customwidgets>\n")
        .arg(entry.widgetType, objectName, entry.publicHeader);
    m_uiSnippet->setPlainText(ui);

    updateInspector();
}

void GalleryWindow::updateInspector()
{
    m_properties->blockSignals(true);
    m_properties->setRowCount(0);

    if (!m_target) {
        m_properties->blockSignals(false);
        return;
    }

    const QMetaObject* meta = m_target->metaObject();
    int shown = 0;
    for (int i = meta->propertyCount() - 1; i >= 0 && shown < 28; --i) {
        const QMetaProperty property = meta->property(i);
        if (!property.isReadable()) continue;
        const QByteArray name = property.name();
        if (name == "geometry" || name == "pos" || name == "size"
            || name == "minimumSize" || name == "maximumSize") {
            continue;
        }

        const QVariant value = property.read(m_target.data());
        const int row = m_properties->rowCount();
        m_properties->insertRow(row);

        auto* nameItem = new QTableWidgetItem(QString::fromLatin1(name));
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        m_properties->setItem(row, 0, nameItem);

        QString displayValue;
        if (property.isEnumType()) {
            const QMetaEnum enumerator = property.enumerator();
            const char* key = enumerator.valueToKey(value.toInt());
            displayValue = key ? QString::fromLatin1(key) : value.toString();
        } else {
            displayValue = value.toString();
        }
        auto* valueItem = new QTableWidgetItem(displayValue);
        if (!property.isWritable()) {
            valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
        }
        m_properties->setItem(row, 1, valueItem);
        ++shown;
    }

    m_properties->resizeColumnToContents(0);
    m_properties->blockSignals(false);
}

void GalleryWindow::writeEditedProperty(int row, int column)
{
    if (column != 1 || !m_target) return;
    QTableWidgetItem* nameItem = m_properties->item(row, 0);
    QTableWidgetItem* valueItem = m_properties->item(row, 1);
    if (!nameItem || !valueItem) return;

    const QByteArray name = nameItem->text().toLatin1();
    const QMetaObject* meta = m_target->metaObject();
    const int propertyIndex = meta->indexOfProperty(name.constData());
    if (propertyIndex < 0) return;

    const QMetaProperty property = meta->property(propertyIndex);
    if (!property.isWritable()) return;

    QVariant value;
    if (property.isEnumType()) {
        const QByteArray key = valueItem->text().toLatin1();
        bool ok = false;
        int enumValue = property.enumerator().keyToValue(key.constData(), &ok);
        if (!ok) enumValue = valueItem->text().toInt(&ok);
        if (!ok) return;
        value = enumValue;
    } else if (property.userType() == QMetaType::Bool) {
        const QString text = valueItem->text().trimmed().toLower();
        value = (text == QStringLiteral("true") || text == QStringLiteral("1")
                 || text == QStringLiteral("yes") || text == QStringLiteral("on"));
    } else {
        value = valueItem->text();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (!value.convert(property.metaType())) {
            return;
        }
#else
        if (!value.convert(property.userType())) {
            return;
        }
#endif
    }

    property.write(m_target.data(), value);
    m_target->update();
    updateInspector();
}

void GalleryWindow::applyStatePreview(const QString& state)
{
    if (!m_target) return;

    m_target->setEnabled(true);
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(m_target.data(), &leave);
    if (auto* button = qobject_cast<QAbstractButton*>(m_target.data())) {
        button->setDown(false);
    }
    const QMetaObject* meta = m_target->metaObject();
    const int errorTextIndex = meta->indexOfProperty("errorText");
    if (errorTextIndex >= 0) {
        const QMetaProperty property = meta->property(errorTextIndex);
        if (property.isWritable()) property.write(m_target.data(), QString());
    }
    const int errorIndex = meta->indexOfProperty("error");
    if (errorIndex >= 0) {
        const QMetaProperty property = meta->property(errorIndex);
        if (property.isWritable()) property.write(m_target.data(), false);
    }

    if (state == QStringLiteral("Hover")) {
        QEvent enter(QEvent::Enter);
        QApplication::sendEvent(m_target.data(), &enter);
    } else if (state == QStringLiteral("Focus")) {
        m_target->setFocus(Qt::OtherFocusReason);
    } else if (state == QStringLiteral("Pressed")) {
        if (auto* button = qobject_cast<QAbstractButton*>(m_target.data())) {
            button->setDown(true);
        }
    } else if (state == QStringLiteral("Selected")) {
        if (auto* button = qobject_cast<QAbstractButton*>(m_target.data())) {
            if (button->isCheckable()) button->setChecked(true);
        } else {
            const int checkedIndex = meta->indexOfProperty("checked");
            if (checkedIndex >= 0 && meta->property(checkedIndex).isWritable()) {
                meta->property(checkedIndex).write(m_target.data(), true);
            }
        }
    } else if (state == QStringLiteral("Disabled")) {
        m_target->setEnabled(false);
    } else if (state == QStringLiteral("Error")) {
        if (errorTextIndex >= 0 && meta->property(errorTextIndex).isWritable()) {
            meta->property(errorTextIndex).write(m_target.data(), QStringLiteral("Example error"));
        } else if (errorIndex >= 0 && meta->property(errorIndex).isWritable()) {
            meta->property(errorIndex).write(m_target.data(), true);
        }
    }

    m_target->update();
    updateInspector();
}
