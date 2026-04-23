#include "themejsonview.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

using namespace QtMaterial;

ThemeJsonView::ThemeJsonView(QWidget* parent)
    : QWidget(parent)
    , m_editor(new QPlainTextEdit(this))
    , m_copyButton(new QPushButton(tr("Copy JSON"), this))
    , m_importButton(new QPushButton(tr("Import JSON"), this))
    , m_exportButton(new QPushButton(tr("Export JSON"), this))
{
    m_editor->setReadOnly(true);
    m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_editor->setMinimumHeight(260);

    auto* actions = new QHBoxLayout();
    actions->addWidget(m_importButton);
    actions->addWidget(m_exportButton);
    actions->addStretch(1);
    actions->addWidget(m_copyButton);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);
    root->addLayout(actions);
    root->addWidget(m_editor, 1);

    connect(m_copyButton, &QPushButton::clicked, this, [this]() {
        if (auto* clipboard = QGuiApplication::clipboard()) {
            clipboard->setText(m_editor->toPlainText());
        }
        emit copyRequested();
    });

    connect(m_importButton, &QPushButton::clicked, this, &ThemeJsonView::importRequested);
    connect(m_exportButton, &QPushButton::clicked, this, &ThemeJsonView::exportRequested);

    m_editor->setPlainText(QString::fromUtf8(
        ThemeManager::instance().exportThemeJson(QJsonDocument::Indented)));
}

void ThemeJsonView::applyTheme(const Theme&)
{
    m_editor->setPlainText(QString::fromUtf8(
        ThemeManager::instance().exportThemeJson(QJsonDocument::Indented)));
}
