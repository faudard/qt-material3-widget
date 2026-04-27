#pragma once

#include <QPointer>
#include <QStringList>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/specs/qtmaterialautocompletespec.h"
#include "qtmaterial/qtmaterialglobal.h"

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QCompleter;
class QLineEdit;
class QStringListModel;
QT_END_NAMESPACE

class QtMaterialAutocompletePopup;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAutocomplete : public QtMaterialWidget {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool popupVisible READ isPopupVisible WRITE setPopupVisible NOTIFY popupVisibilityChanged)

public:
    explicit QtMaterialAutocomplete(QWidget* parent = nullptr);
    ~QtMaterialAutocomplete() override;

    QLineEdit* lineEdit() const noexcept;

    QString text() const;
    void setText(const QString& text);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    void setSuggestions(const QStringList& suggestions);
    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const noexcept;

    bool isPopupVisible() const noexcept;
    void setPopupVisible(bool visible);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void completionActivated(const QString& text);
    void popupVisibilityChanged(bool visible);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void ensureSpecResolved() const;
    void invalidateResolvedSpec();
    void updateChildGeometry();
    void updatePaletteFromSpec();
    void updateFilterText();

    QLineEdit* m_lineEdit = nullptr;
    QtMaterialAutocompletePopup* m_popup = nullptr;
    QStringListModel* m_ownedStringModel = nullptr;
    mutable bool m_specDirty = true;
    mutable AutocompleteSpec m_spec;
};

} // namespace QtMaterial
