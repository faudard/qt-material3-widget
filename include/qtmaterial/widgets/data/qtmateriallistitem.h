#pragma once
#include <QString>
#include "qtmaterial/core/qtmaterialcontrol.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialListItem : public QtMaterialControl
{
    Q_OBJECT
public:
    explicit QtMaterialListItem(QWidget* parent = nullptr);
    ~QtMaterialListItem() override;
    QString headlineText() const;
    void setHeadlineText(const QString& text);
    QString supportingText() const;
    void setSupportingText(const QString& text);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString m_headlineText;
    QString m_supportingText;
};
} // namespace QtMaterial
