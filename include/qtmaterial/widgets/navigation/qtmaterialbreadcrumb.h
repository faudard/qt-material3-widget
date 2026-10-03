#pragma once

#include <memory>

#include <QStringList>
#include <QWidget>

class QResizeEvent;

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialBreadcrumbPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBreadcrumb : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int maximumVisibleItems READ maximumVisibleItems WRITE setMaximumVisibleItems NOTIFY maximumVisibleItemsChanged)
    Q_PROPERTY(bool responsiveElisionEnabled READ responsiveElisionEnabled WRITE setResponsiveElisionEnabled NOTIFY responsiveElisionEnabledChanged)

public:
    explicit QtMaterialBreadcrumb(QWidget* parent = nullptr);
    ~QtMaterialBreadcrumb() override;

    QStringList items() const;
    void setItems(const QStringList& items);
    void addItem(const QString& text);
    void clear();

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    int maximumVisibleItems() const noexcept;
    void setMaximumVisibleItems(int count);

    bool responsiveElisionEnabled() const noexcept;
    void setResponsiveElisionEnabled(bool enabled);

Q_SIGNALS:
    void activated(int index, const QString& text);
    void currentIndexChanged(int index);
    void maximumVisibleItemsChanged(int count);
    void responsiveElisionEnabledChanged(bool enabled);

protected:
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void rebuild();
    void refreshCurrentSegment();
    void refreshDirection();
    int effectiveVisibleLimit() const;

    std::unique_ptr<QtMaterialBreadcrumbPrivate> d_ptr;
};

} // namespace QtMaterial
