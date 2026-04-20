#pragma once

#include <QWidget>

class ValidationFormPage : public QWidget
{
    Q_OBJECT
public:
    explicit ValidationFormPage(QWidget *parent = nullptr);
    ~ValidationFormPage() override;
};
