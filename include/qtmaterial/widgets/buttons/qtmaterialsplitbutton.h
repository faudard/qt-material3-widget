#pragma once

#include <QIcon>
#include <QString>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {

class QtMaterialFilledButton;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSplitButton : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool expressive READ expressive WRITE setExpressive NOTIFY expressiveChanged)
    Q_PROPERTY(QtMaterial::QtMaterialButtonSize expressiveSize READ expressiveSize WRITE setExpressiveSize NOTIFY expressiveSizeChanged)

public:
    explicit QtMaterialSplitButton(QWidget* parent = nullptr);
    explicit QtMaterialSplitButton(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialSplitButton() override;

    QString text() const;
    void setText(const QString& text);

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    bool expressive() const noexcept;
    void setExpressive(bool expressive);

    QtMaterialButtonSize expressiveSize() const noexcept;
    void setExpressiveSize(QtMaterialButtonSize size);

    QtMaterialFilledButton* primaryButton() const noexcept;
    QtMaterialFilledButton* trailingButton() const noexcept;

Q_SIGNALS:
    void primaryTriggered();
    void secondaryTriggered();
    void textChanged(const QString& text);
    void expressiveChanged(bool expressive);
    void expressiveSizeChanged(QtMaterial::QtMaterialButtonSize size);

private:
    void syncButtons();

    QtMaterialFilledButton* m_primary = nullptr;
    QtMaterialFilledButton* m_trailing = nullptr;
    bool m_expressive = true;
    QtMaterialButtonSize m_size = QtMaterialButtonSize::Small;
};

} // namespace QtMaterial
