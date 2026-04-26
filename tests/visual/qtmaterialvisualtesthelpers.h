#pragma once

#include <QByteArray>
#include <QColor>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QtTest/QtTest>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialtheme.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"

namespace QtMaterialVisualTest {

inline QString visualSourceDir()
{
#if defined(QTMATERIAL3_VISUAL_SOURCE_DIR)
    return QString::fromUtf8(QTMATERIAL3_VISUAL_SOURCE_DIR);
#else
    return QString::fromLocal8Bit(qgetenv("QTMATERIAL3_VISUAL_SOURCE_DIR"));
#endif
}

inline bool updateGoldens()
{
    const QByteArray value = qgetenv("QTMATERIAL3_UPDATE_VISUAL_GOLDENS").trimmed().toLower();
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

inline bool strictGoldens()
{
    const QByteArray value = qgetenv("QTMATERIAL3_VISUAL_STRICT").trimmed().toLower();
    return value == "1" || value == "true" || value == "yes" || value == "on";
}

inline int maxAllowedDifference()
{
    bool ok = false;
    const int value = QString::fromLocal8Bit(qgetenv("QTMATERIAL3_VISUAL_MAX_DIFF")).toInt(&ok);
    return ok ? qMax(0, value) : 0;
}

inline QString artifactsDir()
{
    QString base = QString::fromLocal8Bit(qgetenv("QTMATERIAL3_VISUAL_ARTIFACTS_DIR"));
    if (base.isEmpty()) {
        base = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("visual-artifacts"));
    }
    QDir().mkpath(base);
    return base;
}

inline QString goldenDir()
{
    const QString source = visualSourceDir();
    const QString dir = source.isEmpty()
        ? QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("visual/goldens"))
        : QDir(source).absoluteFilePath(QStringLiteral("goldens"));
    QDir().mkpath(dir);
    return dir;
}

inline QByteArray sha256(const QImage& image)
{
    QImage canonical = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(reinterpret_cast<const char*>(canonical.constBits()), canonical.sizeInBytes());
    return hash.result().toHex();
}

inline bool savePng(const QString& path, const QImage& image, QString* errorString = nullptr)
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    if (!image.save(path, "PNG")) {
        if (errorString) {
            *errorString = QStringLiteral("Could not save PNG %1").arg(path);
        }
        return false;
    }
    return true;
}

inline QImage diffImage(const QImage& actual, const QImage& expected, int* differentPixels = nullptr)
{
    const QSize size(qMax(actual.width(), expected.width()), qMax(actual.height(), expected.height()));
    QImage diff(size, QImage::Format_ARGB32_Premultiplied);
    diff.fill(QColor(0, 0, 0, 0));

    int count = 0;
    for (int y = 0; y < size.height(); ++y) {
        for (int x = 0; x < size.width(); ++x) {
            const QColor a = x < actual.width() && y < actual.height()
                ? QColor::fromRgba(actual.pixel(x, y))
                : QColor(255, 0, 255, 255);
            const QColor e = x < expected.width() && y < expected.height()
                ? QColor::fromRgba(expected.pixel(x, y))
                : QColor(0, 255, 255, 255);
            if (a.rgba() != e.rgba()) {
                ++count;
                diff.setPixelColor(x, y, QColor(255, 0, 0, 180));
            }
        }
    }
    if (differentPixels) {
        *differentPixels = count;
    }
    return diff;
}

inline void writeManifestEntry(const QString& name, const QImage& image, const QString& kind)
{
    const QString path = QDir(artifactsDir()).absoluteFilePath(QStringLiteral("manifest.json"));
    QJsonArray entries;
    QFile file(path);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        const QJsonDocument oldDoc = QJsonDocument::fromJson(file.readAll());
        if (oldDoc.isArray()) {
            entries = oldDoc.array();
        }
        file.close();
    }

    QJsonObject object;
    object.insert(QStringLiteral("name"), name);
    object.insert(QStringLiteral("kind"), kind);
    object.insert(QStringLiteral("width"), image.width());
    object.insert(QStringLiteral("height"), image.height());
    object.insert(QStringLiteral("sha256"), QString::fromLatin1(sha256(image)));
    entries.append(object);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(entries).toJson(QJsonDocument::Indented));
    }
}

inline void verifyOrUpdateGolden(const QString& caseName, const QImage& actual)
{
    const QString goldenPath = QDir(goldenDir()).absoluteFilePath(caseName + QStringLiteral(".png"));
    const QString actualPath = QDir(artifactsDir()).absoluteFilePath(caseName + QStringLiteral(".actual.png"));
    const QString diffPath = QDir(artifactsDir()).absoluteFilePath(caseName + QStringLiteral(".diff.png"));

    QString error;
    QVERIFY2(savePng(actualPath, actual, &error), qPrintable(error));
    writeManifestEntry(caseName, actual, QStringLiteral("actual"));

    if (updateGoldens()) {
        QVERIFY2(savePng(goldenPath, actual, &error), qPrintable(error));
        return;
    }

    if (!QFileInfo::exists(goldenPath)) {
        const QString message = QStringLiteral(
            "Missing visual golden %1. Run with QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 to create it. Actual artifact: %2")
            .arg(goldenPath, actualPath);
        if (strictGoldens()) {
            QFAIL(qPrintable(message));
        }
        QSKIP(qPrintable(message));
    }

    QImage expected(goldenPath);
    QVERIFY2(!expected.isNull(), qPrintable(QStringLiteral("Could not load visual golden %1").arg(goldenPath)));

    int differences = 0;
    QImage diff = diffImage(actual, expected, &differences);
    const int allowed = maxAllowedDifference();
    if (differences > allowed) {
        QVERIFY2(savePng(diffPath, diff, &error), qPrintable(error));
        const QString message = QStringLiteral(
            "Visual golden mismatch for %1: %2 pixels differ, allowed %3. Actual: %4 Diff: %5 Golden: %6")
            .arg(caseName)
            .arg(differences)
            .arg(allowed)
            .arg(actualPath, diffPath, goldenPath);
        QFAIL(qPrintable(message));
    }
}

inline QImage renderTokenBoard(const QtMaterial::Theme& theme, const QSize& size = QSize(720, 420))
{
    using namespace QtMaterial;
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    const ColorScheme& scheme = theme.colorScheme();
    image.fill(scheme.color(ColorRole::Surface));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QFont font(QStringLiteral("Sans Serif"));
    font.setPixelSize(14);
    painter.setFont(font);

    const QList<QPair<QString, ColorRole>> swatches = {
        {QStringLiteral("primary"), ColorRole::Primary},
        {QStringLiteral("onPrimary"), ColorRole::OnPrimary},
        {QStringLiteral("primaryContainer"), ColorRole::PrimaryContainer},
        {QStringLiteral("secondary"), ColorRole::Secondary},
        {QStringLiteral("tertiary"), ColorRole::Tertiary},
        {QStringLiteral("surface"), ColorRole::Surface},
        {QStringLiteral("surfaceContainer"), ColorRole::SurfaceContainer},
        {QStringLiteral("error"), ColorRole::Error},
    };

    int x = 24;
    int y = 24;
    for (const auto& entry : swatches) {
        const QColor color = scheme.color(entry.second);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawRoundedRect(QRect(x, y, 78, 64), 16, 16);
        painter.setPen(scheme.color(ColorRole::OnSurface));
        painter.drawText(QRect(x, y + 70, 92, 40), Qt::AlignLeft | Qt::AlignTop, entry.first);
        x += 86;
    }

    y = 150;
    x = 24;
    const QColor primary = scheme.color(ColorRole::Primary);
    const QColor onPrimary = scheme.color(ColorRole::OnPrimary);
    const QColor surfaceContainer = scheme.color(ColorRole::SurfaceContainer);
    const QColor outline = scheme.color(ColorRole::Outline);
    const QColor onSurface = scheme.color(ColorRole::OnSurface);

    auto drawButton = [&](const QRect& rect, const QString& text, const QColor& fill, const QColor& stroke, const QColor& label) {
        QPainterPath path;
        path.addRoundedRect(rect, 20, 20);
        painter.setPen(stroke.isValid() ? QPen(stroke, 1.5) : Qt::NoPen);
        painter.setBrush(fill);
        painter.drawPath(path);
        painter.setPen(label);
        painter.drawText(rect, Qt::AlignCenter, text);
    };

    drawButton(QRect(x, y, 150, 48), QStringLiteral("Filled"), primary, QColor(), onPrimary);
    drawButton(QRect(x + 166, y, 150, 48), QStringLiteral("Outlined"), QColor(0, 0, 0, 0), outline, primary);
    drawButton(QRect(x + 332, y, 150, 48), QStringLiteral("Elevated"), surfaceContainer, QColor(), primary);

    y += 88;
    painter.setPen(QPen(outline, 1));
    painter.setBrush(surfaceContainer);
    painter.drawRoundedRect(QRect(24, y, 320, 116), 20, 20);
    painter.setPen(onSurface);
    QFont title = font;
    title.setPixelSize(20);
    title.setBold(true);
    painter.setFont(title);
    painter.drawText(QRect(48, y + 20, 270, 28), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Card preview"));
    painter.setFont(font);
    painter.drawText(QRect(48, y + 58, 270, 44), Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                     QStringLiteral("Shape, surface, outline, and typography are captured deterministically."));

    const auto& accessibility = theme.accessibility();
    QRect focusRect(394, y + 16, 246, 68);
    painter.setPen(QPen(accessibility.focusRing.color, accessibility.focusRing.width));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(focusRect, 18, 18);
    painter.setPen(onSurface);
    painter.drawText(focusRect.adjusted(14, 0, -14, 0), Qt::AlignCenter,
                     QStringLiteral("Focus ring %1px").arg(accessibility.focusRing.width));

    painter.end();
    return image;
}

inline QImage renderWidget(QWidget* widget)
{
    widget->setAttribute(Qt::WA_DontShowOnScreen, true);
    widget->resize(widget->sizeHint().expandedTo(QSize(720, 420)));
    widget->show();
    QTest::qWaitForWindowExposed(widget, 100);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    return widget->grab().toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

} // namespace QtMaterialVisualTest
