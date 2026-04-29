#pragma once

#include <QString>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT QtMaterialWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString materialComponent READ materialComponent WRITE setMaterialComponent NOTIFY materialMetadataChanged)
    Q_PROPERTY(QString materialVariant READ materialVariant WRITE setMaterialVariant NOTIFY materialMetadataChanged)
    Q_PROPERTY(QString materialRole READ materialRole WRITE setMaterialRole NOTIFY materialMetadataChanged)
    Q_PROPERTY(QString materialTestId READ materialTestId WRITE setMaterialTestId NOTIFY materialMetadataChanged)
    Q_PROPERTY(QString materialState READ materialState WRITE setMaterialState NOTIFY materialMetadataChanged)

public:
    explicit QtMaterialWidget(QWidget* parent = nullptr);
    ~QtMaterialWidget() override;

    QString materialComponent() const;
    QString materialVariant() const;
    QString materialRole() const;
    QString materialTestId() const;
    QString materialState() const;

public slots:
    void setMaterialComponent(const QString& value);
    void setMaterialVariant(const QString& value);
    void setMaterialRole(const QString& value);
    void setMaterialTestId(const QString& value);
    void setMaterialState(const QString& value);

signals:
    void materialMetadataChanged();

protected:
    const QtMaterial::Theme& theme() const;
    virtual void themeChangedEvent(const QtMaterial::Theme& theme);

private slots:
    void handleThemeChanged(const QtMaterial::Theme& theme);

private:
    void setMetadata(QString* storage, const QString& value);

    QString m_materialComponent;
    QString m_materialVariant;
    QString m_materialRole;
    QString m_materialTestId;
    QString m_materialState;
};

} // namespace QtMaterial
