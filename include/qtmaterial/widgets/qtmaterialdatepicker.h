#pragma once
#include <memory>

#include <QDate>
#include <QFrame>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialDatePickerPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDatePicker : public QFrame
    , public ThemeContextHost {
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)
    Q_PROPERTY(
        QtMaterial::ThemeContext* themeContext
        READ themeContext
        WRITE setThemeContext
        NOTIFY themeContextChanged)
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)
    Q_PROPERTY(QDate minimumDate READ minimumDate WRITE setMinimumDate)
    Q_PROPERTY(QDate maximumDate READ maximumDate WRITE setMaximumDate)

public:
    explicit QtMaterialDatePicker(QWidget* parent = nullptr);
    ~QtMaterialDatePicker() override;

    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept override;
    ThemeContext* effectiveThemeContext() const noexcept override;

    QDate selectedDate() const;
    void setSelectedDate(const QDate& date);

    QDate minimumDate() const;
    void setMinimumDate(const QDate& date);

    QDate maximumDate() const;
    void setMaximumDate(const QDate& date);

    DatePickerSpec spec() const;
    const DatePickerSpec& resolvedSpec() const;
    void setSpec(const DatePickerSpec& spec);
    bool hasExplicitSpec() const noexcept;
    void resetSpec();

public slots:
    void showPreviousMonth();
    void showNextMonth();
    void showToday();

signals:
    void themeContextChanged(
        QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);
    void selectedDateChanged(const QDate& date);
    void activated(const QDate& date);

protected:
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void buildUi();
    void updateHeader();
    void ensureSpecResolved() const;
    void applyResolvedSpec();

    std::unique_ptr<QtMaterialDatePickerPrivate> d_ptr;
};

} // namespace QtMaterial
