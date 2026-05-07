#pragma once

#include <QIcon>
#include <QSize>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFab : public QtMaterialFilledButton {
    Q_OBJECT
    Q_PROPERTY(bool requiresAccessibleName READ requiresAccessibleName WRITE setRequiresAccessibleName)
    Q_PROPERTY(QString iconAccessibleName READ iconAccessibleName WRITE setIconAccessibleName)

public:
    explicit QtMaterialFab(QWidget* parent = nullptr);
    explicit QtMaterialFab(const QIcon& icon, QWidget* parent = nullptr);
    ~QtMaterialFab() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool requiresAccessibleName() const noexcept;
    void setRequiresAccessibleName(bool required);

    QString iconAccessibleName() const;
    void setIconAccessibleName(const QString& name);

    QString effectiveAccessibleName() const;
    bool hasUsableAccessibleName() const;
    QString accessibilitySummary() const;

Q_SIGNALS:
    void accessibilitySummaryChanged(const QString& summary);

protected:
    ButtonSpec resolveButtonSpec() const override;

private:
    void initializeFab();
    void syncAccessibilityState();

    bool m_requiresAccessibleName = true;
    QString m_iconAccessibleName;
    QString m_lastAccessibilitySummary;
};

} // namespace QtMaterial
