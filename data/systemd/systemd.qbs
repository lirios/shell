import qbs 1.0
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "liri-shell-systemd"
    type: "systemd"

    Depends { name: "qbsbuildconfig" }
    Depends { name: "lirideployment" }

    Group {
        name: "Templates"
        files: ["*.in"]
        fileTags: ["inFiles"]
    }

    Group {
        name: "Units"
        files: ["*.target"]
        fileTags: ["systemd"]
    }

    Rule {
        inputs: ["inFiles"]

        Artifact {
            filePath: input.fileName.replace(".in", "")
            fileTags: ["systemd"]
        }

        prepare: {
            var vars = {
                INSTALL_BINDIR: FileInfo.joinPaths(product.moduleProperty("qbs", "installRoot"),
                                                   product.moduleProperty("lirideployment", "binDir"))
            };

            var cmd = new JavaScriptCommand();
            cmd.description = "generating " + output.fileName;
            cmd.highlight = "filegen";
            cmd.vars = vars;
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);

                var inFile = new TextFile(output.filePath, TextFile.ReadWrite);
                var contents = inFile.readAll();
                for (var i in vars)
                    contents = contents.replace(new RegExp('@' + i + '@(?!\w)', 'g'), vars[i]);
                inFile.truncate();
                inFile.write(contents);
                inFile.close();
            };
            return [cmd];
        }
    }

    Group {
        qbs.install: true
        qbs.installDir: qbsbuildconfig.systemdUserUnitDir
        fileTagsFilter: product.type
    }
}
