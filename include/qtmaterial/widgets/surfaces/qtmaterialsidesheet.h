#pragma once

#include <memory>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/qtmaterialglobal.h"

class QEvent;
class QHideEvent;
class QKeyEvent;
class QPaintEvent;
class QShowEvent;
class QWidget;

namespace QtMaterial {

class QtMaterialSideSheetPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSideSheet : public QtMaterialOverlaySurface
{
    Q_OBJECT
    Q_PROPERTY(Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)
    Q_PROPERTY(bool modal READ isModal WRITE setModal NOTIFY modalChanged)
    Q_PROPERTY(QString titleText READ titleText WRITE setTitleText NOTIFY titleTextChanged)
    Q_PROPERTY(bool dismissOnScrimClick READ dismissOnScrimClick WRITE setDismissOnScrimClick NOTIFY dismissOnScrimClickChanged)
    Q_PROPERTY(bool open READ isOpen NOTIFY openChanged)

public:
    enum class Edge {
        Left,
        Right
    };
    Q_ENUM(Edge)

    explicit QtMaterialSideSheet(QWidget* parent = nullptr);
    ~QtMaterialSideSheet() override;

    Edge edge() const noexcept;
    void setEdge(Edge edge);

    bool isModal() const noexcept;
    void setModal(bool modal);

    QString titleText() const;
    void setTitleText(const QString& text);

    bool dismissOnScrimClick() const noexcept;
    void setDismissOnScrimClick(bool enabled);

    QWidget* contentWidget() const noexcept;

    bool isOpen() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void open();
    void closeSheet();

Q_SIGNALS:
    void edgeChanged(QtMaterial::QtMaterialSideSheet::Edge edge);
    void modalChanged(bool modal);
    void titleTextChanged(const QString& text);
    void dismissOnScrimClickChanged(bool enabled);
    void openChanged(bool open);
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void syncGeometryToHost() override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;

private:
    void syncScrim();
    void syncAccessibility();

    std::unique_ptr<QtMaterialSideSheetPrivate> d_ptr;
};

} // namespace QtMaterial
