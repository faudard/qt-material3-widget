#include "dashboarddemostyle.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QColor>
#include <QFont>
#include <QFrame>
#include <QListView>
#include <QSize>
#include <QSizePolicy>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QWidget>

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"
#include "qtmaterial/widgets/selection/qtmaterialsegmentedbutton.h"

namespace {

QString cssColor(const QColor& color)
{
    return color.name(QColor::HexRgb);
}

QColor color(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance()
        .theme()
        .colorScheme()
        .color(role);
}

class DashboardComboItemDelegate final : public QStyledItemDelegate
{
public:
    explicit DashboardComboItemDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        QSize result = QStyledItemDelegate::sizeHint(option, index);
        result.setHeight(qMax(result.height(), 44));
        return result;
    }
};

void polishCombo(QtMaterial::QtMaterialComboBox* combo)
{
    if (!combo) {
        return;
    }

    combo->setMinimumHeight(42);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    combo->setMaxVisibleItems(8);
    combo->setMinimumWidth(
        qMax(combo->minimumWidth(), combo->sizeHint().width() + 12));

    if (QAbstractItemView* view = combo->view()) {
        view->setFrameShape(QFrame::NoFrame);
        view->setMinimumWidth(qMax(combo->minimumWidth(), 190));

        if (!view->property("dashboardDemoStyled").toBool()) {
            view->setItemDelegate(new DashboardComboItemDelegate(view));
            view->setProperty("dashboardDemoStyled", true);

            if (auto* list = qobject_cast<QListView*>(view)) {
                list->setUniformItemSizes(true);
                list->setSpacing(2);
            }
        }
    }
}

void polishButton(QtMaterial::QtMaterialTextButton* button)
{
    if (!button) {
        return;
    }

    button->setDensity(QtMaterial::Density::Comfortable);
    button->setMinimumHeight(40);
    button->setMaximumHeight(44);
    QSizePolicy policy = button->sizePolicy();
    policy.setVerticalPolicy(QSizePolicy::Fixed);
    button->setSizePolicy(policy);

    QFont font = button->font();
    font.setWeight(QFont::DemiBold);
    button->setFont(font);
}

} // namespace

namespace DashboardDemoStyle {

void apply(QWidget* root)
{
    if (!root) {
        return;
    }

    const QColor surface = color(QtMaterial::ColorRole::Surface);
    const QColor surfaceLow = color(QtMaterial::ColorRole::SurfaceContainerLow);
    const QColor surfaceHigh = color(QtMaterial::ColorRole::SurfaceContainerHigh);
    const QColor onSurface = color(QtMaterial::ColorRole::OnSurface);
    const QColor onSurfaceVariant = color(QtMaterial::ColorRole::OnSurfaceVariant);
    const QColor outline = color(QtMaterial::ColorRole::OutlineVariant);
    const QColor primary = color(QtMaterial::ColorRole::Primary);
    const QColor primaryContainer = color(QtMaterial::ColorRole::PrimaryContainer);
    const QColor onPrimaryContainer = color(QtMaterial::ColorRole::OnPrimaryContainer);

    root->setStyleSheet(QStringLiteral(
        "QComboBox#qtmaterial_combo_box {"
        " background:%1;"
        " color:%2;"
        " border:1px solid %3;"
        " border-radius:14px;"
        " padding:7px 34px 7px 14px;"
        " min-height:28px;"
        " selection-background-color:%4;"
        " selection-color:%5;"
        " }"
        "QComboBox#qtmaterial_combo_box:hover {"
        " border-color:%6;"
        " background:%7;"
        " }"
        "QComboBox#qtmaterial_combo_box:focus {"
        " border:2px solid %6;"
        " padding:6px 33px 6px 13px;"
        " }"
        "QComboBox#qtmaterial_combo_box::drop-down {"
        " subcontrol-origin:padding;"
        " subcontrol-position:top right;"
        " width:32px;"
        " border:0;"
        " background:transparent;"
        " }"
        "QComboBox#qtmaterial_combo_box::down-arrow {"
        " width:8px;"
        " height:8px;"
        " }"
        "QComboBox#qtmaterial_combo_box QAbstractItemView {"
        " background:%1;"
        " color:%2;"
        " border:1px solid %3;"
        " border-radius:16px;"
        " padding:8px;"
        " outline:0;"
        " selection-background-color:%4;"
        " selection-color:%5;"
        " }"
        "QComboBox#qtmaterial_combo_box QAbstractItemView::item {"
        " min-height:36px;"
        " padding:5px 12px;"
        " border-radius:10px;"
        " }"
        "QComboBox#qtmaterial_combo_box QAbstractItemView::item:hover {"
        " background:%8;"
        " }"
        "QComboBox#qtmaterial_combo_box QAbstractItemView::item:selected {"
        " background:%4;"
        " color:%5;"
        " }"
        "QToolButton[dashboardPill=true] {"
        " background:%8;"
        " color:%2;"
        " border:1px solid %3;"
        " border-radius:12px;"
        " padding:6px 12px;"
        " font-weight:600;"
        " }"
        "QToolButton[dashboardPill=true]:hover {"
        " background:%4;"
        " color:%5;"
        " border-color:%6;"
        " }")
        .arg(cssColor(surface))
        .arg(cssColor(onSurface))
        .arg(cssColor(outline))
        .arg(cssColor(primaryContainer))
        .arg(cssColor(onPrimaryContainer))
        .arg(cssColor(primary))
        .arg(cssColor(surfaceLow))
        .arg(cssColor(surfaceHigh))
        .arg(cssColor(onSurfaceVariant)));

    polishControls(root);
}

void polishControls(QWidget* root)
{
    if (!root) {
        return;
    }

    const auto combos =
        root->findChildren<QtMaterial::QtMaterialComboBox*>();
    for (QtMaterial::QtMaterialComboBox* combo : combos) {
        polishCombo(combo);
    }

    const auto buttons =
        root->findChildren<QtMaterial::QtMaterialTextButton*>();
    for (QtMaterial::QtMaterialTextButton* button : buttons) {
        polishButton(button);
    }

    const auto segmented =
        root->findChildren<QtMaterial::QtMaterialSegmentedButton*>();
    for (QtMaterial::QtMaterialSegmentedButton* control : segmented) {
        control->setMinimumHeight(42);
        control->setMinimumWidth(
            qMax(control->minimumWidth(), control->sizeHint().width()));
    }
}

} // namespace DashboardDemoStyle
