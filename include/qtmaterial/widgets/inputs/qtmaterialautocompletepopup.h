#pragma once
#include <QStringList>
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAutocompletePopup : public QWidget
{
    Q_OBJECT
public:
    explicit QtMaterialAutocompletePopup(QWidget* parent = nullptr);
    ~QtMaterialAutocompletePopup() override;
    void setSuggestions(const QStringList& suggestions);
    QStringList suggestions() const;
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QStringList m_suggestions;
};
} // namespace QtMaterial
