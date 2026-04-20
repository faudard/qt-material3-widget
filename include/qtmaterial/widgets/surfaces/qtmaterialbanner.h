#pragma once

#include <QIcon>
#include <QPointer>
#include <QString>

#include "qtmaterial/widgets/surfaces/qtmaterialsurface.h"

class QAbstractButton;
class QPaintEvent;
class QResizeEvent;
class QKeyEvent;

namespace QtMaterial {
struct BannerSpec;
class Theme;
}

class QtMaterialBanner : public QtMaterialSurface
{
    Q_OBJECT

public:
    explicit QtMaterialBanner(QWidget* parent = nullptr);
    explicit QtMaterialBanner(const QString& titleText,
                              const QString& bodyText = QString(),
                              QWidget* parent = nullptr);
    ~QtMaterialBanner() override;

    QString titleText() const;
    void setTitleText(const QString& text);

    QString bodyText() const;
    void setBodyText(const QString& text);

    QIcon leadingIcon() const;
    void setLeadingIcon(const QIcon& icon);

    bool isDismissible() const;
    void setDismissible(bool dismissible);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void dismiss();

signals:
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void stateChangedEvent() override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();
    void syncDismissButton();

    QString m_titleText;
    QString m_bodyText;
    QIcon m_leadingIcon;
    bool m_dismissible = true;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::BannerSpec m_spec;
    mutable QRect m_visualRect;
    mutable QRect m_contentRect;
    mutable QRect m_titleRect;
    mutable QRect m_bodyRect;
    mutable QRect m_iconRect;
    mutable QPainterPath m_containerPath;
    mutable QString m_elidedTitle;
    mutable QString m_elidedBody;

    QPointer<QAbstractButton> m_dismissButton;
};
