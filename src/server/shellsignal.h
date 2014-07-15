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
#include <functional>

template<class... Args>
class Functor;

template<class... Args>
class Signal {
public:
    Signal() : m_flush(false), m_calling(false) { }

    template<class T> void connect(T *obj, void (T::*func)(Args...));
    void connect(const std::function<void (Args...)> &func);
    template<class T> void disconnect(T *obj, void (T::*func)(Args...));
    template<class T> void disconnect(T *obj);
    template<class T> bool isConnected(T *obj, void (T::*func)(Args...));

    void operator()(Args... args);

    void flush() { m_flush = true; if (!m_calling) delete this;}

private:
    class Functor {
    public:
        Functor() : m_calling(false) {}
        virtual ~Functor() {}
        virtual void call(Args...) = 0;

        bool m_called;
        bool m_toDelete;
        bool m_calling;
    };

    class FunctionFunctor : public Functor {
    public:
        FunctionFunctor(const std::function<void (Args...)> &func) : m_func(func) {}
        virtual void call(Args... args) override { m_func(args...); }

        std::function<void (Args...)> m_func;
    };

    template<class T>
    class MemberFunctor : public Functor {
    public:
        typedef void (T::*Func)(Args...);
        MemberFunctor(T *obj, Func func) : m_obj(obj), m_func(func) {}

        virtual void call(Args... args) {
            (m_obj->*m_func)(args...);
        }

        T *m_obj;
        Func m_func;
    };

    void call(Args... args);

    std::list<Functor *> m_listeners;
    bool m_flush;
    bool m_calling;
};

// -- End of API --

template<class... Args> template<class T>
void Signal<Args...>::connect(T *obj, void (T::*func)(Args...)) {
    if (!isConnected(obj, func)) {
        Functor *f = new MemberFunctor<T>(obj, func);
        m_listeners.push_back(f);
    }
}

template<class... Args>
void Signal<Args...>::connect(const std::function<void (Args...)> &func) {
    Functor *f = new FunctionFunctor(func);
    m_listeners.push_back(f);
}

template<class... Args> template<class T>
void Signal<Args...>::disconnect(T *obj, void (T::*func)(Args...)) {
    for (auto i = m_listeners.begin(); i != m_listeners.end(); ++i) {
        MemberFunctor<T> *f = static_cast<MemberFunctor<T> *>(*i);
        if (f->m_obj == obj && f->m_func == func) {
            if (f->m_calling) {
                f->m_toDelete = true;
            } else {
                delete f;
            }
            m_listeners.erase(i);
            return;
        }
    }
}

template<class... Args> template<class T>
void Signal<Args...>::disconnect(T *obj) {
    for (auto i = m_listeners.begin(); i != m_listeners.end(); ++i) {
        MemberFunctor<T> *f = static_cast<MemberFunctor<T> *>(*i);
        if (f->m_obj == obj) {
            if (f->m_calling) {
                f->m_toDelete = true;
            } else {
                delete f;
            }
            m_listeners.erase(i);
            return;
        }
    }
}

template<class... Args> template<class T>
bool Signal<Args...>::isConnected(T *obj, void (T::*func)(Args...)) {
    for (auto i = m_listeners.begin(); i != m_listeners.end(); ++i) {
        MemberFunctor<T> *f = dynamic_cast<MemberFunctor<T> *>(*i);
        if (f && f->m_obj == obj && f->m_func == func) {
            return true;
        }
    }
    return false;
}

template<class... Args>
void Signal<Args...>::operator()(Args... args) {
    m_calling = true;
    for (Functor *f: m_listeners) {
        f->m_called = false;
    }
    call(args...);
    m_calling = false;
    if (m_flush) {
        delete this;
    }
}

template<class... Args>
void Signal<Args...>::call(Args... args) {
    for (Functor *f: m_listeners) {
        if (!f->m_called) {
            f->m_toDelete = false;
            f->m_calling = true;
            f->call(args...);
            f->m_calling = false;
            f->m_called = true;
            if (f->m_toDelete) {
                delete f;
                call(args...);
                return;
            }
        }
    }
}

#endif
