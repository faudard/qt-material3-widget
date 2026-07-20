#pragma once

#include <memory>

#include <QPointer>
#include <QVariant>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"

class QKeyEvent;
class QResizeEvent;
class QObject;

namespace QtMaterial {


class QtMaterialSnackbarPrivate;

enum class SnackbarDuration {
    Short,
    Long,
    Indefinite
};

enum class SnackbarDismissReason {
    Timeout,
    Action,
    Manual,
    Consecutive
};

struct SnackbarRequest
{
    QString text;
    QString actionText;
    SnackbarDuration duration = SnackbarDuration::Short;
    bool showDismissButton = false;
    QVariant payload;
};

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSnackbar : public QtMaterialOverlaySurface
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)
    Q_PROPERTY(bool pauseAutoHideOnInteraction READ pauseAutoHideOnInteraction WRITE setPauseAutoHideOnInteraction)

public:
    explicit QtMaterialSnackbar(QWidget* parent = nullptr);
    ~QtMaterialSnackbar() override;

    void setRequest(const SnackbarRequest& request);
    SnackbarRequest request() const noexcept;

    void setText(const QString& text);
    QString text() const;

    void setActionText(const QString& text);
    QString actionText() const;

    void setDuration(SnackbarDuration duration);
    SnackbarDuration duration() const noexcept;

    void setShowDismissButton(bool on);
    bool showDismissButton() const noexcept;

    void showSnackbar();
    void dismiss(SnackbarDismissReason reason = SnackbarDismissReason::Manual);

    qreal progress() const noexcept;
    const SnackbarSpec& resolvedSpec() const;

    QString accessibilitySummary() const;
    bool pauseAutoHideOnInteraction() const noexcept;
    bool isAutoHidePaused() const noexcept;
    int remainingAutoHideTimeMs() const noexcept;
    void setPauseAutoHideOnInteraction(bool enabled);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void progressChanged(qreal value);
    void accessibilitySummaryChanged(const QString& summary);
    void autoHidePausedChanged(bool paused);
    void shown();
    void actionTriggered();
    void dismissed(QtMaterial::SnackbarDismissReason reason);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void syncGeometryToHost() override;

private:

 friend class QtMaterialSnackbarPrivate;

 enum class State {
        Hidden,
        Entering,
        Visible,
        Leaving
    };

    void ensureSpecResolved() const;
    void invalidateResolvedSpec();
    void applyResolvedTheme();
    void applyRequestToUi();
    void updateAutoHide();
    void syncAccessibilityState();
    void setAutoHidePaused(bool paused);
    qreal resolvedCornerRadius() const noexcept;

private:

 std::unique_ptr<QtMaterialSnackbarPrivate> d_ptr;

};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::SnackbarDuration)
Q_DECLARE_METATYPE(QtMaterial::SnackbarDismissReason)