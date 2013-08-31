/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef SHORTCUT_P_H
#define SHORTCUT_P_H

#include <QtGui/QKeySequence>

#include <linux/input.h>

class KeyBinding;

enum WaylandModifiers {
    MODIFIER_CTRL = (1 << 0),
    MODIFIER_ALT = (1 << 1),
    MODIFIER_SUPER = (1 << 2),
    MODIFIER_SHIFT = (1 << 3)
};

static const quint32 keyTable[] = {
    Qt::Key_Escape, KEY_ESC,
    Qt::Key_1, KEY_1,
    Qt::Key_2, KEY_2,
    Qt::Key_3, KEY_3,
    Qt::Key_4, KEY_4,
    Qt::Key_5, KEY_5,
    Qt::Key_6, KEY_6,
    Qt::Key_7, KEY_7,
    Qt::Key_8, KEY_8,
    Qt::Key_9, KEY_9,
    Qt::Key_0, KEY_0,
    Qt::Key_Minus, KEY_MINUS,
    Qt::Key_Equal, KEY_EQUAL,
    Qt::Key_Backspace, KEY_BACKSPACE,
    Qt::Key_Tab, KEY_TAB,
    Qt::Key_Q, KEY_Q,
    Qt::Key_W, KEY_W,
    Qt::Key_E, KEY_E,
    Qt::Key_R, KEY_R,
    Qt::Key_T, KEY_T,
    Qt::Key_Y, KEY_T,
    Qt::Key_U, KEY_U,
    Qt::Key_I, KEY_I,
    Qt::Key_O, KEY_O,
    Qt::Key_P, KEY_P,
    Qt::Key_Enter, KEY_ENTER,
    Qt::Key_A, KEY_A,
    Qt::Key_S, KEY_S,
    Qt::Key_D, KEY_D,
    Qt::Key_F, KEY_F,
    Qt::Key_G, KEY_G,
    Qt::Key_H, KEY_H,
    Qt::Key_J, KEY_J,
    Qt::Key_K, KEY_K,
    Qt::Key_L, KEY_L,
    Qt::Key_Semicolon, KEY_SEMICOLON,
    Qt::Key_Apostrophe, KEY_APOSTROPHE,
    Qt::Key_Backslash, KEY_BACKSLASH,
    Qt::Key_Z, KEY_Z,
    Qt::Key_X, KEY_X,
    Qt::Key_C, KEY_C,
    Qt::Key_V, KEY_V,
    Qt::Key_B, KEY_B,
    Qt::Key_N, KEY_N,
    Qt::Key_M, KEY_M,
    Qt::Key_Comma, KEY_COMMA,
    Qt::Key_Slash, KEY_SLASH,
    Qt::Key_Space, KEY_SPACE,
    Qt::Key_CapsLock, KEY_CAPSLOCK,
    Qt::Key_F1, KEY_F1,
    Qt::Key_F2, KEY_F2,
    Qt::Key_F3, KEY_F3,
    Qt::Key_F4, KEY_F4,
    Qt::Key_F5, KEY_F5,
    Qt::Key_F6, KEY_F6,
    Qt::Key_F7, KEY_F7,
    Qt::Key_F8, KEY_F8,
    Qt::Key_F9, KEY_F9,
    Qt::Key_F10, KEY_F10,
    Qt::Key_NumLock, KEY_NUMLOCK,
    Qt::Key_7 | Qt::KeypadModifier, KEY_KP7,
    Qt::Key_8 | Qt::KeypadModifier, KEY_KP8,
    Qt::Key_9 | Qt::KeypadModifier, KEY_KP9,
    Qt::Key_Minus | Qt::KeypadModifier, KEY_KPMINUS,
    Qt::Key_4 | Qt::KeypadModifier, KEY_KP4,
    Qt::Key_5 | Qt::KeypadModifier, KEY_KP5,
    Qt::Key_6 | Qt::KeypadModifier, KEY_KP6,
    Qt::Key_Plus | Qt::KeypadModifier, KEY_KPPLUS,
    Qt::Key_1 | Qt::KeypadModifier, KEY_KP1,
    Qt::Key_2 | Qt::KeypadModifier, KEY_KP2,
    Qt::Key_3 | Qt::KeypadModifier, KEY_KP3,
    Qt::Key_0 | Qt::KeypadModifier, KEY_KP0,
    Qt::Key_Zenkaku_Hankaku, KEY_ZENKAKUHANKAKU,
    Qt::Key_F11, KEY_F11,
    Qt::Key_F12, KEY_F12,
    Qt::Key_Katakana, KEY_KATAKANA,
    Qt::Key_Hiragana, KEY_HIRAGANA,
    Qt::Key_Henkan, KEY_HENKAN,
    Qt::Key_Muhenkan, KEY_MUHENKAN,
    Qt::Key_Enter | Qt::KeypadModifier, KEY_KPENTER,
    Qt::Key_SysReq, KEY_SYSRQ,
    Qt::Key_Home, KEY_HOME,
    Qt::Key_Up, KEY_UP,
    Qt::Key_PageUp, KEY_PAGEUP,
    Qt::Key_Left, KEY_LEFT,
    Qt::Key_Right, KEY_RIGHT,
    Qt::Key_End, KEY_END,
    Qt::Key_Down, KEY_DOWN,
    Qt::Key_PageDown, KEY_PAGEDOWN,
    Qt::Key_Insert, KEY_INSERT,
    Qt::Key_Delete, KEY_DELETE,
    Qt::Key_VolumeMute, KEY_MUTE,
    Qt::Key_VolumeDown, KEY_VOLUMEDOWN,
    Qt::Key_VolumeUp, KEY_VOLUMEUP,
    Qt::Key_PowerDown, KEY_POWER,
    Qt::Key_PowerOff, KEY_POWER2,
    Qt::Key_Equal | Qt::KeypadModifier, KEY_KPEQUAL,
    Qt::Key_plusminus | Qt::KeypadModifier, KEY_KPPLUSMINUS,
    Qt::Key_Pause, KEY_PAUSE,
    Qt::Key_Comma | Qt::KeypadModifier, KEY_KPCOMMA,
    Qt::Key_Hangul, KEY_HANGEUL,
    Qt::Key_yen, KEY_YEN,
    Qt::Key_Meta, KEY_LEFTMETA,
    Qt::Key_Super_L, KEY_LEFTMETA,
    Qt::Key_Super_R, KEY_RIGHTMETA,
    Qt::Key_Stop, KEY_STOP,
    Qt::Key_Copy, KEY_COPY,
    Qt::Key_OpenUrl, KEY_OPEN,
    Qt::Key_Paste, KEY_PASTE,
    Qt::Key_Cut, KEY_CUT,
    Qt::Key_Help, KEY_HELP,
    Qt::Key_Menu, KEY_MENU,
    Qt::Key_Calculator, KEY_CALC,
    Qt::Key_Search, KEY_FIND,
    Qt::Key_Sleep, KEY_SLEEP,
    Qt::Key_WakeUp, KEY_WAKEUP,
    Qt::Key_Send, KEY_SENDFILE,
    Qt::Key_Xfer, KEY_XFER,
    Qt::Key_WWW, KEY_WWW,
    Qt::Key_DOS, KEY_MSDOS,
    Qt::Key_ScreenSaver, KEY_SCREENLOCK,
    Qt::Key_MailForward, KEY_MAIL,
    Qt::Key_Back, KEY_BACK,
    Qt::Key_Forward, KEY_FORWARD,
    Qt::Key_Eject, KEY_EJECTCD,
    Qt::Key_AudioForward, KEY_NEXTSONG,
    Qt::Key_AudioRewind, KEY_PREVIOUSSONG,
    Qt::Key_Play, KEY_PLAYPAUSE,
    Qt::Key_Phone, KEY_PHONE,
    Qt::Key_HomePage, KEY_HOMEPAGE,
    Qt::Key_Refresh, KEY_REFRESH,
    Qt::Key_F13, KEY_F13,
    Qt::Key_F14, KEY_F14,
    Qt::Key_F15, KEY_F15,
    Qt::Key_F16, KEY_F16,
    Qt::Key_F17, KEY_F17,
    Qt::Key_F18, KEY_F18,
    Qt::Key_F19, KEY_F19,
    Qt::Key_F20, KEY_F20,
    Qt::Key_F21, KEY_F21,
    Qt::Key_F22, KEY_F22,
    Qt::Key_F23, KEY_F23,
    Qt::Key_F24, KEY_F24,
    Qt::Key_KeyboardBrightnessUp, KEY_BRIGHTNESSUP,
    Qt::Key_KeyboardBrightnessDown, KEY_BRIGHTNESSDOWN
};

class ShortcutPrivate
{
public:
    ShortcutPrivate();
    ~ShortcutPrivate();

    void qtSequence2Wayland(const QKeySequence &s,
                            quint32 *keys, quint32 *mods);

    QKeySequence sequence;
    KeyBinding *binding;
};

#endif // SHORTCUT_P_H
