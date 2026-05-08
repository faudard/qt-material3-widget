#pragma once

#include <memory>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialsurface.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"

#include <QPainterPath>
#include <QRect>
#include <QString>

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialCardPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCard : public QtMaterialSurface {
    Q_OBJECT
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged)
    Q_PROPERTY(QString bodyText READ bodyText WRITE setBodyText NOTIFY bodyTextChanged)
    Q_PROPERTY(Variant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    enum class Variant {
        Elevated,
        Filled,
        Outlined
    };
    Q_ENUM(Variant)

    explicit QtMaterialCard(QWidget* parent = nullptr);
    ~QtMaterialCard() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    void setTitleText(const QString& text);
    QString titleText() const;

    void setBodyText(const QString& text);
    QString bodyText() const;

    Variant variant() const noexcept;
    void setVariant(Variant variant);

    bool isInteractive() const noexcept;
    void setInteractive(bool interactive);

    bool isPressed() const noexcept;

    QString accessibilitySummary() const;

Q_SIGNALS:
    void titleTextChanged(const QString& text);
    void bodyTextChanged(const QString& text);
    void variantChanged(QtMaterial::QtMaterialCard::Variant variant);
    void interactiveChanged(bool interactive);
    void accessibilitySummaryChanged(const QString& summary);
    void pressed();
    void released();
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool event(QEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void stateChangedEvent() override;

private:
    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;
    void invalidateLayoutCache();
    void syncAccessibility();

    QRect visualRect() const;
    QRect contentRectForPaint() const;
    QPainterPath containerPath() const;
    QColor resolvedContainerColor() const;
    QColor resolvedContentColor() const;

private:
    std::unique_ptr<QtMaterialCardPrivate> d;

};

} // namespace QtMaterial
