#pragma once

#include <memory>

#include <QDialog>
#include <QIcon>
#include <QKeySequence>
#include <QList>
#include <QModelIndex>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"

class QAbstractItemModel;

namespace QtMaterial {

class QtMaterialCommandPalettePrivate;

struct QTMATERIAL3_WIDGETS_EXPORT QtMaterialCommand
{
    QString id;
    QString text;
    QString secondaryText;
    QString section;
    QStringList keywords;
    QKeySequence shortcut;
    QIcon icon;
    bool enabled = true;
};

// Providers remain application-owned. Publish one completed snapshot per request.
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCommandProvider : public QObject
{
    Q_OBJECT

public:
    explicit QtMaterialCommandProvider(QObject* parent = nullptr);
    ~QtMaterialCommandProvider() override;

    virtual void requestCommands(const QString& query, quint64 requestId) = 0;
    virtual void cancelRequest(quint64 requestId);
    virtual void activateCommand(const QString& id) = 0;

Q_SIGNALS:
    void commandsReady(quint64 requestId, const QList<QtMaterial::QtMaterialCommand>& commands);
    void requestFailed(quint64 requestId, const QString& message);
};

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCommandPalette : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QString emptyStateText READ emptyStateText WRITE setEmptyStateText)
    Q_PROPERTY(bool fuzzyMatchingEnabled READ fuzzyMatchingEnabled WRITE setFuzzyMatchingEnabled)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(QStringList favoriteCommandIds READ favoriteCommandIds WRITE setFavoriteCommandIds NOTIFY favoriteCommandIdsChanged)
    Q_PROPERTY(QStringList recentCommandIds READ recentCommandIds WRITE setRecentCommandIds NOTIFY recentCommandIdsChanged)

public:
    enum CommandRole {
        ShortcutRole = Qt::UserRole + 1,
        IdRole,
        SecondaryTextRole,
        SectionRole,
        KeywordsRole,
        FavoriteRole,
        RecentRole,
        SectionHeadingRole
    };
    Q_ENUM(CommandRole)

    explicit QtMaterialCommandPalette(QWidget* parent = nullptr);
    ~QtMaterialCommandPalette() override;

    void setSourceModel(QAbstractItemModel* model);
    QAbstractItemModel* sourceModel() const;

    void addProvider(QtMaterialCommandProvider* provider);
    void removeProvider(QtMaterialCommandProvider* provider);
    QList<QtMaterialCommandProvider*> providers() const;
    void refreshProviders();

    bool fuzzyMatchingEnabled() const;
    void setFuzzyMatchingEnabled(bool enabled);
    bool isLoading() const;

    QStringList favoriteCommandIds() const;
    void setFavoriteCommandIds(const QStringList& ids);
    void setCommandFavorite(const QString& id, bool favorite);
    QStringList recentCommandIds() const;
    void setRecentCommandIds(const QStringList& ids);
    void clearRecentCommands();

    QList<QKeySequence> activationShortcuts() const;
    void setActivationShortcuts(const QList<QKeySequence>& shortcuts);

public Q_SLOTS:
    void openPalette();

public:
    QString query() const;
    void setQuery(const QString& query);

    QString emptyStateText() const;
    void setEmptyStateText(const QString& text);

Q_SIGNALS:
    void queryChanged(const QString& query);
    void commandActivated(const QModelIndex& sourceIndex);
    void providerCommandActivated(QtMaterial::QtMaterialCommandProvider* provider, const QString& id);
    void providerFailed(QtMaterial::QtMaterialCommandProvider* provider, const QString& message);
    void loadingChanged(bool loading);
    void favoriteCommandIdsChanged(const QStringList& ids);
    void recentCommandIdsChanged(const QStringList& ids);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void activateProxyIndex(const QModelIndex& proxyIndex);
    void rebuildCommands();
    void syncResults();
    void cancelProviderRequests();
    void updateLoading();

    std::unique_ptr<QtMaterialCommandPalettePrivate> d_ptr;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::QtMaterialCommand)
Q_DECLARE_METATYPE(QList<QtMaterial::QtMaterialCommand>)
