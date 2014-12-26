install(DIRECTORY desktop/
        DESTINATION ${DATA_INSTALL_DIR}/greenisland/org.hawaii.desktop
        PATTERN .svn EXCLUDE
        PATTERN CMakeLists.txt EXCLUDE
        PATTERN Messages.sh EXCLUDE
        PATTERN dummydata EXCLUDE)
