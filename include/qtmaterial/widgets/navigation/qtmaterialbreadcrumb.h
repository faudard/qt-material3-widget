#pragma once

#include <memory>

#include <QStringList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialBreadcrumbPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBreadcrumb : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    explicit QtMaterialBreadcrumb(QWidget* parent = nullptr);
    ~QtMaterialBreadcrumb() override;

    QStringList items() const;
    void setItems(const QStringList& items);
    void addItem(const QString& text);
    void clear();

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

Q_SIGNALS:
    void activated(int index, const QString& text);
    void currentIndexChanged(int index);

private:
    void rebuild();
    void refreshCurrentSegment();

    std::unique_ptr<QtMaterialBreadcrumbPrivate> d_ptr;
};

} // namespace QtMaterial
