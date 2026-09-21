#pragma once

#include <QString>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QLineEdit;
class QToolButton;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSearchBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(bool clearButtonVisible READ isClearButtonVisible WRITE setClearButtonVisible)

public:
    explicit QtMaterialSearchBar(QWidget* parent = nullptr);
    ~QtMaterialSearchBar() override;

    QString text() const;
    void setText(const QString& text);

    QString placeholderText() const;
    void setPlaceholderText(const QString& text);

    bool isClearButtonVisible() const noexcept;
    void setClearButtonVisible(bool visible);

    QLineEdit* lineEdit() const noexcept;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void textChanged(const QString& text);
    void searchRequested(const QString& text);
    void cleared();

private:
    void syncClearButton();

    QLineEdit* m_lineEdit = nullptr;
    QToolButton* m_clearButton = nullptr;
    bool m_clearButtonVisible = true;
};

} // namespace QtMaterial
