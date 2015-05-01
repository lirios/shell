/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtTest/QtTest>

#include "qgsettings.h"

using namespace Hawaii;

class TestQGSettings : public QObject
{
    Q_OBJECT
public:
    TestQGSettings(QObject *parent = 0)
        : QObject(parent)
        , settings(Q_NULLPTR)
    {
    }

private Q_SLOTS:
    void initTestCase()
    {
        qputenv("GSETTINGS_BACKEND", "memory");
        qputenv("GSETTINGS_SCHEMA_DIR", QByteArrayLiteral(TEST_DATADIR));
        qDebug() << "Using backend:" << qgetenv("GSETTINGS_BACKEND");
        qDebug() << "Using schema directory:" << qgetenv("GSETTINGS_SCHEMA_DIR");
        QCOMPARE(qgetenv("GSETTINGS_BACKEND"), QByteArrayLiteral("memory"));

        QVERIFY2(QGSettings::isSchemaInstalled("org.hawaii.gsettings.test"),
                 "Schema org.hawaii.gsettings.test must be installed");

        settings = new QGSettings("org.hawaii.gsettings.test",
                                  "/org/hawaii/gsettings/test/");
        QVERIFY2(settings->isValid(),
                 "Settings for org.hawaii.gsettings.test must be valid");
    }

    void cleanupTestCase()
    {
        if (settings)
            settings->deleteLater();
    }

    void gVariantToQVariant_data()
    {
        QTest::addColumn<QString>("name");

        QTest::newRow("boolean") << "test-boolean";
        QTest::newRow("integer") << "test-integer";
        QTest::newRow("double") << "test-double";
        QTest::newRow("string") << "test-string";
        QTest::newRow("choices") << "test-choices";
        QTest::newRow("string-list") << "test-string-list";
        QTest::newRow("string-map") << "test-string-map";
        //QTest::newRow("integer-map") << "test-integer-map";
    }

    void gVariantToQVariant()
    {
        QFETCH(QString, name);

        QVariant value = settings->defaultValue(name);
        QVERIFY(value.isValid());
    }

    void readBoolean()
    {
        QVariant value = settings->defaultValue("test-boolean");
        QCOMPARE(value.toBool(), true);
    }

    void readInteger()
    {
        QVariant value = settings->defaultValue("test-integer");
        QCOMPARE(value.toInt(), 42);
    }

    void readDouble()
    {
        QVariant value = settings->defaultValue("test-double");
        QCOMPARE(value.toDouble(), 42.69);
    }

    void readString()
    {
        QVariant value = settings->defaultValue("test-string");
        QCOMPARE(value.toString(), QStringLiteral("howdy?"));
    }

    void readChoices()
    {
        QVariant value = settings->defaultValue(QStringLiteral("test-choices"));
        QCOMPARE(value.toString(), QStringLiteral("one"));
    }

    void readStringList()
    {
        QStringList cmpValue = QStringList() << "one" << "two" << "three";
        QVariant value = settings->defaultValue("test-string-list");
        QCOMPARE(value.toStringList(), cmpValue);
    }

    void readStringMap()
    {
        QMap<QString, QVariant> cmpValue;
        cmpValue.insert("key1", "value1");
        cmpValue.insert("key2", "value2");
        QVariant value = settings->defaultValue("test-string-map");
        QCOMPARE(value.toMap(), cmpValue);
    }

    void readIntegerMap()
    {
        QSKIP("We are unable to handle integer-map now");

        QMap<QString, QVariant> cmpValue;
        cmpValue.insert("key1", 1);
        cmpValue.insert("key2", 2);
        cmpValue.insert("key3", 3);
        QVariant value = settings->defaultValue("test-integer-map");
        QCOMPARE(value.toMap(), cmpValue);
    }

    void setValue_data()
    {
        QTest::addColumn<QString>("name");
        QTest::addColumn<QVariant>("value");

        QTest::newRow("boolean") << "test-boolean" << QVariant(false);
        QTest::newRow("integer") << "test-integer" << QVariant(69);
        QTest::newRow("double") << "test-double" << QVariant(69.42);
        QTest::newRow("string") << "test-string" << QVariant("hello");
        QTest::newRow("choices") << "test-choices" << QVariant("three");
        QStringList stringList = QStringList() << "another" << "value";
        QTest::newRow("string-list") << "test-string-list" << QVariant(stringList);
        QMap<QString, QVariant> stringMap;
        stringMap.insert("another-key1", "one");
        stringMap.insert("another-key2", "two");
        QTest::newRow("string-map") << "test-string-map" << QVariant(stringMap);
        /*
        QMap<QString, QVariant> intMap;
        intMap.insert("another-key1", 1);
        intMap.insert("another-key2", 2);
        QTest::newRow("integer-map") << "test-integer-map" << QVariant(intMap);
        */
    }

    void setValue()
    {
        QFETCH(QString, name);
        QFETCH(QVariant, value);

        settings->setValue(name, value);
        QCOMPARE(settings->value(name), value);
    }

    void invalidSchema()
    {
        QSKIP("Crash for a gio bug");

        QVERIFY2(!QGSettings::isSchemaInstalled("org.hawaii.gsettings.invalid-test"),
                 "Schema org.hawaii.gsettings.invalid-test must not be installed");
        QGSettings *invalidSettings = new QGSettings("org.hawaii.gsettings.invalid-test");
        QVERIFY2(!invalidSettings->isValid(),
                 "Settings for org.hawaii.gsettings.invalid-test must not be valid");
        invalidSettings->deleteLater();
    }

    void invalidPath()
    {
        QSKIP("Crash for a gio bug");

        QGSettings *invalidSettings = new QGSettings("org.hawaii.gsettings.test",
                                                     "/org/hawaii/gsettings/invalid-test/");
        QVERIFY2(!invalidSettings->isValid(),
                 "Settings for org.hawaii.gsettings.test must not be valid");
        invalidSettings->deleteLater();
    }

private:
    QGSettings *settings;
};

QTEST_MAIN(TestQGSettings)

#include "tst_qgsettings.moc"
