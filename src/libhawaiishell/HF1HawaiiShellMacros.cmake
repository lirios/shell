#=============================================================================
# Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of Pier Luigi Fiorini nor the names of his
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

#
# hawaiishell_install_package(type name srcpath)
#
# Installs a Hawaii Shell package.
#
# \param type the package type, one of the following: element, shell,
#             lookandfeel
# \param name the package name, same as the X-Hawaii-PluginInfo-Name
#             key value from metadata.desktop
# \param srcpath the source path to install from, it's the
#                directory that contains metadata.desktop
#
macro(hawaiishell_install_package type name srcpath)
    if("${type}" STREQUAL "element")
        set(dstpath hawaii/elements)
    elseif("${type}" STREQUAL "shell")
        set(dstpath hawaii/shells)
    elseif("${type}" STREQUAL "lookandfeel")
        set(dstpath hawaii/lookandfeel)
    else()
        message(FATAL_ERROR "Package type \"${type}\" not recognized, possible values: \"element\", \"shell\", \"lookandfeeld\"")
    endif()

    install(DIRECTORY ${srcpath}/ DESTINATION ${CMAKE_INSTALL_DATADIR}/${dstpath}/${name}
        PATTERN CMakeLists.txt EXCLUDE)
endmacro()
