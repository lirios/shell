import qbs 1.0
import qbs.File
import qbs.FileInfo
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "Liri Shell (PAM)"
    type: "pam"

    Depends { name: "lirideployment" }

    Group {
        name: "PAM"
        files: ["*.pam"]
        fileTags: ["pam"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.etcDir + "/pam.d"
        qbs.installPrefix: ""
        fileTagsFilter: product.type
    }
}
