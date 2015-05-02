/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import QtTest 1.0
import org.hawaii.settings 0.2

// Some tests start with a number to make sure order
// of execution is respected, in fact we need to test
// default values before other tests change values

TestCase {
    name: "SettingsTest"

    property var changes: []

    Settings {
        id: settings
        schema.id: "org.hawaii.gsettings.test"
        schema.path: "/org/hawaii/gsettings/test/"
        onValueChanged: changes.push([key, value])
    }

    Settings {
        id: invalidSettings
        schema.id: "org.hawaii.gsettings.invalid.test"
    }

    function test_000_valid() {
        compare(settings.schema.valid, true, "Schema must be valid");
    }

    function test_001_readDefaults() {
        compare(settings.testBoolean, true);
        compare(settings.testInteger, 42);
        compare(settings.testDouble, 42.69);
        compare(settings.testString, "howdy?");
        compare(settings.testChoices, "one");
        compare(settings.testStringList, ["one", "two", "three"]);
        compare(settings.testStringMap, {"key1": "value1", "key2": "value2"});
        compare(settings.testIntegerMap, {"key1": 1, "key2": 2});
    }

    function test_setValue() {
        settings.testBoolean = false;
        compare(settings.testBoolean, false);

        settings.testInteger = 69;
        compare(settings.testInteger, 69);

        settings.testDouble = 69.42;
        compare(settings.testDouble, 69.42);

        settings.testString = "hello";
        compare(settings.testString, "hello");

        settings.testChoices = "three";
        compare(settings.testChoices, "three");

        settings.testStringList = ["another", "value"];
        compare(settings.testStringList, ["another", "value"]);

        settings.testStringList = [];
        compare(settings.testStringList, []);

        settings.testStringMap = {"another-key1": "one", "another-key2": "two"};
        compare(settings.testStringMap, {"another-key1": "one", "another-key2": "two"});

        settings.testStringMap = {};
        compare(settings.testStringMap, {});

        settings.testIntegerMap = {"another-key1": 1, "another-key2": 2};
        compare(settings.testIntegerMap, {"another-key1": 1, "another-key2": 2});

        settings.testIntegerMap = {};
        compare(settings.testIntegerMap, {});
    }

    function test_setOutOfRangeValue() {
        // Set a known value first
        settings.testChoices = "one";

        // Try to set an out of range value and verify
        // whether the first value is still reported
        settings.testChoices = "four";
        compare(settings.testChoices, "one");
    }

    function test_changes() {
        changes = [];

        settings.testInteger = 777;
        settings.testDouble = 77.7;
        settings.testString = "ciao";
        compare(changes, [["testInteger", 777], ["testDouble", 77.7], ["testString", "ciao"]]);
    }

    function test_invalidSchema() {
        compare(invalidSettings.schema.valid, false, "Invalid schema must report to be invalid");
        compare(invalidSettings.someKey, undefined, "Invalid schema doesn't have any keys");
    }
}
