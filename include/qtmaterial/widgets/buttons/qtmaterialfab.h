#pragma once
#include <QtGlobal>

#include <QIcon>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class QEvent;

namespace QtMaterial {

enum class QtMaterialFabVariant
{
    Primary,
    Secondary,
    Tertiary,
    Surface,
};


class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFab : public QtMaterialFilledButton
{
    Q_OBJECT

public:
    explicit QtMaterialFab(QWidget* parent = nullptr);
    explicit QtMaterialFab(const QIcon& icon, QWidget* parent = nullptr);
    ~QtMaterialFab() override;

    QtMaterialFabVariant fabVariant() const noexcept;
    void setFabVariant(QtMaterialFabVariant variant);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool requiresAccessibleName() const noexcept;
    void setRequiresAccessibleName(bool required);

    QString iconAccessibleName() const;
    void setIconAccessibleName(const QString& name);

    QString effectiveAccessibleName() const;
    bool hasUsableAccessibleName() const;
    QString accessibilitySummary() const;

signals:
    void accessibilitySummaryChanged(const QString& summary);

protected:
    ButtonSpec resolveButtonSpec() const override;
    void changeEvent(QEvent* event) override;
    void contentChangedEvent() override;
    void syncAccessibilityState() override;

private:
    void initializeFab();
    void syncFabAccessibility();

    QString m_iconAccessibleName;
    QString m_lastAccessibilitySummary;
    bool m_requiresAccessibleName = true;

    QtMaterialFabVariant m_fabVariant = QtMaterialFabVariant::Primary;
};

} // namespace QtMaterial
