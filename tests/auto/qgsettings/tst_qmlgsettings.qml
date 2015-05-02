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
        //compare(settings.integerMap, {"key1": 1, "key2": 2});
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

        /*
        settings.testIntegerMap = {"another-key1": 1, "another-key2": 2};
        compare(settings.testIntegerMap, {"another-key1": 1, "another-key2": 2});

        settings.testIntegerMap = {};
        compare(settings.testIntegerMap, {});
        */
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
