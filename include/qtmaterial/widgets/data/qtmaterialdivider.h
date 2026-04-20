#pragma once

#include <QWidget>
#include <Qt>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {
struct DividerSpec;
class Theme;
}

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDivider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(int leadingInset READ leadingInset WRITE setLeadingInset)
    Q_PROPERTY(int trailingInset READ trailingInset WRITE setTrailingInset)

public:
    explicit QtMaterialDivider(QWidget *parent = nullptr);
    explicit QtMaterialDivider(Qt::Orientation orientation, QWidget *parent = nullptr);
    ~QtMaterialDivider() override;

    Qt::Orientation orientation() const noexcept;
    void setOrientation(Qt::Orientation orientation);

    int leadingInset() const noexcept;
    void setLeadingInset(int value);

    int trailingInset() const noexcept;
    void setTrailingInset(int value);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void ensureSpecResolved() const;

    mutable bool m_specDirty = true;
    mutable QtMaterial::DividerSpec *m_cachedSpecPtr = nullptr; // patch-direction placeholder
    Qt::Orientation m_orientation = Qt::Horizontal;
    int m_leadingInset = 0;
    int m_trailingInset = 0;
};
