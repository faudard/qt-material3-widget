#pragma once

#include <QComboBox>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

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
    QString m_labelText;
};

} // namespace QtMaterial
