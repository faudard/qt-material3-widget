
#include "qtmaterial/widgets/inputs/qtmaterialdatefield.h"

#include <QLineEdit>
#include <QToolButton>
#include <memory>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPalette>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QPointer>
#include "qtmaterial/specs/qtmaterialdatefieldspecresolver.h"


namespace {

QIcon makeCalendarIcon(
    const QColor& color,
    int extent)
{
    const int size = qMax(12, extent);
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    QPen pen(color, 1.7);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const QRectF frame(
        2.5,
        4.0,
        size - 5.0,
        size - 6.5);
    painter.drawRoundedRect(
        frame,
        2.0,
        2.0);
    painter.drawLine(
        QPointF(frame.left(), frame.top() + 4.0),
        QPointF(frame.right(), frame.top() + 4.0));
    painter.drawLine(
        QPointF(frame.left() + 4.0, 2.5),
        QPointF(frame.left() + 4.0, 6.0));
    painter.drawLine(
        QPointF(frame.right() - 4.0, 2.5),
        QPointF(frame.right() - 4.0, 6.0));

    return QIcon(pixmap);
}

QIcon makeClearIcon(
    const QColor& color,
    int extent)
{
    const int size = qMax(12, extent);
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    QPen pen(color, 1.9);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    const qreal inset =
        qMax<qreal>(3.5, size * 0.25);
    painter.drawLine(
        QPointF(inset, inset),
        QPointF(size - inset, size - inset));
    painter.drawLine(
        QPointF(size - inset, inset),
        QPointF(inset, size - inset));

    return QIcon(pixmap);
}

} // namespace

struct QtMaterialDateFieldPrivate {

    QDate m_date;
    QDate m_minimumDate;
    QDate m_maximumDate;
    mutable QString m_lastAccessibilitySummary;
    QString m_displayFormat = QStringLiteral("yyyy-MM-dd");
    QString m_placeholderTextForDate;
    bool m_clearable = false;
    bool m_parseError = false;
    mutable bool m_specDirty = true;
    mutable QtMaterial::DateFieldSpec m_spec;
    bool m_showingClearAction = false;
    QPointer<QToolButton> m_actionButton;
    QPointer<QGraphicsOpacityEffect> m_actionOpacity;
    QPointer<QPropertyAnimation> m_actionAnimation;
    void syncEditorFromDate(QtMaterialDateField& q);
    void syncDateFromEditor(QtMaterialDateField& q);
    void updateTrailingAffordances(QtMaterialDateField& q);
    void updateAccessibilityMetadata(QtMaterialDateField& q);
    bool isDateInRange(const QDate& date) const noexcept;
    QString effectiveErrorTextForDate(const QtMaterialDateField& q) const;
    void setParseError(QtMaterialDateField& q, bool hasError);
    void notifyDateAcceptabilityIfChanged(QtMaterialDateField& q, bool previousAcceptable);

};


QtMaterialDateField::QtMaterialDateField(
    QWidget* parent)
    : QtMaterialOutlinedTextField(parent)
    , d_ptr(
          std::make_unique<QtMaterialDateFieldPrivate>())
{
    setEndActionMode(
        EndActionMode::CustomTrailingAction);
    setTrailingActionVisibleWhenEmpty(true);

    d_ptr->m_actionButton =
        findChild<QToolButton*>(
            QStringLiteral("endActionButton"));

    if (d_ptr->m_actionButton) {
        d_ptr->m_actionButton->setAutoRaise(true);

        d_ptr->m_actionOpacity =
            new QGraphicsOpacityEffect(
                d_ptr->m_actionButton);
        d_ptr->m_actionButton->setGraphicsEffect(
            d_ptr->m_actionOpacity);

        d_ptr->m_actionAnimation =
            new QPropertyAnimation(
                d_ptr->m_actionOpacity,
                "opacity",
                d_ptr->m_actionButton);
        d_ptr->m_actionAnimation->setStartValue(0.0);
        d_ptr->m_actionAnimation->setEndValue(1.0);
    }

    QObject::connect(
        this,
        &QtMaterialOutlinedTextField::
            trailingActionTriggered,
        this,
        [this]() {
            if (d_ptr->m_showingClearAction) {
                handleClearButtonClicked();
            } else {
                handleCalendarButtonClicked();
            }
        });

    if (lineEdit()) {
        QObject::connect(
            lineEdit(),
            &QLineEdit::editingFinished,
            this,
            &QtMaterialDateField::
                handleEditorEditingFinished);

        QObject::connect(
            lineEdit(),
            &QLineEdit::textChanged,
            this,
            [this]() {
                d_ptr->updateTrailingAffordances(
                    *this);
            });
    }

    ensureDateFieldSpecResolved();
    d_ptr->updateTrailingAffordances(*this);
    d_ptr->updateAccessibilityMetadata(*this);
}

QtMaterialDateField::QtMaterialDateField(const QString& labelText, QWidget* parent)
    : QtMaterialDateField(parent)
{
    setLabelText(labelText);
}

QtMaterialDateField::~QtMaterialDateField() = default;

void QtMaterialDateFieldPrivate::syncEditorFromDate(QtMaterialDateField& q)
{
    if (!q.lineEdit()) {
        return;
    }

    if (m_date.isValid()) {
        q.lineEdit()->setText(m_date.toString(m_displayFormat));
    } else {
        q.lineEdit()->clear();
        q.lineEdit()->setPlaceholderText(m_placeholderTextForDate);
    }
}

void QtMaterialDateFieldPrivate::
syncDateFromEditor(
    QtMaterialDateField& q)
{
    if (!q.lineEdit()) {
        return;
    }

    const QString text =
        q.lineEdit()->text().trimmed();

    if (text.isEmpty()) {
        q.setDate(QDate());
        setParseError(q, false);
        return;
    }

    const QDate parsed =
        QDate::fromString(
            text,
            m_displayFormat);

    if (!parsed.isValid()) {
        setParseError(q, true);
        updateTrailingAffordances(q);
        updateAccessibilityMetadata(q);
        return;
    }

    const bool wasAcceptable =
        q.isDateAcceptable();
    const bool dateChanged =
        m_date != parsed;

    m_date = parsed;
    setParseError(
        q,
        !isDateInRange(parsed));

    if (dateChanged) {
        emit q.dateChanged(m_date);
    }

    notifyDateAcceptabilityIfChanged(
        q,
        wasAcceptable);
    updateTrailingAffordances(q);
    updateAccessibilityMetadata(q);
}

void QtMaterialDateFieldPrivate::
updateTrailingAffordances(
    QtMaterialDateField& q)
{
    q.ensureDateFieldSpecResolved();

    const bool hasEditorText =
        q.lineEdit()
        && !q.lineEdit()->text().trimmed().isEmpty();
    const bool showClear =
        m_spec.preferClearAction
        && m_clearable
        && hasEditorText
        && !q.isReadOnly();

    m_showingClearAction = showClear;

    const QColor actionColor =
        q.isEnabled()
        ? (
            showClear
            ? m_spec.clearIconColor
            : m_spec.trailingIconColor)
        : m_spec.disabledIconColor;

    q.setTrailingActionVisibleWhenEmpty(
        m_spec.calendarVisibleWhenEmpty);
    q.setTrailingActionText(QString());
    q.setTrailingActionIcon(
        showClear
        ? makeClearIcon(
            actionColor,
            m_spec.actionIconSize)
        : makeCalendarIcon(
            actionColor,
            m_spec.actionIconSize));
    q.setTrailingActionToolTip(
        showClear
        ? q.tr("Clear date")
        : q.tr("Open calendar"));

    if (m_actionButton) {
        m_actionButton->setIconSize(
            QSize(
                m_spec.actionIconSize,
                m_spec.actionIconSize));
        m_actionButton->setMinimumSize(
            m_spec.actionButtonExtent,
            m_spec.actionButtonExtent);
        m_actionButton->setAccessibleName(
            showClear
            ? q.tr("Clear date")
            : q.tr("Open calendar"));
    }

    if (q.lineEdit()) {
        QPalette editorPalette =
            q.lineEdit()->palette();
        editorPalette.setColor(
            QPalette::PlaceholderText,
            m_spec.placeholderColor);
        q.lineEdit()->setPalette(editorPalette);
    }

    if (m_actionAnimation
        && m_actionOpacity
        && m_spec.hasResolvedMotionStyle) {
        m_actionAnimation->stop();
        m_actionAnimation->setDuration(
            qMax(
                0,
                m_spec.motionStyle.durationMs));
        m_actionAnimation->setEasingCurve(
            m_spec.motionStyle.easing);
        m_actionOpacity->setOpacity(0.0);
        m_actionAnimation->start();
    }
}

void QtMaterialDateFieldPrivate::updateAccessibilityMetadata(QtMaterialDateField& q)
{
    const QString summary = q.accessibilitySummary();
    q.setAccessibleName(q.labelText().isEmpty() ? q.tr("Date") : q.labelText());
    q.setAccessibleDescription(summary);
    if (m_lastAccessibilitySummary != summary) {
        m_lastAccessibilitySummary = summary;
        emit q.accessibilitySummaryChanged(summary);
    }
}

bool QtMaterialDateFieldPrivate::isDateInRange(const QDate& date) const noexcept
{
    if (!date.isValid()) {
        return true;
    }
    if (m_minimumDate.isValid() && date < m_minimumDate) {
        return false;
    }
    if (m_maximumDate.isValid() && date > m_maximumDate) {
        return false;
    }
    return true;
}

QString QtMaterialDateFieldPrivate::effectiveErrorTextForDate(const QtMaterialDateField& q) const
{
    if (m_parseError) {
        if (m_date.isValid() && !isDateInRange(m_date)) {
            if (m_minimumDate.isValid() && m_date < m_minimumDate) {
                return q.tr("Date must be on or after %1").arg(m_minimumDate.toString(m_displayFormat));
            }
            if (m_maximumDate.isValid() && m_date > m_maximumDate) {
                return q.tr("Date must be on or before %1").arg(m_maximumDate.toString(m_displayFormat));
            }
        }
        return q.tr("Enter a valid date");
    }
    return QString();
}

void QtMaterialDateFieldPrivate::setParseError(QtMaterialDateField& q, bool hasError)
{
    if (m_parseError == hasError) {
        return;
    }

    const bool wasAcceptable = q.isDateAcceptable();
    m_parseError = hasError;
    q.setHasErrorState(hasError);
    q.setErrorText(effectiveErrorTextForDate(q));
    emit q.parseErrorChanged(hasError);
    notifyDateAcceptabilityIfChanged(q, wasAcceptable);
    updateAccessibilityMetadata(q);
}

void QtMaterialDateFieldPrivate::notifyDateAcceptabilityIfChanged(QtMaterialDateField& q,
                                                                  bool previousAcceptable)
{
    Q_UNUSED(previousAcceptable);

    q.notifyDateAcceptableIfChanged();
}


QDate QtMaterialDateField::date() const noexcept
{
    return d_ptr->m_date;
}

void QtMaterialDateField::setDate(const QDate& date)
{
    if (d_ptr->m_date == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_date = date;
    d_ptr->syncEditorFromDate(*this);
    d_ptr->setParseError(*this, d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date));
    emit dateChanged(d_ptr->m_date);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateTrailingAffordances(*this);
    d_ptr->updateAccessibilityMetadata(*this);
    update();
}


QString QtMaterialDateField::displayFormat() const
{
    return d_ptr->m_displayFormat;
}

void QtMaterialDateField::setDisplayFormat(const QString& format)
{
    if (d_ptr->m_displayFormat == format) {
        return;
    }
    d_ptr->m_displayFormat = format;
    d_ptr->syncEditorFromDate(*this);
    emit displayFormatChanged(d_ptr->m_displayFormat);
    contentChangedEvent();
}

bool QtMaterialDateField::isClearable() const noexcept
{
    return d_ptr->m_clearable;
}

void QtMaterialDateField::setClearable(bool clearable)
{
    if (d_ptr->m_clearable == clearable) {
        return;
    }
    d_ptr->m_clearable = clearable;
    d_ptr->updateTrailingAffordances(*this);
    emit clearableChanged(d_ptr->m_clearable);
}

QString QtMaterialDateField::placeholderTextForDate() const
{
    return d_ptr->m_placeholderTextForDate;
}

void QtMaterialDateField::setPlaceholderTextForDate(const QString& text)
{
    if (d_ptr->m_placeholderTextForDate == text) {
        return;
    }

    d_ptr->m_placeholderTextForDate = text;
    if (!date().isValid()) {
        setPlaceholderText(text);
    }
    contentChangedEvent();
}


void QtMaterialDateField::contentChangedEvent()
{
    QtMaterialOutlinedTextField::contentChangedEvent();
    d_ptr->updateAccessibilityMetadata(*this);
}

void QtMaterialDateField::themeChangedEvent(
    const QtMaterial::Theme& theme)
{
    QtMaterialOutlinedTextField::
        themeChangedEvent(theme);

    d_ptr->m_specDirty = true;
    ensureDateFieldSpecResolved();
    d_ptr->updateTrailingAffordances(*this);
}

void QtMaterialDateField::changeEvent(
    QEvent* event)
{
    QtMaterialOutlinedTextField::changeEvent(event);

    if (!event) {
        return;
    }

    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
        d_ptr->updateTrailingAffordances(*this);
        break;

    default:
        break;
    }
}

void QtMaterialDateField::notifyDateAcceptableIfChanged()
{
    const bool acceptable = isDateAcceptable();
    if (acceptable == m_lastDateAcceptable) {
        return;
    }

    m_lastDateAcceptable = acceptable;
    emit dateAcceptableChanged(acceptable);
}

void QtMaterialDateField::handleEditorEditingFinished()
{
    d_ptr->syncDateFromEditor(*this);
}

void QtMaterialDateField::handleCalendarButtonClicked()
{
    emit calendarRequested();
}

void QtMaterialDateField::
handleClearButtonClicked()
{
    setDate(QDate());

    if (lineEdit()) {
        lineEdit()->clear();
        lineEdit()->setFocus();
    }

    d_ptr->setParseError(*this, false);
    d_ptr->updateTrailingAffordances(*this);
}

QDate QtMaterialDateField::minimumDate() const noexcept
{
    return d_ptr->m_minimumDate;
}

void QtMaterialDateField::setMinimumDate(const QDate& date)
{
    if (d_ptr->m_minimumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_minimumDate = date;
    if (d_ptr->m_minimumDate.isValid() && d_ptr->m_maximumDate.isValid() && d_ptr->m_minimumDate > d_ptr->m_maximumDate) {
        d_ptr->m_maximumDate = d_ptr->m_minimumDate;
    }

    if (d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date)) {
        d_ptr->setParseError(*this, true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateAccessibilityMetadata(*this);
    update();
}

QDate QtMaterialDateField::maximumDate() const noexcept
{
    return d_ptr->m_maximumDate;
}

void QtMaterialDateField::setMaximumDate(const QDate& date)
{
    if (d_ptr->m_maximumDate == date) {
        return;
    }

    const bool wasAcceptable = isDateAcceptable();
    d_ptr->m_maximumDate = date;
    if (d_ptr->m_minimumDate.isValid() && d_ptr->m_maximumDate.isValid() && d_ptr->m_maximumDate < d_ptr->m_minimumDate) {
        d_ptr->m_minimumDate = d_ptr->m_maximumDate;
    }

    if (d_ptr->m_date.isValid() && !d_ptr->isDateInRange(d_ptr->m_date)) {
        d_ptr->setParseError(*this, true);
    }

    emit dateRangeChanged(d_ptr->m_minimumDate, d_ptr->m_maximumDate);
    d_ptr->notifyDateAcceptabilityIfChanged(*this, wasAcceptable);
    d_ptr->updateAccessibilityMetadata(*this);
    update();
}

bool QtMaterialDateField::isDateAcceptable() const noexcept
{
    return !d_ptr->m_parseError && (!d_ptr->m_date.isValid() || d_ptr->isDateInRange(d_ptr->m_date));
}

bool QtMaterialDateField::hasParseError() const noexcept
{
    return d_ptr->m_parseError;
}

const QtMaterial::DateFieldSpec&
QtMaterialDateField::resolvedDateFieldSpec() const
{
    ensureDateFieldSpecResolved();
    return d_ptr->m_spec;
}

void QtMaterialDateField::
ensureDateFieldSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }

    const QtMaterial::DateFieldSpecResolver resolver;
    d_ptr->m_spec =
        resolver.dateFieldSpec(theme());
    d_ptr->m_specDirty = false;
}

QString QtMaterialDateField::accessibilitySummary() const
{
    QStringList parts;
    const QString label = labelText();
    if (!label.isEmpty()) {
        parts << label;
    }
    if (d_ptr->m_date.isValid()) {
        parts << d_ptr->m_date.toString(d_ptr->m_displayFormat);
    } else {
        parts << tr("No date selected");
    }
    if (d_ptr->m_minimumDate.isValid()) {
        parts << tr("Minimum %1").arg(d_ptr->m_minimumDate.toString(d_ptr->m_displayFormat));
    }
    if (d_ptr->m_maximumDate.isValid()) {
        parts << tr("Maximum %1").arg(d_ptr->m_maximumDate.toString(d_ptr->m_displayFormat));
    }
    const QString error = d_ptr->effectiveErrorTextForDate(*this);
    if (!error.isEmpty()) {
        parts << error;
    }
    return parts.join(QStringLiteral(", "));
}










