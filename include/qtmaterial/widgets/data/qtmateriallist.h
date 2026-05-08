#pragma once

#include <memory>

#include <QList>
#include <QVector>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QVBoxLayout;

namespace QtMaterial {

class QtMaterialListPrivate;

class QtMaterialListItem;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialList : public QWidget
{
    Q_OBJECT

public:
    enum class SelectionMode {
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
    int indexOf(const QtMaterialListItem* item) const noexcept;

    void addItem(QtMaterialListItem* item);
    QtMaterialListItem* addItem(const QString& headline);
    void insertItem(int index, QtMaterialListItem* item);
    QtMaterialListItem* takeItem(int index);
    void removeItem(int index);
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

signals:
    void currentIndexChanged(int index);
    void selectionModeChanged(QtMaterial::QtMaterialList::SelectionMode mode);
    void selectionChanged();
    void itemClicked(int index);
    void itemActivated(int index);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    std::unique_ptr<QtMaterialListPrivate> d_ptr;
    void initialiseItem(QtMaterialListItem* item);
    void syncItemSelection();
    void syncAccessibility();
    void emitAccessibilityIfChanged();

    bool activateIndex(int index);
    bool moveCurrent(int delta);
    bool moveToBoundary(bool first);
    int nextEnabledIndex(int start, int delta) const;
    int firstEnabledIndex() const;
    int lastEnabledIndex() const;
    bool isValidIndex(int index) const noexcept;

};

} // namespace QtMaterial
