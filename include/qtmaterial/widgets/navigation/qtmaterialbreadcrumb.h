#pragma once

#include <memory>

#include <QStringList>
#include <QIcon>
#include <QUrl>
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
    Q_PROPERTY(bool locationEditable READ isLocationEditable WRITE setLocationEditable)
    Q_PROPERTY(bool editingLocation READ isEditingLocation WRITE setEditingLocation NOTIFY editingLocationChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation)
    Q_PROPERTY(bool dragDropEnabled READ dragDropEnabled WRITE setDragDropEnabled)

public:
    explicit QtMaterialBreadcrumb(QWidget* parent = nullptr);
    ~QtMaterialBreadcrumb() override;

    QStringList items() const;
    void setItems(const QStringList& items);
    void addItem(const QString& text);
    void clear();

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);
    QUrl itemUrl(int index) const;
    void setItemUrl(int index, const QUrl& url);
    bool dragDropEnabled() const noexcept;
    void setDragDropEnabled(bool enabled);

    bool isLocationEditable() const noexcept;
    void setLocationEditable(bool editable);
    bool isEditingLocation() const noexcept;
    void setEditingLocation(bool editing);
    QString location() const;
    void setLocation(const QString& location);

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
    void editingLocationChanged(bool editing);
    void locationSubmitted(const QString& location);
    void urlsDropped(int index, const QList<QUrl>& urls);

protected:
    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void rebuild();
    void refreshCurrentSegment();
    void refreshDirection();
    int effectiveVisibleLimit() const;
    void finishLocationEditing(bool submit);

    std::unique_ptr<QtMaterialBreadcrumbPrivate> d_ptr;
};

} // namespace QtMaterial
