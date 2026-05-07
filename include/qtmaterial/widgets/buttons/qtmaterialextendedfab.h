#pragma once

#include <QIcon>
#include <QSize>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialExtendedFab : public QtMaterialFilledButton {
    Q_OBJECT

public:
    explicit QtMaterialExtendedFab(QWidget* parent = nullptr);
    explicit QtMaterialExtendedFab(const QString& text, QWidget* parent = nullptr);
    QtMaterialExtendedFab(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
    ~QtMaterialExtendedFab() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QString effectiveAccessibleName() const;
    bool hasUsableAccessibleName() const;
    QString accessibilitySummary() const;

protected:
    ButtonSpec resolveButtonSpec() const override;

private:
    void initializeExtendedFab();
    void syncAccessibilityState();
};

} // namespace QtMaterial
