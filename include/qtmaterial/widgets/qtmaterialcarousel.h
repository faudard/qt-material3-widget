#pragma once

#include <QWidget>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

class QHBoxLayout;
class QPushButton;
class QStackedWidget;

namespace QtMaterial {

class QtMaterialCarousel : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool wrap READ wrap WRITE setWrap)

public:
    explicit QtMaterialCarousel(QWidget* parent = nullptr);
    ~QtMaterialCarousel() override;

    int count() const;
    int currentIndex() const;
    bool wrap() const;
    void setWrap(bool wrap);

    void addPage(QWidget* page, const QString& label = QString());
    QWidget* pageAt(int index) const;
    QString pageLabel(int index) const;

    CarouselSpec spec() const;
    void setSpec(const CarouselSpec& spec);

public slots:
    void setCurrentIndex(int index);
    void next();
    void previous();

signals:
    void currentIndexChanged(int index);
    void pageActivated(int index);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void buildUi();
    void applySpec();
    void updateControls();

    CarouselSpec m_spec;
    QStackedWidget* m_stack = nullptr;
    QPushButton* m_previousButton = nullptr;
    QPushButton* m_nextButton = nullptr;
    QWidget* m_indicatorHost = nullptr;
    QHBoxLayout* m_indicatorLayout = nullptr;
    QStringList m_labels;
    bool m_wrap = true;
};

} // namespace QtMaterial
