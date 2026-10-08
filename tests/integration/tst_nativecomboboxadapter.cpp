#include <QtTest/QtTest>

#include <QAbstractItemView>
#include <QComboBox>
#include <QImage>
#include <QLineEdit>
#include <QPainter>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QStyle>
#include <QStyleOptionComboBox>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialcomboboxadapter.h"

using namespace QtMaterial;

namespace {

class ExposedComboBox final : public QComboBox
{
public:
    using QComboBox::initStyleOption;
};

class TestDelegate final : public QStyledItemDelegate
{
public:
    explicit TestDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }
};

QImage renderCombo(
    QWidget& widget,
    const QSize& size)
{
    widget.resize(size);
    widget.ensurePolished();

    QImage image(
        size,
        QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    widget.render(&painter);
    painter.end();
    return image;
}

} // namespace

class tst_NativeComboBoxAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesModelViewDelegateAndSignals();
    void preservesEditableLineEdit();
    void materialSubControlsFollowRtl();
    void densityPropertyIsLive();
    void adaptsTreeAndHonorsOptOut();
    void editableAndNonEditableRender();
};

void tst_NativeComboBoxAdapter::
    preservesModelViewDelegateAndSignals()
{
    ExposedComboBox combo;
    QStyle* originalStyle = combo.style();

    QStandardItemModel model(3, 1, &combo);
    model.setData(model.index(0, 0), QStringLiteral("One"));
    model.setData(model.index(1, 0), QStringLiteral("Two"));
    model.setData(model.index(2, 0), QStringLiteral("Three"));
    combo.setModel(&model);

    auto* delegate = new TestDelegate(&combo);
    combo.setItemDelegate(delegate);

    QAbstractItemView* originalView = combo.view();
    QAbstractItemModel* originalModel = combo.model();

    QtMaterialComboBoxAdapter::apply(
        &combo,
        Density::Default);

    QVERIFY(
        QtMaterialComboBoxAdapter::isApplied(
            &combo));
    QVERIFY(combo.style() != originalStyle);
    QCOMPARE(combo.model(), originalModel);
    QCOMPARE(combo.view(), originalView);
    QCOMPARE(combo.itemDelegate(), delegate);

    QSignalSpy changed(
        &combo,
        QOverload<int>::of(
            &QComboBox::currentIndexChanged));
    combo.setCurrentIndex(2);
    QCOMPARE(combo.currentIndex(), 2);
    QCOMPARE(changed.count(), 1);

    QtMaterialComboBoxAdapter::remove(&combo);
    QVERIFY(
        !QtMaterialComboBoxAdapter::isApplied(
            &combo));
    QCOMPARE(combo.style(), originalStyle);
    QCOMPARE(combo.model(), originalModel);
    QCOMPARE(combo.view(), originalView);
    QCOMPARE(combo.itemDelegate(), delegate);
}

void tst_NativeComboBoxAdapter::
    preservesEditableLineEdit()
{
    ExposedComboBox combo;
    combo.setEditable(true);
    combo.addItems({
        QStringLiteral("One"),
        QStringLiteral("Two")
    });
    combo.setEditText(QStringLiteral("Custom"));

    QLineEdit* edit = combo.lineEdit();
    QVERIFY(edit);

    QtMaterialComboBoxAdapter::apply(
        &combo,
        Density::Comfortable);

    QVERIFY(combo.isEditable());
    QCOMPARE(combo.lineEdit(), edit);
    QCOMPARE(combo.currentText(), QStringLiteral("Custom"));

    combo.setEditText(QStringLiteral("Still native"));
    QCOMPARE(combo.lineEdit(), edit);
    QCOMPARE(
        combo.currentText(),
        QStringLiteral("Still native"));
}

void tst_NativeComboBoxAdapter::
    materialSubControlsFollowRtl()
{
    ExposedComboBox combo;
    combo.addItems({
        QStringLiteral("Alpha"),
        QStringLiteral("Beta")
    });
    combo.resize(240, 48);
    QtMaterialComboBoxAdapter::apply(&combo);

    QStyleOptionComboBox option;
    combo.initStyleOption(&option);

    const QRect ltrArrow =
        combo.style()->subControlRect(
            QStyle::CC_ComboBox,
            &option,
            QStyle::SC_ComboBoxArrow,
            &combo);
    const QRect ltrEdit =
        combo.style()->subControlRect(
            QStyle::CC_ComboBox,
            &option,
            QStyle::SC_ComboBoxEditField,
            &combo);

    QVERIFY(ltrArrow.center().x() > combo.width() / 2);
    QVERIFY(ltrEdit.center().x() < ltrArrow.center().x());

    combo.setLayoutDirection(Qt::RightToLeft);
    combo.initStyleOption(&option);

    const QRect rtlArrow =
        combo.style()->subControlRect(
            QStyle::CC_ComboBox,
            &option,
            QStyle::SC_ComboBoxArrow,
            &combo);
    const QRect rtlEdit =
        combo.style()->subControlRect(
            QStyle::CC_ComboBox,
            &option,
            QStyle::SC_ComboBoxEditField,
            &combo);

    QVERIFY(rtlArrow.center().x() < combo.width() / 2);
    QVERIFY(rtlEdit.center().x() > rtlArrow.center().x());

    QCOMPARE(
        int(combo.style()->hitTestComplexControl(
            QStyle::CC_ComboBox,
            &option,
            rtlArrow.center(),
            &combo)),
        int(QStyle::SC_ComboBoxArrow));
}

void tst_NativeComboBoxAdapter::
    densityPropertyIsLive()
{
    ExposedComboBox combo;
    QtMaterialComboBoxAdapter::apply(&combo);

    combo.setProperty(
        QtMaterialComboBoxAdapter::
            densityPropertyName(),
        QStringLiteral("compact"));
    QCOMPARE(
        int(QtMaterialComboBoxAdapter::density(
            &combo)),
        int(Density::Compact));

    QtMaterialComboBoxAdapter::setDensity(
        &combo,
        Density::Comfortable);
    QCOMPARE(
        int(QtMaterialComboBoxAdapter::density(
            &combo)),
        int(Density::Comfortable));
}

void tst_NativeComboBoxAdapter::
    adaptsTreeAndHonorsOptOut()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* first = new QComboBox(&root);
    auto* second = new QComboBox(&root);
    layout->addWidget(first);
    layout->addWidget(second);

    QtMaterialComboBoxAdapter::setOptOut(
        second,
        true);

    QCOMPARE(
        QtMaterialComboBoxAdapter::
            applyToDescendants(
                &root,
                Density::Default),
        1);
    QVERIFY(
        QtMaterialComboBoxAdapter::isApplied(
            first));
    QVERIFY(
        !QtMaterialComboBoxAdapter::isApplied(
            second));
}

void tst_NativeComboBoxAdapter::
    editableAndNonEditableRender()
{
    ExposedComboBox combo;
    combo.addItems({
        QStringLiteral("One"),
        QStringLiteral("Two")
    });
    combo.setCurrentIndex(1);
    QtMaterialComboBoxAdapter::apply(&combo);

    QVERIFY(
        !renderCombo(
             combo,
             QSize(240, 48))
             .isNull());

    combo.setEditable(true);
    combo.setEditText(QStringLiteral("Editable"));
    QVERIFY(
        !renderCombo(
             combo,
             QSize(240, 48))
             .isNull());

    combo.setEnabled(false);
    QVERIFY(
        !renderCombo(
             combo,
             QSize(240, 48))
             .isNull());
}

QTEST_MAIN(tst_NativeComboBoxAdapter)
#include "tst_nativecomboboxadapter.moc"
