import qbs 1.0
import qbs.Probes

QtGuiApplication {
    name: "liri-shell-helper"
    targetName: "liri-shell-helper"

    Depends { name: "lirideployment" }
    Depends { name: "GitRevision" }
    Depends {
        name: "Qt"
        submodules: ["core", "gui", "gui-private", "waylandclient"]
        versionAtLeast: project.minimumQtVersion
    }
    Depends { name: "Wayland.client" }
    Depends { name: "WaylandScanner" }

    LiriIncludeProbe {
        id: haveSysPrctl
        names: ["sys/prctl.h"]
    }

    cpp.defines: {
        var defines = base.concat(['LIRISHELL_VERSION="' + project.version + '"']);
        if (project.developmentBuild)
            defines.push("DEVELOPMENT_BUILD");
        if (haveSysPrctl.found)
            defines.push("HAVE_SYS_PRCTL_H");
        return defines;
    }
    cpp.includePaths: base.concat([
        product.sourceDirectory
    ])

    GitRevision.sourceDirectory: product.sourceDirectory + "/../.."

    files: [
        "main.cpp",
        "shellhelperapplication.cpp",
        "shellhelperapplication.h",
        "shellhelperclient.cpp",
        "shellhelperclient.h",
    ]

    Group {
        name: "Wayland Protocols"
        files: [
            "../../data/protocols/shell-helper.xml"
        ]
        fileTags: ["wayland.client.protocol"]
    }

    Group {
        name: "Git Revision"
        files: ["../../headers/gitsha1.h.in"]
        fileTags: ["liri.gitsha"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.libexecDir
        fileTagsFilter: product.type
    }
}
