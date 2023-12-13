// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "extsessionlocksurfacev1.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Aurora API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

class ExtSessionLockSurfaceV1Private
{
public:
    ExtSessionLockSurfaceV1Private() = default;

    bool initialized = false;
    QWindow *window = nullptr;
};
