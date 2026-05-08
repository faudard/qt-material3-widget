#pragma once

#include <QIcon>
#include <QPointer>
#include <QRect>
#include <QSize>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"
#include "qtmaterial/specs/qtmaterialbannerspec.h"

#include <memory>
class QEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class QToolButton;

namespace QtMaterial {
class Theme;
struct BannerSpec;
}
class QtMaterialBannerPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBanner : public QtMaterial::QtMaterialSurface
{
    Q_OBJECT
    Q_DISABLE_COPY(QtMaterialBanner)

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

    bool dismissOnEscape() const noexcept;
    void setDismissOnEscape(bool dismissOnEscape);

    QString primaryActionText() const;
    void setPrimaryActionText(const QString& text);

    QString secondaryActionText() const;
    void setSecondaryActionText(const QString& text);

    QString dismissAccessibleName() const;
    void setDismissAccessibleName(const QString& name);

    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void dismiss();

signals:
    void dismissed();
    void primaryActionTriggered();
    void secondaryActionTriggered();
    void accessibilitySummaryChanged(const QString& summary);

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
    void syncActionButtons();
    void syncAccessibility();
    void emitAccessibilitySummaryIfChanged();

    std::unique_ptr<QtMaterialBannerPrivate> d;


};
