#pragma once
#include <memory>

#include <QHash>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {
class QtMaterialTextFieldValidationGroupPrivate;

class QtMaterialOutlinedTextField;

/**
 * @brief Lightweight validation coordinator for Material text fields.
 *
 * The group does not own the fields. It only forwards validation/reset requests,
 * observes acceptable-input changes so form code can enable or disable a submit
 * button from one place, and can produce a stable validation summary for forms.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTextFieldValidationGroup : public QObject
{
    Q_OBJECT

public:
    explicit QtMaterialTextFieldValidationGroup(QObject* parent = nullptr);
    ~QtMaterialTextFieldValidationGroup() override;

    void addField(QtMaterialOutlinedTextField* field);
    void removeField(QtMaterialOutlinedTextField* field);
    void clear();

    QList<QtMaterialOutlinedTextField*> fields() const;
    int count() const noexcept;
    bool isEmpty() const noexcept;

    bool isAcceptable() const noexcept;
    bool validateAll();
    void resetValidationFeedback();

    QtMaterialOutlinedTextField* firstInvalidField() const;
    QList<QtMaterialOutlinedTextField*> invalidFields() const;
    bool focusFirstInvalidField();

    void setFieldLabel(QtMaterialOutlinedTextField* field, const QString& label);
    QString fieldLabel(QtMaterialOutlinedTextField* field) const;

    void setFieldErrorMessage(QtMaterialOutlinedTextField* field, const QString& message);
    QString fieldErrorMessage(QtMaterialOutlinedTextField* field) const;

    QStringList invalidFieldLabels() const;
    QStringList validationSummary() const;
    QString validationSummaryText() const;
    QString validationSummaryText(const QString& separator) const;

signals:
    void fieldAdded(QtMaterial::QtMaterialOutlinedTextField* field);
    void fieldRemoved(QtMaterial::QtMaterialOutlinedTextField* field);
    void acceptableChanged(bool acceptable);
    void validationRequested(bool acceptable);
    void validationSummaryChanged(const QStringList& summary);

private:
    std::unique_ptr<QtMaterialTextFieldValidationGroupPrivate> d_ptr;

};

} // namespace QtMaterial
