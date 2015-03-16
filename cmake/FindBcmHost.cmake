#.rst:
# FindBcmHost
# -----------
#
# Try to find Broadcom libraries.
#
# It is meant as a drop-in replacement for FindEGL.cmake on Raspberry Pi
# systems.
#
# This will define the following variables:
#
# ``BcmHost_FOUND``
#     True if bcm_host is available
# ``BcmHost_LIBRARIES``
#     This can be passed to target_link_libraries() instead of the ``BcmHost::BcmHost``
#     target
# ``BcmHost_INCLUDE_DIRS``
#     This should be passed to target_include_directories() if the target is not
#     used for linking
#
# If ``BcmHost_FOUND`` is TRUE, it will also define the following imported target:
#
# ``BcmHost::BcmHost``
#     The BcmHost library
# ``BcmHost::GLESv2``
#     The GLESv2 library
# ``BcmHost::EGL``
#     The EGL library
#
# In general we recommend using the imported target, as it is easier to use.
# Bear in mind, however, that if the target is in the link interface of an
# exported library, it must be made available by the package config file.
#
# Since pre-1.0.0.

#=============================================================================
# Copyright 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

if(NOT BcmHost_PREFIX)
    set(BcmHost_PREFIX "/opt/vc")
endif()

find_path(BcmHost_INCLUDE_DIR
    NAMES
        bcm_host.h
    HINTS
        "${BcmHost_PREFIX}/include"
)

find_library(BcmHost_LIBRARY
    NAMES
        bcm_host
    HINTS
        "${BcmHost_PREFIX}/lib"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BcmHost
    FOUND_VAR
        BcmHost_FOUND
    REQUIRED_VARS
        BcmHost_LIBRARY
        BcmHost_INCLUDE_DIR
)

if(BcmHost_FOUND)
    set(BcmHost_LIBRARY_DIRS "${BcmHost_PREFIX}/lib")
    set(BcmHost_INCLUDE_DIR ${BcmHost_INCLUDE_DIR} ${BcmHost_INCLUDE_DIR}/interface ${BcmHost_INCLUDE_DIR}/interface/vcos/pthreads)
endif()

if(BcmHost_FOUND)
    if(NOT TARGET BcmHost::BcmHost)
        add_library(BcmHost::BcmHost UNKNOWN IMPORTED)
        set_target_properties(BcmHost::BcmHost PROPERTIES
            IMPORTED_LOCATION "-L${BcmHost_LIBRARY_DIRS} ${BcmHost_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${BcmHost_INCLUDE_DIR}"
        )
    endif()

    if(NOT TARGET BcmHost::GLESv2)
        add_library(BcmHost::GLESv2 UNKNOWN IMPORTED)
        set_target_properties(BcmHost::GLESv2 PROPERTIES
            IMPORTED_LOCATION "-L${BcmHost_LIBRARY_DIRS} -lGLESv2"
            INTERFACE_INCLUDE_DIRECTORIES "${BcmHost_INCLUDE_DIR}"
        )
    endif()

    if(NOT TARGET BcmHost::EGL)
        add_library(BcmHost::EGL UNKNOWN IMPORTED)
        set_target_properties(BcmHost::EGL PROPERTIES
            IMPORTED_LOCATION "-L${BcmHost_LIBRARY_DIRS} -lEGL"
            INTERFACE_INCLUDE_DIRECTORIES "${BcmHost_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(BcmHost_LIBRARY BcmHost_INCLUDE_DIR)

# Compatibility variables
set(BcmHost_LIBRARIES "-L${BcmHost_LIBRARY_DIRS} -lGLESv2 -lEGL -lm -lbcm_host")
set(BcmHost_INCLUDE_DIRS ${BcmHost_INCLUDE_DIR})

include(FeatureSummary)
set_package_properties(BcmHost PROPERTIES
    URL "http://www.broadcom.com/"
    DESCRIPTION "Broadcom OpenGLES graphics libraries."
)
