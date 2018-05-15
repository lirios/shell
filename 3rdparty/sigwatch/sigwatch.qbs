import qbs 1.0

StaticLibrary {
    name: "sigwatch"
    targetName: "sigwatch"

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }

    files: [
        "sigwatch.cpp",
        "sigwatch.h",
        "sigwatch_p.h",
    ]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }

        cpp.includePaths: base.concat([product.sourceDirectory])
    }
}
