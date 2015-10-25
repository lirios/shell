if(ENABLE_ALSA)
    find_package(ALSA)

    if(NOT ALSA_FOUND)
        message(WARNING "Alsa was not found!")
    endif()
endif()
if(ENABLE_PULSEAUDIO)
    pkg_check_modules(PulseAudio libpulse)
    pkg_check_modules(PulseAudioGLib libpulse-mainloop-glib)

    if(NOT PulseAudio_FOUND)
        message(WARNING "PulseAudio was not found!")
    endif()
endif()

set(SOURCES
    mixer.cpp
    mixerplugin.cpp
)

if(ALSA_FOUND)
    list(APPEND SOURCES alsamixerbackend.cpp)
endif()

if(PulseAudio_FOUND)
    list(APPEND SOURCES pulseaudiomixerbackend.cpp)
endif()

add_library(mixerplugin SHARED ${SOURCES})
target_link_libraries(mixerplugin
                      Qt5::Qml
                      Qt5::Quick)

if(ALSA_FOUND)
    include_directories(${ALSA_INCLUDE_DIRS})
    add_definitions(-DHAVE_ALSA)
    target_link_libraries(mixerplugin ${ALSA_LIBRARIES})
endif()

if(PulseAudio_FOUND)
    include_directories(${PulseAudioGLib_INCLUDE_DIRS})
    add_definitions(-DHAVE_PULSEAUDIO)
    target_link_libraries(mixerplugin ${PulseAudio_LIBRARIES} ${PulseAudioGLib_LIBRARIES})
endif()

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/mixer)
install(TARGETS mixerplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/mixer)
