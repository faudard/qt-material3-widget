#pragma once

#include <memory>

#include <QPointer>
#include <QString>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/qtmaterialglobal.h"

class QAbstractButton;
class QKeyEvent;
class QVBoxLayout;
class QWidget;

namespace QtMaterial {

class QtMaterialDialogPrivate;
class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDialog : public QtMaterialOverlaySurface
{
    Q_OBJECT

public:
    explicit QtMaterialDialog(QWidget* parent = nullptr);
    ~QtMaterialDialog() override;

    QString titleText() const;
    void setTitleText(const QString& title);

    QString supportingText() const;
    void setSupportingText(const QString& text);

    void setBodyWidget(QWidget* widget);
    QWidget* bodyWidget() const;

    QWidget* initialFocusWidget() const;
    void setInitialFocusWidget(QWidget* widget);

    QAbstractButton* defaultButton() const;
    void setDefaultButton(QAbstractButton* button);

    bool dismissOnEscape() const noexcept;
    void setDismissOnEscape(bool enabled);

    bool restoreFocusOnClose() const noexcept;
    void setRestoreFocusOnClose(bool enabled);

    QString accessibilitySummary() const;

public Q_SLOTS:
    void open();
    void close();
    void reject();

Q_SIGNALS:
    void opened();
    void closed();
    void rejected();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool focusNextPrevChild(bool next) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void resolveSpecIfNeeded() const;
    void syncChildGeometry();
    void updateAccessibilityMetadata();
    QList<QWidget*> focusableDialogChildren() const;
    void focusInitialWidget();
    bool moveFocusInsideDialog(bool next);

    std::unique_ptr<QtMaterialDialogPrivate> d_ptr;

};

} // namespace QtMaterial
