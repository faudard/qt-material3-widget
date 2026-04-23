#pragma once

#include <QPointer>
#include <QVariant>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"

class QLabel;
class QPushButton;
class QHBoxLayout;
class QTimer;
class QKeyEvent;
class QResizeEvent;

namespace QtMaterial {

class QtMaterialTransitionController;
struct SnackbarSpec;

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

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void progressChanged(qreal value);
    void shown();
    void actionTriggered();
    void dismissed(QtMaterial::SnackbarDismissReason reason);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void syncGeometryToHost() override;

private:
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
    int currentDurationMs() const;
    QRectF containerRect() const;
    qreal resolvedCornerRadius() const noexcept;

private:
    mutable bool m_specDirty = true;
    mutable QtMaterial::SnackbarSpec* m_specPtr = nullptr;

    SnackbarRequest m_request;
    SnackbarDismissReason m_pendingDismissReason = SnackbarDismissReason::Manual;
    State m_state = State::Hidden;

    QLabel* m_label = nullptr;
    QPushButton* m_actionButton = nullptr;
    QPushButton* m_dismissButton = nullptr;
    QHBoxLayout* m_layout = nullptr;
    QTimer* m_timer = nullptr;
    QPointer<QtMaterialTransitionController> m_transition;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::SnackbarDuration)
Q_DECLARE_METATYPE(QtMaterial::SnackbarDismissReason)