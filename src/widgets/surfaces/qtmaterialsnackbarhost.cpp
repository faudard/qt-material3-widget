#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"

#include <QTimer>
#include <QWidget>
#include <memory>

namespace QtMaterial {


class QtMaterialSnackbarHostPrivate {
public:
 QPointer<QWidget> host;
 QPointer<QtMaterialSnackbar> snackbar;
 QQueue<SnackbarRequest> queue;
 bool showing = false;
};

QtMaterialSnackbarHost::QtMaterialSnackbarHost(QWidget* host, QObject* parent)
    : QObject(parent)
 , d_ptr(std::make_unique<QtMaterialSnackbarHostPrivate>())
{
 d_ptr->host = host;
    d_ptr->snackbar = new QtMaterialSnackbar(host);
    d_ptr->snackbar->setHostWidget(host);

    connect(d_ptr->snackbar, &QtMaterialSnackbar::dismissed,
            this, [this](SnackbarDismissReason) {
                d_ptr->showing = false;

                if (!d_ptr->queue.isEmpty()) {
                    QTimer::singleShot(0, this, [this]() { showNext(); });
                }
            });
}

void QtMaterialSnackbarHost::showMessage(const SnackbarRequest& request, bool replaceCurrent)
{
    if (!d_ptr->snackbar) {
        return;
    }

    if (!d_ptr->showing) {
        d_ptr->snackbar->setRequest(request);
        d_ptr->showing = true;
        d_ptr->snackbar->showSnackbar();
        return;
    }

    if (replaceCurrent) {
        d_ptr->queue.prepend(request);
        d_ptr->snackbar->dismiss(SnackbarDismissReason::Consecutive);
        return;
    }

    d_ptr->queue.enqueue(request);
}

void QtMaterialSnackbarHost::dismissCurrent()
{
    if (d_ptr->snackbar && d_ptr->showing) {
        d_ptr->snackbar->dismiss(SnackbarDismissReason::Manual);
    }
}

bool QtMaterialSnackbarHost::isShowing() const noexcept
{
    return d_ptr->showing;
}

int QtMaterialSnackbarHost::pendingCount() const noexcept
{
    return d_ptr->queue.size();
}

QWidget* QtMaterialSnackbarHost::hostWidget() const noexcept
{
    return d_ptr->host;
}

QtMaterialSnackbar* QtMaterialSnackbarHost::snackbar() const noexcept
{
    return d_ptr->snackbar;
}

void QtMaterialSnackbarHost::showNext()
{
    if (!d_ptr->snackbar || d_ptr->queue.isEmpty()) {
        return;
    }

    const SnackbarRequest request = d_ptr->queue.dequeue();
    d_ptr->snackbar->setRequest(request);
    d_ptr->showing = true;
    d_ptr->snackbar->showSnackbar();
}

} // namespace QtMaterial