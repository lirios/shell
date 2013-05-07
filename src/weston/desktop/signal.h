/*
 * Copyright 2013  Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <list>

template<class... Args>
class Functor;

template<class... Args>
class Signal {
public:
    Signal() : m_startAgain(false), m_flush(false) { }

    template<class T> void connect(T *obj, void (T::*func)(Args...));
    template<class T> void disconnect(T *obj, void (T::*func)(Args...));
    template<class T> bool isConnected(T *obj, void (T::*func)(Args...));

    void operator()(Args... args);

    void flush() { m_flush = true; }

private:
    void call(Args... args);

    std::list<Functor<Args...> *> m_listeners;
    bool m_startAgain;
    bool m_flush;
};

// -- End of API --

template<class... Args>
class Functor {
public:
    virtual ~Functor() {}
    virtual void call(Args...) = 0;

    bool m_called;
};

template<class T, class... Args>
class MemberFunctor : public Functor<Args...> {
public:
    typedef void (T::*Func)(Args...);
    MemberFunctor(T *obj, Func func) : m_obj(obj), m_func(func) {}

    virtual void call(Args... args) {
        (m_obj->*m_func)(args...);
    }

    T *m_obj;
    Func m_func;
};

template<class... Args> template<class T>
void Signal<Args...>::connect(T *obj, void (T::*func)(Args...)) {
    if (!isConnected(obj, func)) {
        Functor<Args...> *f = new MemberFunctor<T, Args...>(obj, func);
        m_listeners.push_back(f);
    }
}

template<class... Args> template<class T>
void Signal<Args...>::disconnect(T *obj, void (T::*func)(Args...)) {
    for (auto i = m_listeners.begin(); i != m_listeners.end(); ++i) {
        MemberFunctor<T, Args...> *f = static_cast<MemberFunctor<T, Args...> *>(*i);
        if (f->m_obj == obj && f->m_func == func) {
            delete f;
            m_listeners.erase(i);
            m_startAgain = true;
            return;
        }
    }
}

template<class... Args> template<class T>
bool Signal<Args...>::isConnected(T *obj, void (T::*func)(Args...)) {
    for (auto i = m_listeners.begin(); i != m_listeners.end(); ++i) {
        MemberFunctor<T, Args...> *f = static_cast<MemberFunctor<T, Args...> *>(*i);
        if (f->m_obj == obj && f->m_func == func) {
            return true;
        }
    }
    return false;
}

template<class... Args>
void Signal<Args...>::operator()(Args... args) {
    for (Functor<Args...> *f: m_listeners) {
        f->m_called = false;
    }
    m_startAgain = false;
    call(args...);
    if (m_flush) {
        delete this;
    }
}

template<class... Args>
void Signal<Args...>::call(Args... args) {
    for (Functor<Args...> *f: m_listeners) {
        if (!f->m_called) {
            f->call(args...);
            f->m_called = true;
            if (m_startAgain) {
                m_startAgain = false;
                call(args...);
                return;
            }
        }
    }
}

#endif
