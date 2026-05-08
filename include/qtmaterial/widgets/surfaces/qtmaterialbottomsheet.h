#pragma once

#include <QPointer>
#include <QSize>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialoverlaysurface.h"

class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QShowEvent;
class QWidget;
class QPainterPath;

namespace QtMaterial {

class BottomSheetSpec;
class QtMaterialScrimWidget;
class QtMaterialTransitionController;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialBottomSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool modal READ isModal WRITE setModal NOTIFY modalChanged)
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(bool dismissOnEscape READ dismissOnEscape WRITE setDismissOnEscape NOTIFY dismissOnEscapeChanged)
    Q_PROPERTY(bool dismissOnScrim READ dismissOnScrim WRITE setDismissOnScrim NOTIFY dismissOnScrimChanged)
    Q_PROPERTY(bool dragToDismissEnabled READ isDragToDismissEnabled WRITE setDragToDismissEnabled NOTIFY dragToDismissEnabledChanged)
    Q_PROPERTY(bool restoreFocusOnClose READ restoreFocusOnClose WRITE setRestoreFocusOnClose NOTIFY restoreFocusOnCloseChanged)
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged)
    Q_PROPERTY(QString supportingText READ supportingText WRITE setSupportingText NOTIFY supportingTextChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    enum class SheetState {
        Closed,
        Opening,
        Open,
        Closing
    };
    Q_ENUM(SheetState)

    explicit QtMaterialBottomSheet(QWidget *parent = nullptr);
    ~QtMaterialBottomSheet() override;

    void open();
    void close();

    bool isOpen() const noexcept;
    qreal progress() const noexcept;
    SheetState state() const noexcept;

    void setModal(bool modal);
    bool isModal() const noexcept;

    void setExpandedHeight(int px);
    int expandedHeight() const noexcept;

    void setCollapsedHeight(int px);
    int collapsedHeight() const noexcept;

    void setExpanded(bool expanded);
    bool isExpanded() const noexcept;
    void expand();
    void collapse();

    void setDismissOnEscape(bool enabled);
    bool dismissOnEscape() const noexcept;

    void setDismissOnScrim(bool enabled);
    bool dismissOnScrim() const noexcept;

    void setDragToDismissEnabled(bool enabled);
    bool isDragToDismissEnabled() const noexcept;

    void setRestoreFocusOnClose(bool enabled);
    bool restoreFocusOnClose() const noexcept;

    void setInitialFocusWidget(QWidget *widget);
    QWidget *initialFocusWidget() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QWidget *contentWidget() const noexcept;

    void setTitleText(const QString &text);
    QString titleText() const;

    void setSupportingText(const QString &text);
    QString supportingText() const;

    QString accessibilitySummary() const;

signals:
    void progressChanged(qreal value);
    void stateChanged(QtMaterial::QtMaterialBottomSheet::SheetState state);
    void modalChanged(bool modal);
    void expandedHeightChanged(int height);
    void collapsedHeightChanged(int height);
    void expandedChanged(bool expanded);
    void dismissOnEscapeChanged(bool enabled);
    void dismissOnScrimChanged(bool enabled);
    void dragToDismissEnabledChanged(bool enabled);
    void restoreFocusOnCloseChanged(bool enabled);
    void titleTextChanged(const QString &text);
    void supportingTextChanged(const QString &text);
    void accessibilitySummaryChanged(const QString &summary);
    void opened();
    void closed();
    void dismissed();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void themeChangedEvent(const QtMaterial::Theme &theme) override;

private:
    void ensureSpecResolved() const;
    void ensureGeometryResolved() const;
    void invalidateCachedGeometry();
    void syncToHost();
    void syncScrim();
    void syncContainerGeometry();
    void applyContainerClip();
    void applySheetMask();
    void focusFirstChild();
    void syncAccessibility();
    void setState(SheetState state);
    int effectiveSheetHeight() const;
    QRect sheetVisualRect() const;
    QRect contentRect() const;
    qreal cornerRadius() const;

private:
    mutable bool m_specDirty = true;
    mutable bool m_geometryDirty = true;
    mutable QtMaterial::BottomSheetSpec *m_specPtr = nullptr;
    mutable QRect m_cachedVisualRect;
    mutable QRect m_cachedContentRect;
    mutable QPainterPath m_cachedContainerPath;
    mutable qreal m_cachedCornerRadius = 0.0;

    QPointer<QtMaterialScrimWidget> m_scrim;
    QPointer<QtMaterialTransitionController> m_transition;
    QPointer<QWidget> m_container;
    QPointer<QWidget> m_initialFocusWidget;
    QPointer<QWidget> m_lastFocusBeforeOpen;

    SheetState m_state = SheetState::Closed;
    bool m_modal = true;
    bool m_expanded = true;
    bool m_dismissOnEscape = true;
    bool m_dismissOnScrim = true;
    bool m_dragToDismissEnabled = true;
    bool m_restoreFocusOnClose = true;
    bool m_dragging = false;
    int m_expandedHeight = 320;
    int m_collapsedHeight = 80;
    QPoint m_dragStartGlobalPos;
    QString m_titleText;
    QString m_supportingText;
    QString m_lastAccessibilitySummary;
};

} // namespace QtMaterial
