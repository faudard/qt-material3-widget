#pragma once

#include <QStringList>
#include <QVector>

#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"

class QBoxLayout;
class QEvent;

namespace QtMaterial {

class QtMaterialFilledButton;
class QtMaterialTextButton;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialButtonGroup : public QtMaterialWidget
{
    Q_OBJECT
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive NOTIFY exclusiveChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(bool expressive READ expressive WRITE setExpressive NOTIFY expressiveChanged)
    Q_PROPERTY(QStringList buttonLabels READ buttonLabels WRITE setButtonLabels NOTIFY buttonLabelsChanged)

public:
    explicit QtMaterialButtonGroup(QWidget* parent = nullptr);
    ~QtMaterialButtonGroup() override;

    int count() const noexcept;
    QtMaterialTextButton* buttonAt(int index) const;

    QtMaterialFilledButton* addButton(const QString& text);
    void addButton(QtMaterialTextButton* button);
    void removeButton(QtMaterialTextButton* button);
    void clear();

    QStringList buttonLabels() const;
    void setButtonLabels(const QStringList& labels);

    bool isExclusive() const noexcept;
    void setExclusive(bool exclusive);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    int spacing() const noexcept;
    void setSpacing(int spacing);

    bool expressive() const noexcept;
    void setExpressive(bool expressive);

Q_SIGNALS:
    void buttonTriggered(int index);
    void currentIndexChanged(int index);
    void exclusiveChanged(bool exclusive);
    void spacingChanged(int spacing);
    void expressiveChanged(bool expressive);
    void buttonLabelsChanged(const QStringList& labels);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    int indexOf(const QObject* object) const noexcept;
    int nextEnabledIndex(int start, int step) const noexcept;
    void syncButtons();
    void focusIndex(int index);

    QBoxLayout* m_layout = nullptr;
    QVector<QtMaterialTextButton*> m_buttons;
    bool m_exclusive = true;
    bool m_expressive = true;
    int m_currentIndex = -1;
    int m_pendingCurrentIndex = -1;
};

} // namespace QtMaterial
