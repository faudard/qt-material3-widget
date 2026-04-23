#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"

#include <QTimer>
#include <QWidget>

namespace QtMaterial {

QtMaterialSnackbarHost::QtMaterialSnackbarHost(QWidget* host, QObject* parent)
    : QObject(parent)
    , m_host(host)
{
    m_snackbar = new QtMaterialSnackbar(host);
    m_snackbar->setHostWidget(host);

    connect(m_snackbar, &QtMaterialSnackbar::dismissed,
            this, [this](SnackbarDismissReason) {
                m_showing = false;

                if (!m_queue.isEmpty()) {
                    QTimer::singleShot(0, this, [this]() { showNext(); });
                }
            });
}

void QtMaterialSnackbarHost::showMessage(const SnackbarRequest& request, bool replaceCurrent)
{
    if (!m_snackbar) {
        return;
    }

    if (!m_showing) {
        m_snackbar->setRequest(request);
        m_showing = true;
        m_snackbar->showSnackbar();
        return;
    }

    if (replaceCurrent) {
        m_queue.prepend(request);
        m_snackbar->dismiss(SnackbarDismissReason::Consecutive);
        return;
    }

    m_queue.enqueue(request);
}

void QtMaterialSnackbarHost::dismissCurrent()
{
    if (m_snackbar && m_showing) {
        m_snackbar->dismiss(SnackbarDismissReason::Manual);
    }
}

bool QtMaterialSnackbarHost::isShowing() const noexcept
{
    return m_showing;
}

int QtMaterialSnackbarHost::pendingCount() const noexcept
{
    return m_queue.size();
}

QWidget* QtMaterialSnackbarHost::hostWidget() const noexcept
{
    return m_host;
}

QtMaterialSnackbar* QtMaterialSnackbarHost::snackbar() const noexcept
{
    return m_snackbar;
}

void QtMaterialSnackbarHost::showNext()
{
    if (!m_snackbar || m_queue.isEmpty()) {
        return;
    }

    const SnackbarRequest request = m_queue.dequeue();
    m_snackbar->setRequest(request);
    m_showing = true;
    m_snackbar->showSnackbar();
}

} // namespace QtMaterial