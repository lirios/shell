import qbs 1.0
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "liri-shell-scripts"
    type: "script"

    Depends { name: "lirideployment" }

    Group {
        name: "Scripts"
        files: ["liri-session"]
        qbs.install: true
        qbs.installDir: lirideployment.binDir
    }
}
