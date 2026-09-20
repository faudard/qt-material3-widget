#include "themejsonview.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

using namespace QtMaterial;

ThemeJsonView::ThemeJsonView(QWidget* parent)
    : QWidget(parent)
    , m_editor(new QPlainTextEdit(this))
    , m_copyButton(new QPushButton(tr("Copy JSON"), this))
    , m_importButton(new QPushButton(tr("Import JSON"), this))
    , m_exportButton(new QPushButton(tr("Export JSON"), this))
    , m_validateButton(new QPushButton(tr("Validate"), this))
    , m_applyButton(new QPushButton(tr("Apply JSON"), this))
    , m_validationLabel(new QLabel(this))
{
    m_editor->setReadOnly(false);
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_editor->setMinimumHeight(260);

    auto* actions = new QHBoxLayout();
    actions->addWidget(m_importButton);
    actions->addWidget(m_exportButton);
    actions->addWidget(m_validateButton);
    actions->addWidget(m_applyButton);
    actions->addStretch(1);
    actions->addWidget(m_copyButton);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);
    root->addLayout(actions);
    root->addWidget(m_validationLabel);
    root->addWidget(m_editor, 1);

    connect(m_copyButton, &QPushButton::clicked, this, [this]() {
        if (auto* clipboard = QGuiApplication::clipboard()) {
            clipboard->setText(m_editor->toPlainText());
        }
        emit copyRequested();
    });

    connect(m_importButton, &QPushButton::clicked, this, &ThemeJsonView::importRequested);
    connect(m_exportButton, &QPushButton::clicked, this, &ThemeJsonView::exportRequested);
    connect(m_validateButton, &QPushButton::clicked, this, [this]() {
        emit validateRequested(m_editor->toPlainText().toUtf8());
    });
    connect(m_applyButton, &QPushButton::clicked, this, [this]() {
        emit applyRequested(m_editor->toPlainText().toUtf8());
    });

    m_validationLabel->setText(tr("Strict JSON validation available."));
    m_editor->setPlainText(QString::fromUtf8(
        ThemeSerializer::toJson(ThemeManager::instance().theme(), QJsonDocument::Indented)));
}

void ThemeJsonView::applyTheme(const Theme& theme)
{
    m_editor->setPlainText(QString::fromUtf8(
        ThemeSerializer::toJson(theme, QJsonDocument::Indented)));
}

void ThemeJsonView::setValidationResult(bool valid, const QString& message)
{
    m_validationLabel->setText(
        valid ? tr("Valid Theme JSON v1")
              : tr("Invalid: %1").arg(message));
}
