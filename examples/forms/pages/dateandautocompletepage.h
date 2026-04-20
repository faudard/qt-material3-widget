#pragma once

#include <QWidget>

class DateAndAutocompletePage : public QWidget
{
    Q_OBJECT
public:
    explicit DateAndAutocompletePage(QWidget *parent = nullptr);
    ~DateAndAutocompletePage() override;
};
