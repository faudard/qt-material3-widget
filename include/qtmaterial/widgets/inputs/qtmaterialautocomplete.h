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
 Q_PROPERTY(int maxVisibleCompletions READ maxVisibleCompletions WRITE setMaxVisibleCompletions)
 Q_PROPERTY(bool completesOnReturn READ completesOnReturn WRITE setCompletesOnReturn)
 Q_PROPERTY(bool opensOnFocus READ opensOnFocus WRITE setOpensOnFocus)

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

    int maxVisibleCompletions() const noexcept;

  void setMaxVisibleCompletions(int count);

  bool completesOnReturn() const noexcept;

  void setCompletesOnReturn(bool enabled);

  bool opensOnFocus() const noexcept;

  void setOpensOnFocus(bool enabled);

  bool hidePopupOnEscape() const noexcept;

  void setHidePopupOnEscape(bool enabled);

  QString currentCompletion() const;

  QString accessibilitySummary() const;

  bool isPopupVisible() const noexcept;
    void setPopupVisible(bool visible);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void completionActivated(const QString& text);
    void popupVisibilityChanged(bool visible);

  void accessibilitySummaryChanged(const QString& summary);

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
    void updateAccessibilityState();

  void updateFilterText();

    QLineEdit* m_lineEdit = nullptr;
    QtMaterialAutocompletePopup* m_popup = nullptr;
    QStringListModel* m_ownedStringModel = nullptr;
    int m_maxVisibleCompletions = 8;

  bool m_completesOnReturn = true;

  bool m_opensOnFocus = true;

  bool m_hidePopupOnEscape = true;

  QString m_lastAccessibilitySummary;

  mutable bool m_specDirty = true;
    mutable AutocompleteSpec m_spec;
};

} // namespace QtMaterial
