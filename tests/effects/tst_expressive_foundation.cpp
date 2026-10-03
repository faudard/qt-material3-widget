#include <QTest>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeserializer.h"
#include "qtmaterial/theme/private/qtmaterialtokenids_p.h"
#include "qtmaterial/effects/private/qtmaterialshapemorph_p.h"

using namespace QtMaterial;

class tst_ExpressiveFoundation : public QObject
{
    Q_OBJECT
private slots:
    void schemesProvideSixProfilesWithoutRenumberingLegacyTokens()
    {
        static_assert(tokenId(MotionToken::Long4).raw() == 0x0500000Cu, "Legacy motion IDs must stay stable");
        static_assert(tokenId(MotionToken::SpatialFast).raw() == 0x0500000Du, "New motion IDs must append");
        Theme standard;
        QCOMPARE(standard.motionScheme(), MotionScheme::Standard);
        Theme expressive = standard;
        expressive.setMotionScheme(MotionScheme::Expressive);
        QVERIFY(standard != expressive);
        QCOMPARE(expressive.options().motionScheme, MotionScheme::Expressive);
        QCOMPARE(expressive.motion().style(MotionToken::Short1).durationMs, standard.motion().style(MotionToken::Short1).durationMs);
        QVERIFY(expressive.motion().style(MotionToken::SpatialFast).durationMs < expressive.motion().style(MotionToken::SpatialDefault).durationMs);
        QVERIFY(expressive.motion().style(MotionToken::SpatialDefault).durationMs < expressive.motion().style(MotionToken::SpatialSlow).durationMs);
        QCOMPARE(expressive.motion().style(MotionToken::SpatialFast).easing.type(), QEasingCurve::OutBack);
        for (MotionToken token : {MotionToken::EffectsFast, MotionToken::EffectsDefault, MotionToken::EffectsSlow}) {
            const auto style = expressive.motion().style(token);
            for (int i = 0; i <= 100; ++i) {
                const qreal value = style.easing.valueForProgress(i / 100.0);
                QVERIFY(value >= 0 && value <= 1);
            }
        }
    }

    void builderAndSerializerPreserveExpressiveScheme()
    {
        ThemeOptions options;
        options.motionScheme = MotionScheme::Expressive;
        ThemeBuilder builder;
        const Theme theme = builder.build(options);
        QCOMPARE(theme.motionScheme(), MotionScheme::Expressive);
        QJsonObject object = ThemeSerializer::toJsonObject(theme);
        bool ok = false;
        QString error;
        const Theme restored = ThemeSerializer::fromJsonObject(object, ThemeReadMode::Strict, &ok, &error);
        QVERIFY2(ok, qPrintable(error));
        QCOMPARE(restored.motionScheme(), MotionScheme::Expressive);
        for (MotionToken token : allMotionTokens()) {
            QCOMPARE(restored.motion().style(token).durationMs, theme.motion().style(token).durationMs);
            QCOMPARE(restored.motion().style(token).easing.type(), theme.motion().style(token).easing.type());
        }
        QJsonObject source = object.value(QStringLiteral("source")).toObject();
        source.insert(QStringLiteral("motionScheme"), QStringLiteral("Unknown"));
        object.insert(QStringLiteral("source"), source);
        ThemeSerializer::fromJsonObject(object, ThemeReadMode::Strict, &ok, &error);
        QVERIFY(!ok);
    }

    void legacyThemeFilesDefaultToStandard()
    {
        QJsonObject object = ThemeSerializer::toJsonObject(Theme());
        QJsonObject source = object.value(QStringLiteral("source")).toObject();
        source.remove(QStringLiteral("motionScheme"));
        object.insert(QStringLiteral("source"), source);
        QJsonObject resolved = object.value(QStringLiteral("resolved")).toObject();
        QJsonObject motion = resolved.value(QStringLiteral("motionTokens")).toObject();
        for (const QString& name : {QStringLiteral("SpatialFast"), QStringLiteral("SpatialDefault"), QStringLiteral("SpatialSlow"), QStringLiteral("EffectsFast"), QStringLiteral("EffectsDefault"), QStringLiteral("EffectsSlow")}) { motion.remove(name); }
        resolved.insert(QStringLiteral("motionTokens"), motion);
        object.insert(QStringLiteral("resolved"), resolved);
        bool ok = false;
        QString error;
        const Theme restored = ThemeSerializer::fromJsonObject(object, ThemeReadMode::Strict, &ok, &error);
        QVERIFY2(ok, qPrintable(error));
        QCOMPARE(restored.motionScheme(), MotionScheme::Standard);
        QVERIFY(restored.motion().contains(MotionToken::SpatialFast));
    }

    void reducedMotionIncludesAllProfilesAndSurvivesSchemeChanges()
    {
        Theme theme;
        theme.accessibility().reducedMotion = true;
        theme.setMotionScheme(MotionScheme::Expressive);
        for (MotionToken token : allMotionTokens()) {
            QCOMPARE(theme.motion().style(token).durationMs, 0);
            QCOMPARE(theme.motion().style(token).easing.type(), QEasingCurve::Linear);
        }
        Theme copy = theme;
        MotionStyle changed = copy.motion().style(MotionToken::EffectsSlow);
        changed.durationMs = 12;
        copy.motion().setStyle(MotionToken::EffectsSlow, changed);
        QVERIFY(copy != theme);
    }

    void morphKeepsEndpointsAndInterpolatesRoundedOutlines()
    {
        const auto source = QtMaterialShapeMorph::roundedRectangle(QRectF(0, 0, 160, 48), 24);
        const auto target = QtMaterialShapeMorph::roundedRectangle(QRectF(0, 0, 160, 48), 6);
        QtMaterialShapeMorph morph;
        QVERIFY(morph.setShapes(source, target));
        QCOMPARE(morph.pathAt(0), source);
        QCOMPARE(morph.pathAt(1), target);
        const QPainterPath middle = morph.pathAt(0.5);
        QVERIFY(!middle.isEmpty());
        QVERIFY(middle.contains(QPointF(80, 24)));
        QVERIFY(middle != source && middle != target);
        QVERIFY(middle.boundingRect().width() <= 160.01);
        QVERIFY(middle.boundingRect().height() <= 48.01);
    }

    void morphAlignsDifferentTopologyAndWinding()
    {
        QPainterPath circle;
        circle.addEllipse(QRectF(0, 0, 100, 100));
        QPainterPath triangle;
        triangle.moveTo(50, 0); triangle.lineTo(0, 100); triangle.lineTo(100, 100); triangle.closeSubpath();
        QtMaterialShapeMorph morph(128);
        QVERIFY(morph.setShapes(circle, triangle));
        QVERIFY(morph.pathAt(0.5).contains(QPointF(50, 50)));
        QVERIFY(morph.setShapes(circle, circle.toReversed()));
        const QRectF bounds = morph.pathAt(0.5).boundingRect();
        QVERIFY(bounds.width() > 98 && bounds.height() > 98);
    }

    void morphRejectsUnsupportedContoursAndBoundsProgress()
    {
        const QPainterPath shape = QtMaterialShapeMorph::roundedRectangle(QRectF(0, 0, 100, 50), 10);
        QtMaterialShapeMorph morph;
        QVERIFY(morph.setShapes(shape, shape));
        QCOMPARE(morph.pathAt(-1), shape);
        QCOMPARE(morph.pathAt(2), shape);
        QPainterPath multiple = shape;
        multiple.addRect(QRectF(200, 0, 20, 20));
        QVERIFY(!morph.setShapes(shape, multiple));
        QVERIFY(!morph.isValid());
        QVERIFY(morph.pathAt(0.5).isEmpty());
        QVERIFY(!morph.setShapes(QPainterPath(), shape));
    }
};

QTEST_MAIN(tst_ExpressiveFoundation)
#include "tst_expressive_foundation.moc"
