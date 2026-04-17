#pragma once
#include <QString>
#include "qtmaterial/core/qtmaterialsurface.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBanner : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialBanner(QWidget* parent = nullptr);
    ~QtMaterialBanner() override;
    void setHeadlineText(const QString& text);
    void setSupportingText(const QString& text);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString m_headlineText;
    QString m_supportingText;
};
} // namespace QtMaterial
