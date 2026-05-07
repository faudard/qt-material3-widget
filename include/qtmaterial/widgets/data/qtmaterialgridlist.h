#pragma once

#include <QIcon>
#include <QListWidget>
#include <QSize>
#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialGridList : public QListWidget {
    Q_OBJECT
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(QSize cellExtent READ cellExtent WRITE setCellExtent NOTIFY cellExtentChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    enum class SelectionMode {
        NoSelection,
        SingleSelection,
        MultiSelection,
    };
    Q_ENUM(SelectionMode)

    explicit QtMaterialGridList(QWidget* parent = nullptr);
    ~QtMaterialGridList() override;

    int addGridItem(const QString& title,
                    const QString& supportingText = QString(),
                    const QIcon& icon = QIcon());
    void insertGridItem(int index,
                        const QString& title,
                        const QString& supportingText = QString(),
                        const QIcon& icon = QIcon());

    QString itemTitle(int index) const;
    void setItemTitle(int index, const QString& title);

    QString itemSupportingText(int index) const;
    void setItemSupportingText(int index, const QString& supportingText);

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);

    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);

    int currentIndex() const;
    void setCurrentIndex(int index);

    SelectionMode gridSelectionMode() const;
    void setGridSelectionMode(SelectionMode mode);

    QStringList selectedIndexesAsStrings() const;
    QList<int> selectedIndexesList() const;
    void setItemSelected(int index, bool selected);
    void clearGridSelection();

    int columns() const;
    void setColumns(int columns);

    QSize cellExtent() const;
    void setCellExtent(const QSize& size);

    QString itemAccessibleText(int index) const;
    QString currentItemAccessibleText() const;
    QString accessibilitySummary() const;

Q_SIGNALS:
    void currentIndexChanged(int index);
    void gridItemClicked(int index);
    void gridItemActivated(int index);
    void gridSelectionChanged();
    void columnsChanged(int columns);
    void cellExtentChanged(const QSize& size);
    void accessibilitySummaryChanged(const QString& summary);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    enum ItemRole {
        TitleRole = Qt::UserRole + 120,
        SupportingTextRole,
    };

    QListWidgetItem* gridItemAt(int index) const;
    int nextEnabledIndex(int from, int delta) const;
    int firstEnabledIndex() const;
    int lastEnabledIndex() const;
    void updateItemText(QListWidgetItem* item);
    void updateItemAccessibility(QListWidgetItem* item);
    void updateGridGeometry();
    void syncAccessibilitySummary();
    void emitSelectionChangedIfNeeded();

    SelectionMode m_selectionMode = SelectionMode::SingleSelection;
    int m_columns = 3;
    QSize m_cellExtent = QSize(160, 112);
    QString m_accessibilitySummary;
};

} // namespace QtMaterial
