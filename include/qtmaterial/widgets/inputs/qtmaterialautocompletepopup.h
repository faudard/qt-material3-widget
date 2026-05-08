#pragma once
#include <memory>

#include <QPointer>
#include <QStringList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialautocompletepopupspec.h"

class QListView;
class QStringListModel;
class QSortFilterProxyModel;
class QModelIndex;
class QAbstractItemModel;
class QLineEdit;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial { class Theme; }

class QtMaterialAutocompletePopupPrivate;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAutocompletePopup : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialAutocompletePopup(QWidget* parent = nullptr);
    ~QtMaterialAutocompletePopup() override;

    void setAnchorLineEdit(QLineEdit* lineEdit);
    QLineEdit* anchorLineEdit() const noexcept;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const noexcept;

    void setSuggestions(const QStringList& suggestions);
    QString currentCompletion() const;

    void setPopupVisible(bool visible);
    bool isPopupVisible() const noexcept;

    void setFilterText(const QString& text);
    QString filterText() const;

    void clearSelection();
    void selectNext();
    void selectPrevious();
    void acceptCurrent();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void completionActivated(const QString& text);
    void popupVisibilityChanged(bool visible);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void ensureSpecResolved() const;
    void invalidatePopupLayout();
    void updatePopupGeometry();
    void updatePopupPalette();
    void syncToAnchorGeometry();
    void syncSelectionFromCurrentIndex();
    std::unique_ptr<QtMaterialAutocompletePopupPrivate> d_ptr;

    QPointer<QAbstractItemModel> m_sourceModel;
    QSortFilterProxyModel* m_filterModel = nullptr;
};
