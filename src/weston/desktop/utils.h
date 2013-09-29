/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#ifndef UTILS_H
#define UTILS_H

#include "signal.h"

#include <weston/compositor.h>

template<typename T>
class Vector2D {
public:
    inline Vector2D(T a, T b) : x(a), y(b) {}
    T x;
    T y;
};

template<typename T>
class Rect2D {
public:
    inline Rect2D() : x(0), y(0), width(0), height(0) {}
    inline Rect2D(T a, T b, T w, T h) : x(a), y(b), width(w), height(h) {}
    T x;
    T y;
    T width;
    T height;
};

typedef Vector2D<int> IVector2D;
typedef Rect2D<int> IRect2D;

class WlListener {
public:
    WlListener() { m_listener.parent = this; signal = new Signal<>; }
#if 0
    ~WlListener() { signal->flush(); wl_list_remove(&m_listener.listener.link); }
#else
    ~WlListener() { signal->flush(); }
#endif

    void listen(struct wl_signal *signal) {
        m_listener.listener.notify = [](struct wl_listener *listener, void *data) {
            WlListener *_this = static_cast<Wrapper *>(container_of(listener, Wrapper, listener))->parent;
            (*_this->signal)();
        };
        wl_signal_add(signal, &m_listener.listener);
    }

    Signal<> *signal;
private:
    struct Wrapper {
        struct wl_listener listener;
        WlListener *parent;
    };
    Wrapper m_listener;
};

template<class T, class... Args>
struct Wrapper {
    template<void (T::*F)(wl_client *, wl_resource *, Args...)>
    static void forward(wl_client *client, wl_resource *resource, Args... args) {
        (static_cast<T *>(wl_resource_get_user_data(resource))->*F)(client,resource, args...);
    }
    template<void (T::*F)(Args...)>
    static void forward(wl_client *client, wl_resource *resource, Args... args) {
        (static_cast<T *>(wl_resource_get_user_data(resource))->*F)(args...);
    }
};

template<class T, class... Args>
constexpr static auto createWrapper(void (T::*func)(wl_client *client, wl_resource *resource, Args...)) -> Wrapper<T, Args...> {
    return Wrapper<T, Args...>();
}

template<class T, class... Args>
constexpr static auto createWrapper(void (T::*func)(Args...)) -> Wrapper<T, Args...> {
    return Wrapper<T, Args...>();
}

#define wrapInterface(method) createWrapper(method).forward<method>

#endif
