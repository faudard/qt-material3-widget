#include "studiodocument.h"
#include <QtTest/QtTest>
#include <QFile>
#include <QTemporaryDir>
class StudioDocumentTest : public QObject {
    Q_OBJECT
private slots:
    void templatesRoundTrip() {
        for (const QString& name : {"dashboard", "settings", "form"}) {
            Studio::Document d = Studio::templateDocument(name);
            Studio::Document read; QString error;
            QVERIFY2(Studio::parse(Studio::serialize(d), &read, &error), qPrintable(error));
            QCOMPARE(Studio::serialize(read), Studio::serialize(d));
            QVERIFY(!read.items.isEmpty());
        }
    }
    void invalidInputIsFailClosed() {
        Studio::Document d = Studio::templateDocument("form");
        Studio::Document unchanged = Studio::templateDocument("dashboard");
        const QByteArray baseline = Studio::serialize(unchanged);
        QString error; QByteArray invalid = Studio::serialize(d);
        invalid.replace("\"schemaVersion\": 1", "\"schemaVersion\": 2");
        QVERIFY(!Studio::parse(invalid, &unchanged, &error));
        QCOMPARE(Studio::serialize(unchanged), baseline);
        invalid = Studio::serialize(d);
        invalid.replace("input.text.outlined", "fake.unknown");
        QVERIFY(!Studio::parse(invalid, &unchanged, &error));
        invalid = Studio::serialize(d);
        invalid.replace("\"text\": \"Email\"", "\"onClick\": \"bad\", \"text\": \"Email\"");
        QVERIFY(!Studio::parse(invalid, &unchanged, &error));
        d.items.append(d.items.front());
        QVERIFY(!Studio::parse(Studio::serialize(d), &unchanged, &error));
        d = Studio::templateDocument("form");
        d.items[0].rect.moveLeft(-1);
        QVERIFY(!Studio::parse(Studio::serialize(d), &unchanged, &error));
        d.items[0].rect.moveLeft(0);
        d.items[0].rect.setWidth(d.width + 1);
        QVERIFY(!Studio::parse(Studio::serialize(d), &unchanged, &error));
        QVERIFY(!Studio::parse(QByteArray(2 * 1024 * 1024 + 1, 'x'), &unchanged, &error));
    }
    void deterministicEscapedExport() {
        QTemporaryDir temp; QVERIFY(temp.isValid());
        Studio::Document d = Studio::templateDocument("form");
        d.items[0].text = QString::fromUtf8("A < B & \"quotes\" é");
        const QString input = temp.filePath("form.json");
        const QString ui = temp.filePath("form.ui"); QString error;
        QVERIFY2(Studio::save(input, d, &error), qPrintable(error));
        Studio::Document read;
        QVERIFY2(Studio::load(input, &read, &error), qPrintable(error));
        QCOMPARE(Studio::serialize(read), Studio::serialize(d));
        QVERIFY2(Studio::exportUi(read, ui, &error), qPrintable(error));
        QFile xml(ui); QVERIFY(xml.open(QIODevice::ReadOnly));
        const QByteArray first = xml.readAll();
        QVERIFY(first.contains("&lt;")); QVERIFY(first.contains("&amp;"));
        QVERIFY(first.contains("QtMaterialOutlinedTextField"));
        xml.close();
        QVERIFY(Studio::exportUi(read, ui, &error));
        QVERIFY(xml.open(QIODevice::ReadOnly));
        QCOMPARE(xml.readAll(), first);
        const QString dir = temp.filePath("generated");
        QVERIFY2(Studio::exportCpp(read, dir, &error), qPrintable(error));
        QFile main(dir + "/main.cpp"); QVERIFY(main.open(QIODevice::ReadOnly));
        QByteArray cpp = main.readAll();
        QVERIFY(cpp.contains("QtMaterialOutlinedTextField"));
        QVERIFY(cpp.contains(d.items[0].text.toUtf8().toHex()));
        QVERIFY(!cpp.contains("A < B &"));
        QFile cmake(dir + "/CMakeLists.txt"); QVERIFY(cmake.open(QIODevice::ReadOnly));
        QVERIFY(cmake.readAll().contains("QtMaterial3::Widgets"));
    }
};
QTEST_GUILESS_MAIN(StudioDocumentTest)
#include "tst_studiodocument.moc"
