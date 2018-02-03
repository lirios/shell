import qbs 1.0

Product {
    name: "liri-shell-pam"
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
        fileTagsFilter: product.type
    }
}
