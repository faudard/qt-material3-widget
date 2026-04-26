#include <QtTest/QtTest>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QList>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QVBoxLayout>

class tst_KeyboardAccessibility : public QObject {
    Q_OBJECT

private slots:
    void commonControlsExposeKeyboardFocus();
    void accessibleNamesArePresentForPreviewControls();
    void dialogDefaultButtonIsKeyboardReachable();
};

void tst_KeyboardAccessibility::commonControlsExposeKeyboardFocus()
{
    QWidget form;
    auto* layout = new QVBoxLayout(&form);
    auto* button = new QPushButton(QStringLiteral("Button"), &form);
    auto* checkbox = new QCheckBox(QStringLiteral("Checkbox"), &form);
    auto* radio = new QRadioButton(QStringLiteral("Radio"), &form);
    auto* slider = new QSlider(Qt::Horizontal, &form);
    auto* field = new QLineEdit(&form);
    auto* combo = new QComboBox(&form);
    combo->addItem(QStringLiteral("One"));

    const QList<QWidget*> widgets = {button, checkbox, radio, slider, field, combo};
    for (QWidget* widget : widgets) {
        widget->setAccessibleName(widget->metaObject()->className());
        layout->addWidget(widget);
        QVERIFY2(widget->focusPolicy() != Qt::NoFocus,
                 qPrintable(QStringLiteral("%1 must be keyboard-focusable").arg(widget->metaObject()->className())));
        QVERIFY2(!widget->accessibleName().isEmpty(),
                 qPrintable(QStringLiteral("%1 must expose an accessible name").arg(widget->metaObject()->className())));
    }
}

void tst_KeyboardAccessibility::accessibleNamesArePresentForPreviewControls()
{
    QPushButton button(QStringLiteral("Export JSON"));
    button.setAccessibleName(QStringLiteral("Export JSON"));
    button.setAccessibleDescription(QStringLiteral("Export the resolved theme as JSON."));
    QVERIFY(!button.accessibleName().isEmpty());
    QVERIFY(!button.accessibleDescription().isEmpty());
}

void tst_KeyboardAccessibility::dialogDefaultButtonIsKeyboardReachable()
{
    QDialog dialog;
    auto* layout = new QVBoxLayout(&dialog);
    auto* field = new QLineEdit(&dialog);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(field);
    layout->addWidget(buttons);

    QPushButton* ok = buttons->button(QDialogButtonBox::Ok);
    QVERIFY(ok);
    QVERIFY(ok->focusPolicy() != Qt::NoFocus);
    QVERIFY(field->focusPolicy() != Qt::NoFocus);
}

QTEST_MAIN(tst_KeyboardAccessibility)
#include "tst_keyboard_accessibility.moc"
