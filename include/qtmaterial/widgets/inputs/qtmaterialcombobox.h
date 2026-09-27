#pragma once

#include <memory>

#include <QComboBox>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialComboBoxPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText NOTIFY labelTextChanged)

public:
    explicit QtMaterialComboBox(QWidget* parent = nullptr);
    ~QtMaterialComboBox() override;

    QString labelText() const;
    void setLabelText(const QString& text);

signals:
    void labelTextChanged(const QString& text);

private:
    std::unique_ptr<QtMaterialComboBoxPrivate> d_ptr;
};

} // namespace QtMaterial
