#pragma once

#include <QWidget>

class BasicFormPage : public QWidget
{
    Q_OBJECT
public:
    explicit BasicFormPage(QWidget *parent = nullptr);
    ~BasicFormPage() override;
};
