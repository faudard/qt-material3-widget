#pragma once

#include <memory>

#include <QList>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmateriallistspec.h"

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
    explicit QtMaterialList(QWidget* parent = nullptr);
    ~QtMaterialList() override;

    int count() const noexcept;
    QtMaterialListItem* itemAt(int index) const;

    void addItem(QtMaterialListItem* item);
    void insertItem(int index, QtMaterialListItem* item);
    void removeItem(QtMaterialListItem* item);
    void clear();

    bool dividersVisible() const noexcept;
    void setDividersVisible(bool visible);

    const ListSpec& resolvedSpec() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void countChanged(int count);
    void dividersVisibleChanged(bool visible);

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const Theme& theme) override;

private:
    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void synchronizeItemDividers();
    void removeDestroyedItem(QObject* object);

    std::unique_ptr<QtMaterialListPrivate> d_ptr;
};

} // namespace QtMaterial
