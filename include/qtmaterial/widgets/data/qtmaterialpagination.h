#pragma once

#include <QList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QComboBox;
class QLabel;
class QToolButton;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialPagination : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(int pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)
    Q_PROPERTY(int totalCount READ totalCount WRITE setTotalCount NOTIFY totalCountChanged)

public:
    explicit QtMaterialPagination(QWidget* parent = nullptr);

    int page() const noexcept;
    int pageSize() const noexcept;
    int totalCount() const noexcept;
    int pageCount() const noexcept;

    void setPage(int page);
    void setPageSize(int pageSize);
    void setTotalCount(int totalCount);

    QList<int> pageSizeOptions() const;
    void setPageSizeOptions(const QList<int>& options);

    QString rangeText() const;

Q_SIGNALS:
    void pageChanged(int page);
    void pageSizeChanged(int pageSize);
    void totalCountChanged(int totalCount);

private:
    void updateUi();

    int m_page = 1;
    int m_pageSize = 25;
    int m_totalCount = 0;
    QToolButton* m_firstButton = nullptr;
    QToolButton* m_previousButton = nullptr;
    QToolButton* m_nextButton = nullptr;
    QToolButton* m_lastButton = nullptr;
    QLabel* m_rangeLabel = nullptr;
    QComboBox* m_pageSizeCombo = nullptr;
};

} // namespace QtMaterial
