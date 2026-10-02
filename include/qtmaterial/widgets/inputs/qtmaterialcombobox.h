#pragma once

#include <memory>

#include <QComboBox>

#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QPaintEvent;

namespace QtMaterial {

struct AutocompleteSpec;
class ThemeContext;
class QtMaterialComboBoxPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialComboBox
    : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(
        QString labelText
        READ labelText
        WRITE setLabelText
        NOTIFY labelTextChanged)
    Q_PROPERTY(
        QtMaterial::ThemeContext* themeContext
        READ themeContext
        WRITE setThemeContext
        NOTIFY themeContextChanged)

public:
    explicit QtMaterialComboBox(QWidget* parent = nullptr);
    ~QtMaterialComboBox() override;

    QString labelText() const;
    void setLabelText(const QString& text);

    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept;
    ThemeContext* effectiveThemeContext() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void showPopup() override;
    void hidePopup() override;

signals:
    void labelTextChanged(const QString& text);
    void themeContextChanged(QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    const AutocompleteSpec& resolvedSpec() const;
    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void preparePopup();
    void updatePopupMask();
    void syncEditableLineEdit();

    std::unique_ptr<QtMaterialComboBoxPrivate> d_ptr;
};

} // namespace QtMaterial
