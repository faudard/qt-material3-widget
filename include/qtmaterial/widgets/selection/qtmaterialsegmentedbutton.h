#pragma once

#include <memory>

#include <QIcon>

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {
class QtMaterialSegmentedButtonPrivate;

struct QtMaterialSegmentedButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSegmentedButton : public QtMaterialControl {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool multiSelection READ isMultiSelection WRITE setMultiSelection NOTIFY multiSelectionChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

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

    bool isSegmentEnabled(int index) const;
    void setSegmentEnabled(int index, bool enabled);
    QString segmentAccessibleText(int index) const;
    QString currentSegmentAccessibleText() const;
    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(int index);
    void segmentToggled(int index, bool checked);
    void segmentEnabledChanged(int index, bool enabled);
    void multiSelectionChanged(bool enabled);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
void ensureSpecResolved() const;
    QRect segmentRect(int index) const;
    int indexAt(const QPoint& pos) const;
    void toggleIndex(int index);
    int firstEnabledIndex() const noexcept;
    int lastEnabledIndex() const noexcept;
    int nextEnabledIndex(int start, int delta) const noexcept;
    void syncAccessibility();
    std::unique_ptr<QtMaterialSegmentedButtonPrivate> d;
};

} // namespace QtMaterial
