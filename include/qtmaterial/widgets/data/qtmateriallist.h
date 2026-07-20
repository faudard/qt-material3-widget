#pragma once

#include <memory>

#include <QList>
#include <QString>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmateriallistspec.h"

class QEvent;
class QKeyEvent;
class QPaintEvent;

namespace QtMaterial {

class QtMaterialListItem;
class QtMaterialListPrivate;
class Theme;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialList
    : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(
        bool dividersVisible
        READ dividersVisible
        WRITE setDividersVisible
        NOTIFY dividersVisibleChanged)

public:
    enum class SelectionMode
    {
        NoSelection,
        SingleSelection,
        MultiSelection
    };
    Q_ENUM(SelectionMode)

    explicit QtMaterialList(QWidget* parent = nullptr);
    ~QtMaterialList() override;

    int count() const noexcept;
    bool isEmpty() const noexcept;
    QtMaterialListItem* itemAt(int index) const;
    int indexOf(
        const QtMaterialListItem* item) const noexcept;

    void addItem(QtMaterialListItem* item);
    QtMaterialListItem* addItem(const QString& headline);
    void insertItem(
        int index,
        QtMaterialListItem* item);
    QtMaterialListItem* takeItem(int index);
    void removeItem(int index);
    void removeItem(QtMaterialListItem* item);
    void clear();

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    SelectionMode selectionMode() const noexcept;
    void setSelectionMode(SelectionMode mode);
    QList<int> selectedIndexes() const;
    void clearSelection();

    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);

    QString itemAccessibleText(int index) const;
    QString accessibilitySummary() const;

    bool dividersVisible() const noexcept;
    void setDividersVisible(bool visible);

    const ListSpec& resolvedSpec() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void countChanged(int count);
    void currentIndexChanged(int index);
    void selectionModeChanged(
        QtMaterial::QtMaterialList::SelectionMode mode);
    void selectionChanged();
    void itemClicked(int index);
    void itemActivated(int index);
    void accessibilitySummaryChanged(
        const QString& summary);
    void dividersVisibleChanged(bool visible);

protected:
    bool eventFilter(
        QObject* watched,
        QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const Theme& theme) override;

private:
    void initialiseItem(QtMaterialListItem* item);
    void syncItemSelection();
    void syncAccessibility();
    void emitAccessibilityIfChanged();
    bool activateIndex(int index);
    bool moveCurrent(int delta);
    bool moveToBoundary(bool first);
    int nextEnabledIndex(
        int start,
        int delta) const;
    int firstEnabledIndex() const;
    int lastEnabledIndex() const;
    bool isValidIndex(int index) const noexcept;

    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void synchronizeItemDividers();
    void removeDestroyedItem(QObject* object);

    std::unique_ptr<QtMaterialListPrivate> d_ptr;
};

} // namespace QtMaterial
