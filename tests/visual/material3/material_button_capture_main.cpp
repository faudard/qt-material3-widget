#include <QApplication>
#include <QByteArray>
#include <QColorSpace>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFontInfo>
#include <QGuiApplication>
#include <QImageWriter>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QList>
#include <QLocale>
#include <QRawFont>
#include <QSaveFile>
#include <QScreen>
#include <QSet>
#include <QStyle>
#include <QStringList>
#include <QSysInfo>
#include <QVector>
#include <QtGlobal>

#include "qtmaterialbuttoncapturecontroller_p.h"
#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

#ifndef QTMATERIAL3_MATERIAL_BUTTON_VISUAL_CONTRACT
#error "QTMATERIAL3_MATERIAL_BUTTON_VISUAL_CONTRACT must name the source contract"
#endif

namespace {

using QtMaterial::ColorBackendPolicy;
using QtMaterial::ColorRole;
using QtMaterial::ContrastMode;
using QtMaterial::Theme;
using QtMaterial::ThemeBuilder;
using QtMaterial::ThemeMode;
using QtMaterial::ThemeOptions;
using QtMaterial::TypeRole;
using QtMaterial::TypographyStyle;
using QtMaterial::VisualCapturePrivate::ButtonCaptureController;
using QtMaterial::VisualCapturePrivate::ButtonCaptureResult;
using QtMaterial::VisualCapturePrivate::ButtonCaptureState;

constexpr int kExpectedCaseCount = 50;
constexpr qreal kNumericEpsilon = 0.001;

const QStringList kComponents = {
    QStringLiteral("button.text"),
    QStringLiteral("button.filled"),
    QStringLiteral("button.filled-tonal"),
    QStringLiteral("button.outlined"),
    QStringLiteral("button.elevated"),
};
const QStringList kStates = {
    QStringLiteral("enabled"),
    QStringLiteral("disabled"),
    QStringLiteral("hovered"),
    QStringLiteral("focused"),
    QStringLiteral("pressed"),
};
const QStringList kThemes = {
    QStringLiteral("light"),
    QStringLiteral("dark"),
};
const QStringList kRuntimeFingerprintFields = {
    QStringLiteral("runner-image"),
    QStringLiteral("operating-system"),
    QStringLiteral("operating-system-version"),
    QStringLiteral("architecture"),
    QStringLiteral("qt-version"),
    QStringLiteral("qt-build-abi"),
    QStringLiteral("compiler"),
    QStringLiteral("qpa-platform"),
    QStringLiteral("application-style"),
    QStringLiteral("font-family"),
    QStringLiteral("font-face-sha256"),
    QStringLiteral("font-style"),
    QStringLiteral("font-weight"),
    QStringLiteral("font-point-size"),
    QStringLiteral("device-pixel-ratio"),
    QStringLiteral("logical-dpi-x"),
    QStringLiteral("logical-dpi-y"),
};

struct CaptureTarget
{
    QString id;
    QString operatingSystem;
    QString runnerImage;
    QString architecture;
    QString qtVersion;
    int qtMajor = 0;
    QString compiler;
    QString fontFamily;
    int fontPointSize = 0;
};

struct CaptureCase
{
    QString id;
    QString componentId;
    QString state;
    QString theme;
    QString actualPath;
};

struct CaptureContract
{
    QByteArray bytes;
    QString id;
    QString label;
    QString manifestPath;
    QString runnerMode;
    QSize canvasSize;
    QRect widgetRect;
    qreal devicePixelRatio = 0.0;
    qreal logicalDpiX = 0.0;
    qreal logicalDpiY = 0.0;
    CaptureTarget target;
    QHash<QString, ButtonCaptureState> stateSetups;
    QVector<CaptureCase> cases;
    QStringList referenceRoots;
};

struct FontIdentity
{
    QString family;
    QByteArray faceSha256;
    QString style;
    int weight = 0;
    qreal pointSize = 0.0;

    bool operator==(const FontIdentity& other) const noexcept
    {
        return family == other.family
            && faceSha256 == other.faceSha256
            && style == other.style
            && weight == other.weight
            && qAbs(pointSize - other.pointSize) < kNumericEpsilon;
    }
};

struct CapturedCase
{
    CaptureCase contractCase;
    QImage image;
    QByteArray pixelSha256;
};

bool fail(QString* errorMessage, const QString& message)
{
    if (errorMessage) {
        *errorMessage = message;
    }
    return false;
}

bool closeEnough(qreal lhs, qreal rhs)
{
    return qAbs(lhs - rhs) < kNumericEpsilon;
}

bool requireString(
    const QJsonObject& object,
    const QString& key,
    const QString& expected,
    QString* errorMessage)
{
    const QJsonValue value = object.value(key);
    if (!value.isString() || value.toString() != expected) {
        return fail(
            errorMessage,
            QStringLiteral("contract field %1 must equal %2")
                .arg(key, expected));
    }
    return true;
}

bool requireBool(
    const QJsonObject& object,
    const QString& key,
    bool expected,
    QString* errorMessage)
{
    const QJsonValue value = object.value(key);
    if (!value.isBool() || value.toBool() != expected) {
        return fail(
            errorMessage,
            QStringLiteral("contract field %1 has an invalid Boolean value")
                .arg(key));
    }
    return true;
}

bool requireNumber(
    const QJsonObject& object,
    const QString& key,
    qreal expected,
    QString* errorMessage)
{
    const QJsonValue value = object.value(key);
    if (!value.isDouble() || !closeEnough(value.toDouble(), expected)) {
        return fail(
            errorMessage,
            QStringLiteral("contract field %1 has an invalid numeric value")
                .arg(key));
    }
    return true;
}

bool isSafeRelativePath(const QString& path)
{
    if (path.isEmpty() || QDir::isAbsolutePath(path)
        || path.contains(QLatin1Char('\\'))) {
        return false;
    }
    const QString cleaned = QDir::cleanPath(path);
    return cleaned == path
        && cleaned != QLatin1String(".")
        && cleaned != QLatin1String("..")
        && !cleaned.startsWith(QLatin1String("../"));
}

QString canonicalActualPath(
    const QString& componentId,
    const QString& state,
    const QString& theme)
{
    return QStringLiteral("actual/%1/%2/%3.png")
        .arg(componentId, state, theme);
}

bool parseStateSetup(
    const QJsonObject& object,
    ButtonCaptureState* state,
    QString* id,
    QString* errorMessage)
{
    if (!state || !id) {
        return fail(errorMessage, QStringLiteral("state setup output is null"));
    }
    *id = object.value(QStringLiteral("id")).toString();
    if (!kStates.contains(*id)
        || !object.value(QStringLiteral("enabled")).isBool()
        || !object.value(QStringLiteral("hovered")).isBool()
        || !object.value(QStringLiteral("focused")).isBool()
        || !object.value(QStringLiteral("pressed")).isBool()
        || !object.value(QStringLiteral("stateLayerProgress")).isDouble()) {
        return fail(errorMessage, QStringLiteral("invalid Button state setup"));
    }

    const QJsonObject feedback =
        object.value(QStringLiteral("feedback")).toObject();
    const QJsonObject origin =
        feedback.value(QStringLiteral("origin")).toObject();
    if (feedback.isEmpty() || origin.isEmpty()
        || !feedback.value(QStringLiteral("kind")).isString()
        || !feedback.value(QStringLiteral("progress")).isDouble()
        || !origin.value(QStringLiteral("normalizedX")).isDouble()
        || !origin.value(QStringLiteral("normalizedY")).isDouble()) {
        return fail(errorMessage, QStringLiteral("invalid Button feedback setup"));
    }

    state->enabled = object.value(QStringLiteral("enabled")).toBool();
    state->hovered = object.value(QStringLiteral("hovered")).toBool();
    state->focused = object.value(QStringLiteral("focused")).toBool();
    state->pressed = object.value(QStringLiteral("pressed")).toBool();
    state->stateLayerProgress =
        object.value(QStringLiteral("stateLayerProgress")).toDouble();
    state->feedbackKind = feedback.value(QStringLiteral("kind")).toString();
    state->feedbackProgress =
        feedback.value(QStringLiteral("progress")).toDouble();
    state->feedbackOrigin = QPointF(
        origin.value(QStringLiteral("normalizedX")).toDouble(),
        origin.value(QStringLiteral("normalizedY")).toDouble());
    return true;
}

ButtonCaptureState expectedStateSetup(const QString& id)
{
    ButtonCaptureState state;
    state.feedbackKind = QStringLiteral("none");
    state.feedbackOrigin = QPointF(0.5, 0.5);
    if (id == QLatin1String("disabled")) {
        state.enabled = false;
    } else if (id == QLatin1String("hovered")) {
        state.hovered = true;
        state.stateLayerProgress = 1.0;
        state.feedbackKind = QStringLiteral("state-layer");
        state.feedbackProgress = 1.0;
    } else if (id == QLatin1String("focused")) {
        state.focused = true;
        state.stateLayerProgress = 1.0;
        state.feedbackKind = QStringLiteral("state-layer");
        state.feedbackProgress = 1.0;
    } else if (id == QLatin1String("pressed")) {
        state.pressed = true;
        state.feedbackKind = QStringLiteral("ripple");
        state.feedbackProgress = 0.5;
    }
    return state;
}

bool sameStateSetup(
    const ButtonCaptureState& lhs,
    const ButtonCaptureState& rhs)
{
    return lhs.enabled == rhs.enabled
        && lhs.hovered == rhs.hovered
        && lhs.focused == rhs.focused
        && lhs.pressed == rhs.pressed
        && closeEnough(lhs.stateLayerProgress, rhs.stateLayerProgress)
        && lhs.feedbackKind == rhs.feedbackKind
        && closeEnough(lhs.feedbackProgress, rhs.feedbackProgress)
        && closeEnough(lhs.feedbackOrigin.x(), rhs.feedbackOrigin.x())
        && closeEnough(lhs.feedbackOrigin.y(), rhs.feedbackOrigin.y());
}

bool parseTarget(
    const QJsonObject& object,
    CaptureTarget* target,
    QString* referenceRoot,
    QString* errorMessage)
{
    if (!target || !referenceRoot) {
        return fail(errorMessage, QStringLiteral("target output is null"));
    }
    const QJsonObject font = object.value(QStringLiteral("font")).toObject();
    if (font.isEmpty()
        || !requireNumber(font, QStringLiteral("pointSize"), 14.0, errorMessage)
        || !requireBool(font, QStringLiteral("exactMatchRequired"), true, errorMessage)
        || !requireBool(font, QStringLiteral("faceFingerprintRequired"), true, errorMessage)
        || !requireBool(font, QStringLiteral("substitutionsAllowed"), false, errorMessage)
        || !requireString(
            font,
            QStringLiteral("hintingPreference"),
            QStringLiteral("prefer-no-hinting"),
            errorMessage)) {
        return false;
    }

    target->id = object.value(QStringLiteral("id")).toString();
    target->operatingSystem =
        object.value(QStringLiteral("operatingSystem")).toString();
    target->runnerImage =
        object.value(QStringLiteral("runnerImage")).toString();
    target->architecture =
        object.value(QStringLiteral("architecture")).toString();
    target->qtVersion = object.value(QStringLiteral("qtVersion")).toString();
    target->qtMajor = object.value(QStringLiteral("qtMajor")).toInt();
    target->compiler = object.value(QStringLiteral("compiler")).toString();
    target->fontFamily = font.value(QStringLiteral("family")).toString();
    target->fontPointSize = font.value(QStringLiteral("pointSize")).toInt();
    *referenceRoot =
        object.value(QStringLiteral("referenceRoot")).toString();
    if (target->id.isEmpty() || target->operatingSystem.isEmpty()
        || target->runnerImage.isEmpty() || target->architecture.isEmpty()
        || target->qtVersion.isEmpty() || target->compiler.isEmpty()
        || target->fontFamily.isEmpty() || target->qtMajor == 0
        || !isSafeRelativePath(*referenceRoot)) {
        return fail(errorMessage, QStringLiteral("invalid capture target"));
    }
    return true;
}

bool validateRuntimeFieldInventory(
    const QJsonObject& environment,
    QString* errorMessage)
{
    const QJsonObject fingerprint =
        environment.value(QStringLiteral("runtimeFingerprint")).toObject();
    const QJsonArray fields = fingerprint.value(QStringLiteral("fields")).toArray();
    if (!requireBool(
            fingerprint,
            QStringLiteral("required"),
            true,
            errorMessage)
        || !requireString(
            fingerprint,
            QStringLiteral("algorithm"),
            QStringLiteral("sha256"),
            errorMessage)
        || fields.size() != kRuntimeFingerprintFields.size()) {
        return fail(
            errorMessage,
            QStringLiteral("runtime fingerprint contract is incomplete"));
    }
    for (int index = 0; index < fields.size(); ++index) {
        if (fields.at(index).toString() != kRuntimeFingerprintFields.at(index)) {
            return fail(
                errorMessage,
                QStringLiteral("runtime fingerprint field order drift"));
        }
    }
    return true;
}

bool loadContract(
    const QString& path,
    const QString& selectedTargetId,
    CaptureContract* contract,
    QString* errorMessage)
{
    if (!contract) {
        return fail(errorMessage, QStringLiteral("capture contract output is null"));
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail(
            errorMessage,
            QStringLiteral("cannot open capture contract %1: %2")
                .arg(path, file.errorString()));
    }
    contract->bytes = file.readAll();
    QJsonParseError parseError;
    const QJsonDocument document =
        QJsonDocument::fromJson(contract->bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return fail(
            errorMessage,
            QStringLiteral("invalid capture contract JSON: %1")
                .arg(parseError.errorString()));
    }
    const QJsonObject root = document.object();
    const QString contractStatus =
        root.value(QStringLiteral("status")).toString();
    if (root.value(QStringLiteral("schemaVersion")).toInt() != 1) {
        return fail(errorMessage, QStringLiteral("unsupported contract schema"));
    }
    if (!requireString(
            root,
            QStringLiteral("id"),
            QStringLiteral("classic.buttons.visual"),
            errorMessage)) {
        return false;
    }
    if (contractStatus != QLatin1String("contract-only")
        && contractStatus != QLatin1String("single-target-active")
        && contractStatus != QLatin1String("matrix-active")) {
        return fail(
            errorMessage,
            QStringLiteral("unsupported visual contract status"));
    }
    contract->id = root.value(QStringLiteral("id")).toString();

    const QJsonObject fixture = root.value(QStringLiteral("fixture")).toObject();
    if (!requireString(fixture, QStringLiteral("size"), QStringLiteral("small"), errorMessage)
        || !requireString(fixture, QStringLiteral("density"), QStringLiteral("default"), errorMessage)
        || !requireString(fixture, QStringLiteral("content"), QStringLiteral("label-only"), errorMessage)) {
        return false;
    }
    contract->label = fixture.value(QStringLiteral("label")).toString();
    if (contract->label.isEmpty()) {
        return fail(errorMessage, QStringLiteral("capture fixture label is empty"));
    }

    const QJsonObject runner =
        root.value(QStringLiteral("captureRunner")).toObject();
    if (!requireString(runner, QStringLiteral("status"), QStringLiteral("source-implemented"), errorMessage)
        || !requireString(runner, QStringLiteral("executable"), QStringLiteral("qtmaterial3_material_button_capture"), errorMessage)
        || !requireString(runner, QStringLiteral("mode"), QStringLiteral("actual-only"), errorMessage)
        || !requireString(runner, QStringLiteral("manifestPath"), QStringLiteral("manifest.json"), errorMessage)
        || !requireString(runner, QStringLiteral("actualPathTemplate"), QStringLiteral("actual/{componentId}/{state}/{theme}.png"), errorMessage)
        || !requireBool(runner, QStringLiteral("requiresEmptyOutputDirectory"), true, errorMessage)
        || !requireBool(runner, QStringLiteral("referenceWritesAllowed"), false, errorMessage)
        || !requireString(runner, QStringLiteral("runtimeValidation"), QStringLiteral("fail-closed"), errorMessage)) {
        return false;
    }
    contract->runnerMode = runner.value(QStringLiteral("mode")).toString();
    contract->manifestPath =
        runner.value(QStringLiteral("manifestPath")).toString();

    const QJsonObject environment =
        root.value(QStringLiteral("environment")).toObject();
    const QJsonObject animation =
        environment.value(QStringLiteral("animation")).toObject();
    if (!requireString(environment, QStringLiteral("seedColor"), QStringLiteral("#6750A4"), errorMessage)
        || !requireString(environment, QStringLiteral("contrastMode"), QStringLiteral("standard"), errorMessage)
        || !requireString(environment, QStringLiteral("colorBackend"), QStringLiteral("force-fallback"), errorMessage)
        || !requireString(environment, QStringLiteral("applicationStyle"), QStringLiteral("Fusion"), errorMessage)
        || !requireString(environment, QStringLiteral("locale"), QStringLiteral("C"), errorMessage)
        || !requireString(environment, QStringLiteral("timeZone"), QStringLiteral("UTC"), errorMessage)
        || !requireString(environment, QStringLiteral("layoutDirection"), QStringLiteral("left-to-right"), errorMessage)
        || !requireString(environment, QStringLiteral("qpaPlatform"), QStringLiteral("offscreen"), errorMessage)
        || !requireNumber(environment, QStringLiteral("devicePixelRatio"), 1.0, errorMessage)
        || !requireNumber(environment, QStringLiteral("logicalDpiX"), 96.0, errorMessage)
        || !requireNumber(environment, QStringLiteral("logicalDpiY"), 96.0, errorMessage)
        || !requireString(animation, QStringLiteral("driver"), QStringLiteral("direct-test-controller"), errorMessage)
        || !requireBool(animation, QStringLiteral("wallClockAllowed"), false, errorMessage)
        || !requireBool(animation, QStringLiteral("eventLoopWaitAllowed"), false, errorMessage)
        || !validateRuntimeFieldInventory(environment, errorMessage)) {
        return false;
    }
    contract->devicePixelRatio =
        environment.value(QStringLiteral("devicePixelRatio")).toDouble();
    contract->logicalDpiX =
        environment.value(QStringLiteral("logicalDpiX")).toDouble();
    contract->logicalDpiY =
        environment.value(QStringLiteral("logicalDpiY")).toDouble();

    const QJsonObject normalization =
        root.value(QStringLiteral("imageNormalization")).toObject();
    const QJsonObject canvas =
        normalization.value(QStringLiteral("logicalCanvas")).toObject();
    const QJsonObject widget =
        normalization.value(QStringLiteral("widgetRect")).toObject();
    if (!requireString(normalization, QStringLiteral("backgroundColorRole"), QStringLiteral("Surface"), errorMessage)
        || !requireString(normalization, QStringLiteral("encoder"), QStringLiteral("png"), errorMessage)
        || !requireString(normalization, QStringLiteral("pixelFormat"), QStringLiteral("RGBA8888"), errorMessage)
        || !requireString(normalization, QStringLiteral("alphaMode"), QStringLiteral("straight"), errorMessage)
        || !requireString(normalization, QStringLiteral("colorSpace"), QStringLiteral("sRGB"), errorMessage)
        || !requireString(normalization, QStringLiteral("origin"), QStringLiteral("top-left"), errorMessage)
        || !requireString(normalization, QStringLiteral("metadataPolicy"), QStringLiteral("strip-all"), errorMessage)) {
        return false;
    }
    contract->canvasSize = QSize(
        canvas.value(QStringLiteral("width")).toInt(),
        canvas.value(QStringLiteral("height")).toInt());
    contract->widgetRect = QRect(
        widget.value(QStringLiteral("x")).toInt(),
        widget.value(QStringLiteral("y")).toInt(),
        widget.value(QStringLiteral("width")).toInt(),
        widget.value(QStringLiteral("height")).toInt());
    if (contract->canvasSize != QSize(160, 80)
        || contract->widgetRect != QRect(24, 20, 112, 40)) {
        return fail(errorMessage, QStringLiteral("capture geometry contract drift"));
    }

    bool foundTarget = false;
    const QJsonArray targets = root.value(QStringLiteral("targets")).toArray();
    for (const QJsonValue& value : targets) {
        if (!value.isObject()) {
            return fail(errorMessage, QStringLiteral("capture target is not an object"));
        }
        CaptureTarget parsed;
        QString referenceRoot;
        if (!parseTarget(value.toObject(), &parsed, &referenceRoot, errorMessage)) {
            return false;
        }
        contract->referenceRoots.append(referenceRoot);
        if (parsed.id == selectedTargetId) {
            if (foundTarget) {
                return fail(errorMessage, QStringLiteral("duplicate selected target"));
            }
            contract->target = parsed;
            foundTarget = true;
        }
    }
    if (!foundTarget) {
        return fail(
            errorMessage,
            QStringLiteral("target %1 is absent from the contract")
                .arg(selectedTargetId));
    }

    const QJsonArray setups =
        root.value(QStringLiteral("stateSetups")).toArray();
    if (setups.size() != kStates.size()) {
        return fail(errorMessage, QStringLiteral("state setup inventory is incomplete"));
    }
    for (int index = 0; index < setups.size(); ++index) {
        ButtonCaptureState parsed;
        QString id;
        if (!setups.at(index).isObject()
            || !parseStateSetup(
                setups.at(index).toObject(),
                &parsed,
                &id,
                errorMessage)) {
            return false;
        }
        if (id != kStates.at(index)
            || !sameStateSetup(parsed, expectedStateSetup(id))
            || contract->stateSetups.contains(id)) {
            return fail(errorMessage, QStringLiteral("state setup contract drift"));
        }
        contract->stateSetups.insert(id, parsed);
    }

    const QJsonArray cases = root.value(QStringLiteral("cases")).toArray();
    if (root.value(QStringLiteral("expectedActualCount")).toInt()
            != kExpectedCaseCount
        || cases.size() != kExpectedCaseCount) {
        return fail(errorMessage, QStringLiteral("actual capture case count drift"));
    }
    QSet<QString> ids;
    QSet<QString> actualPaths;
    int caseIndex = 0;
    for (const QString& componentId : kComponents) {
        for (const QString& state : kStates) {
            for (const QString& theme : kThemes) {
                const QJsonObject object = cases.at(caseIndex).toObject();
                CaptureCase parsed;
                parsed.id = object.value(QStringLiteral("id")).toString();
                parsed.componentId =
                    object.value(QStringLiteral("componentId")).toString();
                parsed.state = object.value(QStringLiteral("state")).toString();
                parsed.theme = object.value(QStringLiteral("theme")).toString();
                parsed.actualPath =
                    object.value(QStringLiteral("actualPath")).toString();
                const QString expectedId =
                    QStringLiteral("classic.buttons.visual.%1.%2.%3")
                        .arg(componentId, state, theme);
                const QString expectedPath =
                    canonicalActualPath(componentId, state, theme);
                if (object.isEmpty() || parsed.id != expectedId
                    || parsed.componentId != componentId
                    || parsed.state != state || parsed.theme != theme
                    || object.value(QStringLiteral("stateSetupId")).toString()
                        != state
                    || parsed.actualPath != expectedPath
                    || !isSafeRelativePath(parsed.actualPath)
                    || ids.contains(parsed.id)
                    || actualPaths.contains(parsed.actualPath)) {
                    return fail(errorMessage, QStringLiteral("capture case matrix drift"));
                }
                ids.insert(parsed.id);
                actualPaths.insert(parsed.actualPath);
                contract->cases.push_back(parsed);
                ++caseIndex;
            }
        }
    }
    return true;
}

QString compiledOperatingSystem()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("windows");
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    return QStringLiteral("macos");
#elif defined(Q_OS_LINUX)
    return QStringLiteral("linux");
#else
    return QStringLiteral("unsupported");
#endif
}

QString compiledCompiler()
{
#if defined(_MSC_VER)
    const int major = _MSC_VER / 100;
    const int minor = _MSC_VER % 100;
    QString toolset;
    if (_MSC_VER >= 1930 && _MSC_VER < 1940) {
        toolset = QStringLiteral("v143");
    } else if (_MSC_VER >= 1920 && _MSC_VER < 1930) {
        toolset = QStringLiteral("v142");
    } else if (_MSC_VER >= 1910 && _MSC_VER < 1920) {
        toolset = QStringLiteral("v141");
    } else {
        toolset = QStringLiteral("unknown");
    }
    return QStringLiteral("msvc-%1.%2-%3")
        .arg(major)
        .arg(minor, 2, 10, QLatin1Char('0'))
        .arg(toolset);
#elif defined(__apple_build_version__) && defined(__clang_major__)
    return QStringLiteral("apple-clang-%1").arg(__clang_major__);
#elif defined(__clang_major__)
    return QStringLiteral("clang-%1").arg(__clang_major__);
#elif defined(__GNUC__)
    return QStringLiteral("gcc-%1").arg(__GNUC__);
#else
    return QStringLiteral("unknown");
#endif
}

QByteArray fontFaceSha256(const QRawFont& rawFont, bool* hasTable)
{
    static const QList<QByteArray> tableTags = {
        QByteArrayLiteral("head"),
        QByteArrayLiteral("hhea"),
        QByteArrayLiteral("maxp"),
        QByteArrayLiteral("OS/2"),
        QByteArrayLiteral("name"),
        QByteArrayLiteral("cmap"),
        QByteArrayLiteral("post"),
        QByteArrayLiteral("glyf"),
        QByteArrayLiteral("loca"),
        QByteArrayLiteral("CFF "),
        QByteArrayLiteral("CFF2"),
    };
    QCryptographicHash hash(QCryptographicHash::Sha256);
    bool found = false;
    for (const QByteArray& tag : tableTags) {
        const QByteArray table = rawFont.fontTable(tag.constData());
        if (table.isEmpty()) {
            continue;
        }
        found = true;
        hash.addData(tag);
        hash.addData("\0", 1);
        hash.addData(QByteArray::number(table.size()));
        hash.addData("\0", 1);
        hash.addData(table);
    }
    if (hasTable) {
        *hasTable = found;
    }
    return found ? hash.result().toHex() : QByteArray();
}

bool resolveFontIdentity(
    const QFont& font,
    const CaptureTarget& target,
    FontIdentity* identity,
    QString* errorMessage)
{
    if (!identity) {
        return fail(errorMessage, QStringLiteral("font identity output is null"));
    }
    const QFontInfo info(font);
    const QRawFont rawFont = QRawFont::fromFont(font);
    bool hasTable = false;
    const QByteArray faceSha256 = fontFaceSha256(rawFont, &hasTable);
    const QString style = !info.styleName().isEmpty()
        ? info.styleName()
        : rawFont.styleName();
    if (font.hintingPreference() != QFont::PreferNoHinting
        || !info.exactMatch() || info.family() != target.fontFamily
        || !closeEnough(info.pointSizeF(), target.fontPointSize)
        || !rawFont.isValid() || !hasTable || faceSha256.size() != 64
        || style.isEmpty()) {
        return fail(
            errorMessage,
            QStringLiteral("font resolution does not match target %1")
                .arg(target.id));
    }
    identity->family = info.family();
    identity->faceSha256 = faceSha256;
    identity->style = style;
    identity->weight = static_cast<int>(info.weight());
    identity->pointSize = info.pointSizeF();
    return true;
}

bool validateRuntimeEnvironment(
    const CaptureContract& contract,
    QJsonObject* values,
    QString* errorMessage)
{
    if (!values) {
        return fail(errorMessage, QStringLiteral("runtime fingerprint output is null"));
    }
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen || !QApplication::style()) {
        return fail(errorMessage, QStringLiteral("Qt raster environment is unavailable"));
    }
    const QString runnerImage =
        QString::fromUtf8(qgetenv("QTMATERIAL3_VISUAL_RUNNER_IMAGE"));
    const QString operatingSystem = compiledOperatingSystem();
    const QString operatingSystemVersion = QSysInfo::prettyProductName();
    const QString architecture = QSysInfo::currentCpuArchitecture();
    const QString qtVersion = QString::fromLatin1(qVersion());
    const QString qtBuildAbi = QSysInfo::buildAbi();
    const QString compiler = compiledCompiler();
    const QString qpaPlatform = QGuiApplication::platformName();
    const QString style = QApplication::style()->objectName();
    const qreal devicePixelRatio = screen->devicePixelRatio();
    const qreal logicalDpiX = screen->logicalDotsPerInchX();
    const qreal logicalDpiY = screen->logicalDotsPerInchY();

    if (runnerImage.isEmpty() || runnerImage != contract.target.runnerImage
        || operatingSystem != contract.target.operatingSystem
        || operatingSystemVersion.isEmpty()
        || architecture != contract.target.architecture
        || qtVersion != contract.target.qtVersion
        || QT_VERSION_MAJOR != contract.target.qtMajor
        || qtBuildAbi.isEmpty()
        || compiler != contract.target.compiler
        || qpaPlatform != QLatin1String("offscreen")
        || style.compare(QLatin1String("Fusion"), Qt::CaseInsensitive) != 0
        || QLocale().name() != QLatin1String("C")
        || qgetenv("TZ") != QByteArrayLiteral("UTC")
        || QGuiApplication::layoutDirection() != Qt::LeftToRight
        || !closeEnough(devicePixelRatio, contract.devicePixelRatio)
        || !closeEnough(logicalDpiX, contract.logicalDpiX)
        || !closeEnough(logicalDpiY, contract.logicalDpiY)) {
        return fail(
            errorMessage,
            QStringLiteral("runtime environment does not match target %1")
                .arg(contract.target.id));
    }

    values->insert(QStringLiteral("runner-image"), runnerImage);
    values->insert(QStringLiteral("operating-system"), operatingSystem);
    values->insert(
        QStringLiteral("operating-system-version"),
        operatingSystemVersion);
    values->insert(QStringLiteral("architecture"), architecture);
    values->insert(QStringLiteral("qt-version"), qtVersion);
    values->insert(QStringLiteral("qt-build-abi"), qtBuildAbi);
    values->insert(QStringLiteral("compiler"), compiler);
    values->insert(QStringLiteral("qpa-platform"), qpaPlatform);
    values->insert(QStringLiteral("application-style"), QStringLiteral("Fusion"));
    values->insert(QStringLiteral("device-pixel-ratio"), devicePixelRatio);
    values->insert(QStringLiteral("logical-dpi-x"), logicalDpiX);
    values->insert(QStringLiteral("logical-dpi-y"), logicalDpiY);
    return true;
}

Theme buildTheme(
    const QString& themeName,
    const CaptureTarget& target)
{
    ThemeOptions options;
    options.sourceColor = QColor(QStringLiteral("#6750A4"));
    options.mode = themeName == QLatin1String("dark")
        ? ThemeMode::Dark
        : ThemeMode::Light;
    options.contrast = ContrastMode::Standard;
    options.backendPolicy = ColorBackendPolicy::ForceFallback;

    Theme theme = ThemeBuilder().build(options);
    TypographyStyle label = theme.typography().style(TypeRole::LabelLarge);
    label.font.setFamily(target.fontFamily);
    label.font.setPointSize(target.fontPointSize);
    label.font.setHintingPreference(QFont::PreferNoHinting);
    theme.typography().setStyle(TypeRole::LabelLarge, label);
    return theme;
}

QByteArray pixelSha256(const QImage& image)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    const int rowBytes = image.width() * 4;
    for (int row = 0; row < image.height(); ++row) {
        hash.addData(
            reinterpret_cast<const char*>(image.constScanLine(row)),
            rowBytes);
    }
    return hash.result().toHex();
}

QImage normalizedImage(const QImage& source)
{
    QImage normalized = source.convertToFormat(QImage::Format_RGBA8888);
    normalized.setDevicePixelRatio(1.0);
    normalized.setColorSpace(QColorSpace(QColorSpace::SRgb));
    return normalized;
}

QString absoluteNormalizedPath(const QString& path)
{
    QString probe = QDir::cleanPath(
        QDir::fromNativeSeparators(QFileInfo(path).absoluteFilePath()));
    QStringList missingSegments;
    while (!QFileInfo::exists(probe)) {
        const QFileInfo probeInfo(probe);
        const QString segment = probeInfo.fileName();
        const QString parent = QDir::cleanPath(
            QDir::fromNativeSeparators(probeInfo.absolutePath()));
        if (segment.isEmpty() || parent == probe) {
            break;
        }
        missingSegments.prepend(segment);
        probe = parent;
    }
    const QString canonicalAncestor = QFileInfo(probe).canonicalFilePath();
    QString resolved = canonicalAncestor.isEmpty()
        ? probe
        : QDir::fromNativeSeparators(canonicalAncestor);
    for (const QString& segment : missingSegments) {
        resolved = QDir(resolved).filePath(segment);
    }
    return QDir::cleanPath(QDir::fromNativeSeparators(resolved));
}

Qt::CaseSensitivity fileSystemCaseSensitivity()
{
#if defined(Q_OS_WIN)
    return Qt::CaseInsensitive;
#else
    return Qt::CaseSensitive;
#endif
}

bool isSameOrChildPath(const QString& candidate, const QString& parent)
{
    const Qt::CaseSensitivity sensitivity = fileSystemCaseSensitivity();
    const QString normalizedCandidate =
        QDir::cleanPath(QDir::fromNativeSeparators(candidate));
    const QString normalizedParent =
        QDir::cleanPath(QDir::fromNativeSeparators(parent));
    if (normalizedCandidate.compare(normalizedParent, sensitivity) == 0) {
        return true;
    }
    QString prefix = normalizedParent;
    if (!prefix.endsWith(QLatin1Char('/'))) {
        prefix.append(QLatin1Char('/'));
    }
    return normalizedCandidate.startsWith(prefix, sensitivity);
}

QString findRepositoryRoot(const QString& contractPath)
{
    QDir directory(QFileInfo(contractPath).absolutePath());
    while (true) {
        if (QFileInfo::exists(directory.filePath(
                QStringLiteral("references/material3/reference-catalog.json")))) {
            return absoluteNormalizedPath(directory.absolutePath());
        }
        const QString before = directory.absolutePath();
        if (!directory.cdUp() || directory.absolutePath() == before) {
            return {};
        }
    }
}

bool validateOutputDirectory(
    const QString& outputPath,
    const QString& contractPath,
    const CaptureContract& contract,
    QString* normalizedOutput,
    QString* errorMessage)
{
    if (!normalizedOutput || outputPath.isEmpty()) {
        return fail(errorMessage, QStringLiteral("output directory is required"));
    }
    const QFileInfo outputInfo(outputPath);
    if (outputInfo.exists()
        && (!outputInfo.isDir() || outputInfo.isSymLink())) {
        return fail(
            errorMessage,
            QStringLiteral("output must be a real directory, not a file or symlink"));
    }
    const QDir outputDirectory(outputInfo.absoluteFilePath());
    if (outputInfo.exists()
        && !outputDirectory.entryList(
                QDir::AllEntries | QDir::NoDotAndDotDot,
                QDir::Name)
                .isEmpty()) {
        return fail(
            errorMessage,
            QStringLiteral("requiresEmptyOutputDirectory rejected non-empty output"));
    }

    const QString repositoryRoot = findRepositoryRoot(contractPath);
    if (repositoryRoot.isEmpty()) {
        return fail(errorMessage, QStringLiteral("cannot locate contract repository root"));
    }
    const QString candidate = absoluteNormalizedPath(outputPath);
    for (const QString& relativeRoot : contract.referenceRoots) {
        const QString referenceRoot = absoluteNormalizedPath(
            QDir(repositoryRoot).filePath(relativeRoot));
        if (isSameOrChildPath(candidate, referenceRoot)) {
            return fail(
                errorMessage,
                QStringLiteral("referenceWritesAllowed=false rejected output path"));
        }
    }
    *normalizedOutput = candidate;
    return true;
}

bool saveNormalizedPng(
    const QString& path,
    const QImage& image,
    QString* errorMessage)
{
    if (QFileInfo::exists(path)) {
        return fail(errorMessage, QStringLiteral("actual image already exists"));
    }
    if (image.format() != QImage::Format_RGBA8888
        || !closeEnough(image.devicePixelRatio(), 1.0)
        || image.colorSpace() != QColorSpace(QColorSpace::SRgb)
        || !image.textKeys().isEmpty()) {
        return fail(errorMessage, QStringLiteral("image normalization drift"));
    }
    if (!QDir().mkpath(QFileInfo(path).absolutePath())) {
        return fail(errorMessage, QStringLiteral("cannot create actual image directory"));
    }
    QSaveFile output(path);
    if (!output.open(QIODevice::WriteOnly)) {
        return fail(
            errorMessage,
            QStringLiteral("cannot open actual image %1: %2")
                .arg(path, output.errorString()));
    }
    QImage encoded = image;
    encoded.setColorSpace(QColorSpace());
    encoded.setDotsPerMeterX(0);
    encoded.setDotsPerMeterY(0);
    {
        QImageWriter writer(&output, QByteArrayLiteral("png"));
        if (!writer.write(encoded)) {
            return fail(
                errorMessage,
                QStringLiteral("cannot encode actual image %1: %2")
                    .arg(path, writer.errorString()));
        }
    }
    if (!output.commit()) {
        return fail(
            errorMessage,
            QStringLiteral("cannot commit actual image %1: %2")
                .arg(path, output.errorString()));
    }
    return true;
}

bool saveManifest(
    const QString& path,
    const QJsonObject& manifest,
    QString* errorMessage)
{
    if (QFileInfo::exists(path)) {
        return fail(errorMessage, QStringLiteral("capture manifest already exists"));
    }
    QSaveFile output(path);
    if (!output.open(QIODevice::WriteOnly)) {
        return fail(
            errorMessage,
            QStringLiteral("cannot open capture manifest: %1")
                .arg(output.errorString()));
    }
    const QByteArray bytes =
        QJsonDocument(manifest).toJson(QJsonDocument::Indented);
    if (output.write(bytes) != bytes.size()) {
        return fail(
            errorMessage,
            QStringLiteral("cannot write complete capture manifest: %1")
                .arg(output.errorString()));
    }
    if (!output.commit()) {
        return fail(
            errorMessage,
            QStringLiteral("cannot commit capture manifest: %1")
                .arg(output.errorString()));
    }
    return true;
}

int runCapture(
    const QString& contractPath,
    const QString& targetId,
    const QString& outputPath)
{
    QString errorMessage;
    CaptureContract contract;
    if (!loadContract(
            contractPath,
            targetId,
            &contract,
            &errorMessage)) {
        qCritical().noquote() << errorMessage;
        return 2;
    }

    QString outputDirectory;
    if (!validateOutputDirectory(
            outputPath,
            contractPath,
            contract,
            &outputDirectory,
            &errorMessage)) {
        qCritical().noquote() << errorMessage;
        return 2;
    }

    QFont applicationFont(
        contract.target.fontFamily,
        contract.target.fontPointSize);
    applicationFont.setHintingPreference(QFont::PreferNoHinting);
    QApplication::setFont(applicationFont);

    QJsonObject runtimeValues;
    if (!validateRuntimeEnvironment(
            contract,
            &runtimeValues,
            &errorMessage)) {
        qCritical().noquote() << errorMessage;
        return 2;
    }

    QVector<CapturedCase> captured;
    captured.reserve(contract.cases.size());
    FontIdentity fingerprintFont;
    const Theme fontProbeTheme =
        buildTheme(QStringLiteral("light"), contract.target);
    if (!resolveFontIdentity(
            fontProbeTheme.typography().style(TypeRole::LabelLarge).font,
            contract.target,
            &fingerprintFont,
            &errorMessage)) {
        qCritical().noquote() << errorMessage;
        return 2;
    }
    bool hasFingerprintFont = true;
    ButtonCaptureController controller;
    for (const CaptureCase& contractCase : contract.cases) {
        const Theme theme = buildTheme(contractCase.theme, contract.target);
        ButtonCaptureResult capture;
        if (!controller.capture(
                contractCase.componentId,
                contract.stateSetups.value(contractCase.state),
                theme,
                contract.label,
                contract.canvasSize,
                contract.widgetRect,
                theme.colorScheme().color(ColorRole::Surface),
                &capture,
                &errorMessage)) {
            qCritical().noquote()
                << contractCase.id << ':' << errorMessage;
            return 2;
        }

        FontIdentity caseFont;
        if (!resolveFontIdentity(
                capture.resolvedLabelFont,
                contract.target,
                &caseFont,
                &errorMessage)) {
            qCritical().noquote()
                << contractCase.id << ':' << errorMessage;
            return 2;
        }
        if (hasFingerprintFont && !(caseFont == fingerprintFont)) {
            qCritical().noquote()
                << contractCase.id
                << ": resolved font face changed within the capture";
            return 2;
        }
        fingerprintFont = caseFont;
        hasFingerprintFont = true;

        CapturedCase item;
        item.contractCase = contractCase;
        item.image = normalizedImage(capture.image);
        if (item.image.size() != contract.canvasSize
            || item.image.format() != QImage::Format_RGBA8888) {
            qCritical().noquote()
                << contractCase.id << ": normalized image geometry drift";
            return 2;
        }
        item.pixelSha256 = pixelSha256(item.image);
        captured.push_back(item);
    }
    if (captured.size() != kExpectedCaseCount || !hasFingerprintFont) {
        qCritical() << "capture did not produce the complete case set";
        return 2;
    }

    runtimeValues.insert(
        QStringLiteral("font-family"), fingerprintFont.family);
    runtimeValues.insert(
        QStringLiteral("font-face-sha256"),
        QString::fromLatin1(fingerprintFont.faceSha256));
    runtimeValues.insert(QStringLiteral("font-style"), fingerprintFont.style);
    runtimeValues.insert(QStringLiteral("font-weight"), fingerprintFont.weight);
    runtimeValues.insert(
        QStringLiteral("font-point-size"), fingerprintFont.pointSize);
    if (runtimeValues.size() != kRuntimeFingerprintFields.size()) {
        qCritical() << "runtime fingerprint inventory drift";
        return 2;
    }
    for (const QString& field : kRuntimeFingerprintFields) {
        if (!runtimeValues.contains(field)) {
            qCritical() << "runtime fingerprint inventory drift";
            return 2;
        }
    }

    QString revalidatedOutput;
    if (!validateOutputDirectory(
            outputDirectory,
            contractPath,
            contract,
            &revalidatedOutput,
            &errorMessage)
        || revalidatedOutput != outputDirectory) {
        qCritical().noquote() << errorMessage;
        return 2;
    }
    if (!QDir().mkpath(outputDirectory)) {
        qCritical() << "cannot create output directory";
        return 2;
    }
    QJsonArray manifestCases;
    for (const CapturedCase& item : captured) {
        const QString actualFile =
            QDir(outputDirectory).filePath(item.contractCase.actualPath);
        if (!saveNormalizedPng(actualFile, item.image, &errorMessage)) {
            qCritical().noquote() << errorMessage;
            return 2;
        }
        QJsonObject entry;
        entry.insert(QStringLiteral("id"), item.contractCase.id);
        entry.insert(
            QStringLiteral("componentId"),
            item.contractCase.componentId);
        entry.insert(QStringLiteral("state"), item.contractCase.state);
        entry.insert(QStringLiteral("theme"), item.contractCase.theme);
        entry.insert(
            QStringLiteral("actualPath"),
            item.contractCase.actualPath);
        entry.insert(QStringLiteral("width"), item.image.width());
        entry.insert(QStringLiteral("height"), item.image.height());
        entry.insert(
            QStringLiteral("pixelSha256"),
            QString::fromLatin1(item.pixelSha256));
        manifestCases.append(entry);
    }

    const QByteArray runtimeBytes =
        QJsonDocument(runtimeValues).toJson(QJsonDocument::Compact);
    QJsonObject runtimeFingerprint;
    runtimeFingerprint.insert(QStringLiteral("algorithm"), QStringLiteral("sha256"));
    runtimeFingerprint.insert(
        QStringLiteral("digest"),
        QString::fromLatin1(
            QCryptographicHash::hash(runtimeBytes, QCryptographicHash::Sha256)
                .toHex()));
    runtimeFingerprint.insert(QStringLiteral("values"), runtimeValues);

    QJsonObject manifest;
    manifest.insert(QStringLiteral("schemaVersion"), 1);
    manifest.insert(QStringLiteral("contractId"), contract.id);
    manifest.insert(
        QStringLiteral("contractSha256"),
        QString::fromLatin1(
            QCryptographicHash::hash(
                contract.bytes,
                QCryptographicHash::Sha256)
                .toHex()));
    manifest.insert(QStringLiteral("targetId"), contract.target.id);
    manifest.insert(QStringLiteral("captureMode"), contract.runnerMode);
    manifest.insert(QStringLiteral("runtimeFingerprint"), runtimeFingerprint);
    manifest.insert(QStringLiteral("caseCount"), captured.size());
    manifest.insert(QStringLiteral("cases"), manifestCases);

    const QString manifestFile =
        QDir(outputDirectory).filePath(contract.manifestPath);
    if (!saveManifest(manifestFile, manifest, &errorMessage)) {
        qCritical().noquote() << errorMessage;
        return 2;
    }
    qInfo().noquote()
        << QStringLiteral("Captured %1 actual-only Button cases for %2 in %3")
               .arg(captured.size())
               .arg(contract.target.id, outputDirectory);
    return 0;
}

} // namespace

int main(int argc, char** argv)
{
    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));
    qputenv("QT_SCALE_FACTOR", QByteArrayLiteral("1"));
    qputenv("TZ", QByteArrayLiteral("UTC"));
    qputenv("LC_ALL", QByteArrayLiteral("C"));
    QCoreApplication::setAttribute(Qt::AA_Use96Dpi, true);

    QApplication application(argc, argv);
    QCoreApplication::setApplicationName(
        QStringLiteral("qtmaterial3_material_button_capture"));
    QLocale::setDefault(QLocale::c());
    application.setLayoutDirection(Qt::LeftToRight);
    if (!QApplication::setStyle(QStringLiteral("Fusion"))) {
        qCritical() << "Fusion style is unavailable";
        return 2;
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Deterministic actual-only Material Button capture"));
    parser.addHelpOption();
    const QCommandLineOption contractOption(
        QStringLiteral("contract"),
        QStringLiteral("Path to the generated visual contract."),
        QStringLiteral("path"),
        QString::fromUtf8(QTMATERIAL3_MATERIAL_BUTTON_VISUAL_CONTRACT));
    const QCommandLineOption targetOption(
        QStringLiteral("target"),
        QStringLiteral("Declared capture target id."),
        QStringLiteral("id"));
    const QCommandLineOption outputOption(
        QStringLiteral("output"),
        QStringLiteral("Fresh output directory for actual artifacts."),
        QStringLiteral("directory"));
    parser.addOption(contractOption);
    parser.addOption(targetOption);
    parser.addOption(outputOption);
    parser.process(application);

    if (!parser.isSet(targetOption) || !parser.isSet(outputOption)) {
        qCritical() << "both --target and --output are required";
        parser.showHelp(2);
    }

    return runCapture(
        parser.value(contractOption),
        parser.value(targetOption),
        parser.value(outputOption));
}
