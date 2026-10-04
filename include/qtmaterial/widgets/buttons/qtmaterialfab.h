#pragma once
#include <memory>
#include <QtGlobal>

#include <QIcon>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class QEvent;

namespace QtMaterial {

class QtMaterialFabPrivate;

enum class QtMaterialFabVariant
{
    Primary,
    Secondary,
    Tertiary,
    Surface,
};

enum class QtMaterialFabSize
{
    Small,
    Standard,
    Medium,
    Large
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

    QtMaterialFabSize fabSize() const noexcept;
    void setFabSize(QtMaterialFabSize size);

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

signals:
    void fabSizeChanged(QtMaterial::QtMaterialFabSize size);

protected:
    ButtonSpec resolveButtonSpec() const override;
    void applyExpressiveSpec(ButtonSpec& spec) const override;
    void changeEvent(QEvent* event) override;
    void contentChangedEvent() override;
    void syncAccessibilityState() override;

private:
    void initializeFab();
    void syncFabAccessibility();

    std::unique_ptr<QtMaterialFabPrivate> d_ptr;
};

} // namespace QtMaterial
