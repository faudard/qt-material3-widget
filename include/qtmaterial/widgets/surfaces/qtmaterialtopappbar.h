#pragma once
#include <QString>
#include "qtmaterial/core/qtmaterialsurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTopAppBar : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialTopAppBar(QWidget* parent = nullptr);
    ~QtMaterialTopAppBar() override;
    void setTitle(const QString& title);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString m_title;
};
} // namespace QtMaterial
