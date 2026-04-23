#pragma once

#include <QPointer>
#include <QQueue>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"

class QWidget;

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSnackbarHost : public QObject
{
    Q_OBJECT

public:
    explicit QtMaterialSnackbarHost(QWidget* host, QObject* parent = nullptr);

    void showMessage(const SnackbarRequest& request, bool replaceCurrent = false);
    void dismissCurrent();

    bool isShowing() const noexcept;
    int pendingCount() const noexcept;

    QWidget* hostWidget() const noexcept;
    QtMaterialSnackbar* snackbar() const noexcept;

private:
    void showNext();

private:
    QPointer<QWidget> m_host;
    QPointer<QtMaterialSnackbar> m_snackbar;
    QQueue<SnackbarRequest> m_queue;
    bool m_showing = false;
};

} // namespace QtMaterial