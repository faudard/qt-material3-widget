#pragma once

#include <QIcon>
#include <QVector>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMenu : public QtMaterialControl {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    explicit QtMaterialMenu(QWidget* parent = nullptr);
    ~QtMaterialMenu() override;

    int addItem(const QString& text, const QIcon& icon = QIcon(), const QString& shortcut = QString());
    void addSeparator();
    void clear();

    int count() const noexcept;
    QString itemText(int index) const;
    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    void popup(const QPoint& globalPos);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(int index);
    void itemTriggered(int index, const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    struct MenuItem {
        QString text;
        QIcon icon;
        QString shortcut;
        bool separator = false;
        bool enabled = true;
    };

    void ensureSpecResolved() const;
    QRect itemRect(int index) const;
    int indexAt(const QPoint& pos) const;
    void triggerIndex(int index);

    QVector<MenuItem> m_items;
    int m_currentIndex = -1;
    mutable bool m_specDirty = true;
    mutable MenuSpec m_spec;
};

} // namespace QtMaterial
