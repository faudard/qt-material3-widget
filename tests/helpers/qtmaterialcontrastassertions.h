#pragma once

#include <QColor>
#include <QtTest/QtTest>

#include "qtmaterial/theme/qtmaterialaccessibilitytokens.h"

inline void qtm3AssertContrastAtLeast(
    const QColor& foreground,
    const QColor& background,
    qreal minimumRatio,
    const char* foregroundExpr,
    const char* backgroundExpr,
    const char* file,
    int line)
{
    const qreal actual = QtMaterial::contrastRatio(foreground, background);
    if (actual + 0.0001 < minimumRatio) {
        QTest::qFail(qPrintable(QStringLiteral("contrast %1/%2 was %3, expected at least %4")
                         .arg(QString::fromLatin1(foregroundExpr),
                              QString::fromLatin1(backgroundExpr))
                         .arg(actual, 0, 'f', 2)
                         .arg(minimumRatio, 0, 'f', 2)),
                     file,
                     line);
    }
}

#define QTM3_COMPARE_CONTRAST_AT_LEAST(fg, bg, ratio) \
    qtm3AssertContrastAtLeast((fg), (bg), (ratio), #fg, #bg, __FILE__, __LINE__)
