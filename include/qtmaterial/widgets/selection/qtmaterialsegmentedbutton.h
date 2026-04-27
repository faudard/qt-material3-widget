#pragma once

#include <QIcon>
#include <QVector>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialsegmentedbuttonspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSegmentedButton : public QtMaterialControl {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool multiSelection READ isMultiSelection WRITE setMultiSelection NOTIFY multiSelectionChanged)

public:
    explicit QtMaterialSegmentedButton(QWidget* parent = nullptr);
    ~QtMaterialSegmentedButton() override;

    int count() const noexcept;
    void addSegment(const QString& text, const QIcon& icon = QIcon());
    void insertSegment(int index, const QString& text, const QIcon& icon = QIcon());
    void removeSegment(int index);
    void clearSegments();

    QString segmentText(int index) const;
    void setSegmentText(int index, const QString& text);
    QIcon segmentIcon(int index) const;
    void setSegmentIcon(int index, const QIcon& icon);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    bool isSegmentChecked(int index) const;
    void setSegmentChecked(int index, bool checked);

    bool isMultiSelection() const noexcept;
    void setMultiSelection(bool enabled);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(int index);
    void segmentToggled(int index, bool checked);
    void multiSelectionChanged(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    struct Segment {
        QString text;
        QIcon icon;
        bool checked = false;
    };

    void ensureSpecResolved() const;
    QRect segmentRect(int index) const;
    int indexAt(const QPoint& pos) const;
    void toggleIndex(int index);

    QVector<Segment> m_segments;
    int m_currentIndex = -1;
    bool m_multiSelection = false;
    mutable bool m_specDirty = true;
    mutable SegmentedButtonSpec m_spec;
};

} // namespace QtMaterial
