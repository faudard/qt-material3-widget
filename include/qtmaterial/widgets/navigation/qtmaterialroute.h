#pragma once

#include <QString>

#include <utility>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialRoute
{
    Q_GADGET
    Q_PROPERTY(QString path READ path)

public:
    QtMaterialRoute() = default;

    explicit QtMaterialRoute(QString path)
        : m_path(normalizedPath(std::move(path)))
    {
    }

    static QString normalizedPath(QString path)
    {
        path = path.trimmed();
        path.replace(
            QLatin1Char('\\'),
            QLatin1Char('/'));
        if (path.isEmpty()) {
            return {};
        }

        QString normalized;
        normalized.reserve(path.size() + 1);
        bool previousWasSlash = false;

        for (const QChar character : path) {
            const bool isSlash =
                character == QLatin1Char('/');
            if (isSlash) {
                if (!previousWasSlash) {
                    normalized.append(
                        QLatin1Char('/'));
                }
            } else {
                normalized.append(character);
            }
            previousWasSlash = isSlash;
        }

        while (normalized.size() > 1
               && normalized.endsWith(
                   QLatin1Char('/'))) {
            normalized.chop(1);
        }

        if (!normalized.startsWith(
                QLatin1Char('/'))) {
            normalized.prepend(
                QLatin1Char('/'));
        }

        return normalized;
    }

    QString path() const
    {
        return m_path;
    }

    QString toString() const
    {
        return m_path;
    }

    bool isValid() const
    {
        return !m_path.isEmpty();
    }

    bool operator==(
        const QtMaterialRoute& other) const
    {
        return m_path == other.m_path;
    }

    bool operator!=(
        const QtMaterialRoute& other) const
    {
        return !(*this == other);
    }

private:
    QString m_path;
};

} // namespace QtMaterial

Q_DECLARE_METATYPE(QtMaterial::QtMaterialRoute)
