/*
Copyright (C) AD 2013-2021 Claudiu-Stefan Costea

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
*/
#ifndef PLAY_HPP
#define PLAY_HPP

#include <atomic>
#include <mutex>

#include <QMap>
#include <QDebug>
#include <QMutex>
#include <QCoreApplication>

#include "library/translations.h"

#ifdef Q_OS_ANDROID
#include <stdlib.h>
#endif

#if Q_PROCESSOR_WORDSIZE > 4
/*
If the COMPRESS_POINTERS macro is set to a non-zero value, 64bit pointers will be compressed into 32bit integers, according to the following options:
    +5 can compress addresses up to 32GB, at the expense of the 4 lower tag bits, which can no longer be used for other purporses
    +4 can compress addresses up to 16GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    +3 can compress addresses up to 8GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    +2 can compress addresses up to 4GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    +1 always stores the pointer in a thread-safe vector, returning its index, thus preserving its full form (including higher bits)
Attempting to compress an address higher than the mentioned limits, will result in the pointer being stored in a thread-safe vector;
The following negative values can also be used, but they are not safe and will lead to crashes, when the memory limits are exceeded:
    -5 can compress addresses up to 64GB, at the expense of the 4 lower tag bits, which can no longer be used for other purporses
    -4 can compress addresses up to 32GB, at the expense of the 3 lower tag bits, which can no longer be used for other purporses
    -3 can compress addresses up to 16GB, at the expense of the 2 lower tag bits, which can no longer be used for other purporses
    -2 can compress addresses up to 8GB, at the expense of the lower tag bit, which can no longer be used for other purporses
    -1 can compress addresses up to 4GB, leaving the 3 lower tag bits to be used for other purporses
Setting the ALIGN_PTR_LOW_BITS macro to a positive value, can increase the number of lower bits available in pointers for shifting,
thus allowing compression of larger adresses, but will reduce usable memory, as it will also lead to its increased fragmentation.
*/
    #define ALIGN_PTR_LOW_BITS 4
    #define COMPRESS_POINTERS 7//3
#else
    #define ALIGN_PTR_LOW_BITS 0
    #define COMPRESS_POINTERS 0
#endif

#define USE_BIT_FIELDS 1

#define CONVERT_DELEGATE(Type, Attribute, Field) \
    Attribute operator Type() const { return Field; } \
    Attribute operator const Type*() const { return &(Field); } \
    Attribute operator const Type&() const { return Field; } \
    Attribute operator Type*() { return &(Field); } \
    Attribute operator Type&() { return Field; }

#define FORWARD_DELEGATE(Type, Attribute, Field) \
    Attribute const Type& operator*() const { return Field; } \
    Attribute const Type* operator->() const { return &(Field); } \
    Attribute Type* operator->() { return &(Field); } \
    Attribute Type& operator*() { return Field; }

#define FORWARD_DELEGATE_PTR(Type, Attribute, Field) \
    FORWARD_DELEGATE(Type, Attribute, *Field)

#define CONVERT_DELEGATE_PTR(Type, Attribute, Field) \
    CONVERT_DELEGATE(Type, Attribute, *Field)

#define PROP_FIELD_GET_CUSTOM(Name, Type, GetLevel, Field) \
    GetLevel: inline Type Name() const \
    { \
        return this->Field; \
    }

#define PROP_FIELD_GET_NEW(Name, Type, GetLevel, Field, FieldLevel) \
    FieldLevel: Type Field; \
    PROP_FIELD_GET_CUSTOM(Name, Type, GetLevel, Field)

#define PROP_FIELD_GET(Name, Type, Level) \
    PROP_FIELD_GET_NEW(Name, Type, Level, _##Name, protected)

#define PROP_REF_GET(Name, Type, Level, Field) \
    Level: inline Type& Name() const \
    { \
        return *this->Field; \
    }

#define PROP_REF_GET_NEW(Name, Type, Level, Field, FieldLevel) \
    FieldLevel: Type* Field; \
    PROP_REF_GET(Name, Type, GetLevel, Field)

#define PROP_FIELD_SET(Name, Type, Level, Field) \
    Level: inline void Name(Type const value) \
    { \
        this->Field = value; \
    }

#define PROP_FIELD_SWAP(SwapName, SetName, GetType, SetType, Level, GetName) \
    Level: inline GetType SwapName(SetType value) \
    { \
        GetType old = this->GetName(); \
        this->SetName(value); \
        return old; \
    }

#define PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    Level: inline Class& WithName(Type value) \
    { \
        this->SetName(value); \
        return *this; \
    }

#define PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Level, GetName) \
    PROP_FIELD_WITH(Class, WithName, Type, Level, SetName) \
    PROP_FIELD_SWAP(SwapName, SetName, Type, Type, Level, GetName)

#define PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, Level, GetName, Field) \
    PROP_FIELD_SET(SetName, Type, Level, Field) \
    PROP_FIELD_WITH_SWAP(Class, SetName, SwapName, WithName, Type, Level, GetName)

#define PROP_CUSTOM_FIELD_NEW(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field)

#define PROP_CUSTOM_FIELD(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_FIELD_GET_CUSTOM(GetName, Type, GetLevel, Field) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type, SetLevel, GetName, Field)

#define PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD_NEW(Class, GetName, set##PropName, swap##PropName, with##PropName, Type, GetLevel, SetLevel, _##GetName)

#if (defined(__clang__) || defined(_MSC_VER))
#define PROP_DECL_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    PROP_FIELD(Class, PropName, GetName, Type, GetLevel, SetLevel) \
    __declspec(property(put = set##PropName, get = GetName)) Type PropName;
#endif

#define PROP_CAMEL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, get##Name, set##Name swap##Name, with##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_SNAKE_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Name, set_##Name swap_##Name, with_##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_PASCAL_FIELD(Class, Name, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_FIELD(Class, Get##Name, Set##Name, Swap##Name, With##Name, Type, GetLevel, SetLevel, _##Name)

#define PROP_CUSTOM_REF(Class, GetName, SetName, SwapName, WithName, Type, GetLevel, SetLevel, Field) \
    PROP_REF_GET_NEW(GetName, Type, GetLevel, Field, protected) \
    PROP_FIELD_SET_ALL(Class, SetName, SwapName, WithName, Type*, SetLevel, GetName, Field)

#define PROP_REF(Class, GetName, SetName, Type, GetLevel, SetLevel) \
    PROP_CUSTOM_REF(Class, GetName, set##SetName, swap##SetName, with##SetName, Type, GetLevel, SetLevel, _##GetName)

#define PROP_FLAG_SET(Name, Flag, Level) \
    Level: inline void Name(bool const value) \
    { \
        this->setFlag(Flag, value); \
    }

#define PROP_FLAG_SET_ALL(Class, PropName, Flag, Level, GetName) \
    PROP_FLAG_SET(set##PropName, Flag, Level) \
    PROP_FIELD_WITH_SWAP(Class, set##PropName, swap##PropName, with##PropName, bool, Level, GetName)

#define PROP_FLAG_GET(Name, Flag, Level) \
    Level: inline bool Name() const \
    { \
        return this->hasAllFlags(Flag); \
    }

#define PROP_FLAG(Class, Name, Flag, GetLevel, SetLevel) \
    PROP_FLAG_GET(is##Name, Flag, GetLevel) \
    PROP_FLAG_SET_ALL(Class, Name, Flag, SetLevel, is##Name)

namespace
{

    template <typename T, class P, const int opt = -1> class BasePtr
    {
    protected:
        template<typename... Args>
        inline static P make(Args&&... args)
        {
            return P(new T(std::forward<Args>(args)...));
        }

        inline T& def() const
        {
            if constexpr(std::is_default_constructible<T>::value && (opt == 0 || opt < -1))
            {
                return (const_cast<BasePtr<T, P, opt>*>(this))->refOrNew();
            }
            else
            {
                return const_cast<BasePtr<T, P, opt>*>(this)->obj();
            }
        }

        inline const T& obj() const
        {
            return *static_cast<const P*>(this)->P::addr();
        }

        inline T& obj()
        {
            return *static_cast<P*>(this)->P::addr();
        }

    public:
        FORWARD_DELEGATE(T, inline, def())
        CONVERT_DELEGATE(T, inline explicit, obj())

        template<typename R = T*>
        inline auto ptr() -> std::enable_if_t<(opt > 1), R>
        {
            return static_cast<P*>(this)->P::addr();
        }

        template<typename R = const T*>
        inline auto ptr() const -> std::enable_if_t<(opt > 1), R>
        {
            return static_cast<P*>(this)->P::addr();
        }

        template<typename R = T*>
        inline auto swapPtr(T* const ptr) -> std::enable_if_t<(opt > 1), R>
        {
            auto _ptr = static_cast<P*>(this)->addr();
            static_cast<P*>(this)->P::setPntr(ptr);
            return _ptr;
        }

        template<typename R = T*>
        inline auto takePtr() -> std::enable_if_t<(opt > 1), R>
        {
            return this->swapPtr(nullptr);
        }

        template<typename R = P&>
        inline auto withPtr(T* const ptr) -> std::enable_if_t<(opt > 1), R>
        {
            static_cast<P*>(this)->P::setPntr(ptr);
            return *this;
        }

        /*inline auto resetPtr(T* const ptr = nullptr) -> std::enable_if_t<(opt > 1), void>
        {
            static_cast<P*>(this)->P::setPntr(ptr);
        }*/

        inline T& operator()() const
        {
            return this->def();
        }

        inline operator bool() const
        {
            if constexpr(opt != 0)
            {
                return static_cast<P*>(this)->_ptr;
            }
            else
            {
                return true;
            }
        }

        //inline auto operator=(std::nullptr_t) -> std::enable_if_t<(opt > 1), P&>
        inline P& operator=(std::nullptr_t)
        {
            static_assert(opt > 1, "Assignment from pointers not allowed.");
            static_cast<P*>(this)->P::setPntr(static_cast<std::nullptr_t>(nullptr));
            return *static_cast<P*>(this);
        }

        //inline auto operator=(T* const ptr) -> std::enable_if_t<(opt > 1), P&>
        inline P& operator=(T* const ptr)
        {
            static_assert(opt > 1, "Assignment from pointers not allowed.");
            static_cast<P*>(this)->P::setPntr(ptr);
            return *static_cast<P*>(this);
        }

        inline bool operator<(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() < ptr;
        }

        inline bool operator<(const P& cloned) const
        {
            return static_cast<P*>(this)->_ptr < cloned._ptr;
        }

        inline bool operator>(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() > ptr;
        }

        inline bool operator>(const P& cloned) const
        {
            return static_cast<P*>(this)->_ptr > cloned._ptr;
        }

        inline bool operator<=(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() <= ptr;
        }
        inline bool operator<=(const P& cloned) const
        {
            return static_cast<P*>(this)->_ptr <= cloned._ptr;
        }

        inline bool operator>=(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() >= ptr;
        }

        inline bool operator>=(const P& cloned) const
        {
            return static_cast<P*>(this)->_ptr >= cloned._ptr;
        }

        inline bool operator==(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() == ptr;
        }

        inline bool operator==(const P& cloned) const
        {
            return static_cast<P*>(this)->_ptr == cloned._ptr;
        }

        inline bool operator!=(const T* const ptr) const
        {
            return static_cast<P*>(this)->addr() != ptr;
        }

        inline bool operator!=(const P& cloned)
        {
            return static_cast<P*>(this)->_ptr != cloned._ptr;
        }

        inline P& operator=(P&& cloned)
        {
            if (this != &cloned)
            {
                static_cast<P*>(this)->move(std::forward<P>(cloned));
            }
            return *static_cast<P*>(this);
        }

        inline P& operator=(const P& cloned)
        {
            if (this != &cloned)
            {
                static_cast<P*>(this)->copy(cloned);
            }
            return *static_cast<P*>(this);
        }

        //inline auto operator=(const T& cloned) -> std::enable_if_t<(opt > -2), P&>
        inline P& operator=(T& cloned)
        {
            //this->setRef(cloned);
            //return *(static_cast<P*>(this));
            static_assert(opt > -2, "Cannot re-assign weak reference.");
            static_cast<P*>(this)->P::setPntr(&(const_cast<T&>(cloned)));
            return *static_cast<P*>(this);
        }

        template<typename R = void>
        inline auto setPtr(std::nullptr_t) -> std::enable_if_t<(opt > 1), R>
        {
            static_cast<P*>(this)->P::setPntr(static_cast<std::nullptr_t>(nullptr));
        }

        template<typename R = void>
        inline auto setRef(T& cloned) -> std::enable_if_t<(opt > -2), R>
        {
            static_cast<P*>(this)->P::setPntr(&cloned);
        }

        template<typename R = T&>
        inline auto ref() -> std::enable_if_t<opt == 0, R>
        {
            return this->obj();
        }

        template<typename R = const T&>
        inline auto ref() const -> std::enable_if_t<opt == 0, R>
        {
            return this->obj();
        }

        template<typename R = void>
        inline auto resetRef() -> std::enable_if_t<(opt > 0), R>
        {
            static_cast<P*>(this)->P::setPntr(static_cast<std::nullptr_t>(nullptr));
        }

        template<typename R = bool>
        inline auto hasRef() const -> std::enable_if_t<opt != 0, R>
        {
            return this->operator bool();
        }

        template<typename R = T&>
        inline auto refOrFail() -> std::enable_if_t<opt != 0, R>
        {
            return this->obj();
        }

        template<typename R = const T&>
        inline auto refOrFail() const -> std::enable_if_t<opt != 0, R>
        {
            return this->obj();
        }

        template<typename R = T&>
        inline auto refOrElse(T& def) const -> std::enable_if_t<opt != 0, R>
        {
            auto ptr = static_cast<P*>(this)->P::addr();
            return ptr ? *ptr : def;
        }

        template<typename... Args, typename R = T&>
        inline auto refOrNew(Args&&... args) -> std::enable_if_t<(opt != 0 && opt > -2), R>
        {
            auto ptr = static_cast<P*>(this)->P::addr();
            if (ptr == nullptr)
            {
                ptr = new T(std::forward<Args>(args)...);
                static_cast<P*>(this)->P::setPntr(ptr);
            }
            return *ptr;
        }

        template<typename R = T&>
        inline auto refOrDef() const -> std::enable_if_t<std::is_nothrow_default_constructible<T>::value, R>
        {
            return this->def();
        }

        template<typename R = T&>
        inline auto refOrSet(T& def) -> std::enable_if_t<(opt != 0 && opt > -2), R>
        {
            auto ptr = static_cast<P*>(this)->P::addr();
            if (ptr == nullptr)
            {
                ptr = &def;
                static_cast<P*>(this)->P::setPntr(ptr);
            }
            return *ptr;
        }

        template<typename R = void>
        inline auto setPtr(T* const ptr) -> std::enable_if_t<(opt > 1), R>
        {
            static_cast<P*>(this)->P::setPntr(ptr);
        }

        template<typename F, typename R = void>
        inline auto runIfRef(F callback) -> std::enable_if_t<opt != 0, R>
        {
            auto ptr = static_cast<P*>(this)->P::addr();
            if (ptr)
            {
                callback(*ptr);
            }
        }

        template<typename F, typename R>
        inline auto callIfRef(F callback, R defValue) -> std::enable_if_t<opt != 0, R>
        {
            auto ptr = static_cast<P*>(this)->P::addr();
            if (ptr)
            {
                return callback(*ptr);
            }
            else
            {
                return defValue;
            }
        }

        inline BasePtr<T, P, opt>(const P& cloned)
        {
            static_cast<P*>(this)->P::copy(cloned);
        }

        inline BasePtr<T, P, opt>(P&& cloned)
        {
            static_cast<P*>(this)->P::move(std::forward<P>(cloned));
        }

        inline BasePtr<T, P, opt>(T& ptr)
        {
            static_cast<P*>(this)->P::setAddr(&ptr);
        }

        inline BasePtr<T, P, opt>(T* const ptr)
        {
            static_assert(opt != 0, "This reference is not optional and cannot be initialized from nullable pointers.");
            static_cast<P*>(this)->P::setAddr(ptr);
        }

        inline BasePtr<T, P, opt>()
        {
            static_assert(opt != 0 && opt > -2, "This reference is not optional and must be initialized.");
        }
    };
#if COMPRESS_POINTERS > 0
    class PtrList
    {
    protected:
        inline static uint32_t _null_idx = 0U;
        inline static std::vector<void*> _ptr_list;
        //inline static std::mutex _locker;
        inline static QMutex _locker;

        inline static bool listed(const uint32_t ptr)
        {
            return (ptr & 1U) == 1U;
        }
    };
#define CMPS_LEVEL COMPRESS_POINTERS - 2
    template<typename T, const int own = 0, const int opt = -1, const int level = CMPS_LEVEL>
    class BaseCmp : public BasePtr<T, BaseCmp<T, own, opt, level>, opt>, protected PtrList
    {
        static constexpr uint32_t CmpsLengthShift(int cmpsLevel)
        {
            if (cmpsLevel == -1)
            {
                return 0;
            }
            if (cmpsLevel < -1)
            {
                cmpsLevel = (cmpsLevel * -1) - 1;
            }
#if ALIGN_PTR_LOW_BITS > 0
#define ALIGN_POINTERS 1U << ALIGN_PTR_LOW_BITS
            uint32_t bits = ALIGN_PTR_LOW_BITS - 1;
            return static_cast<uint32_t>(cmpsLevel) > bits ? bits : cmpsLevel;
#else
            return cmpsLevel > 1 ? 2 : cmpsLevel;
#endif
        }

        uint32_t _ptr = 0U;

    protected:
        static bool clearList(uint32_t ptr)
        {
            if (ptr == 0U)
            {
                return false;
            }
            if (listed(ptr))
            {
                //auto uniqueLocker = std::unique_lock(PtrList::_locker);
                auto uniqueLocker = QMutexLocker(&PtrList::_locker);
                //uniqueLocker.lock();
                //_locker.lock();
                auto ptrList = &_ptr_list;
                if ((ptr >>= 1) == ptrList->size())
                {
                    size_t ptrListLen;
                    do
                    {
                        ptrList->pop_back();
                    }
                    while ((ptrListLen = ptrList->size()) > 0 && (--ptr) == ptrListLen && ptrList->at(ptr - 1U) == nullptr);
                    if (_null_idx > ptrListLen - 2)
                    {
                        _null_idx = 0U;
                    }
                    //ptrList->shrink_to_fit();
                }
                else
                {
                    auto idx = ptr - 1U;
                    (*ptrList)[idx] = nullptr;
                    if (idx < _null_idx)
                    {
                        _null_idx = idx;
                    }
                }
                //uniqueLocker.unlock();
                //_locker.unlock();
            }
            return true;
        }

        inline T* addr() const
        {
            const uint32_t ptr = this->_ptr;
            if (ptr == 0)
            {
                return nullptr;
            }
            else if (listed(ptr))
            {
                return static_cast<T*>(_ptr_list[(ptr >> 1) - 1U]);
            }
            else
            {
                return reinterpret_cast<T*>(static_cast<uintptr_t>(this->_ptr) << SHIFT_LEN);
            }
        }

        void listPtr(T* const ptr)
        {
            //qDebug() << "listPtr: ptr = " << ptr;
            uint32_t oldPtr = this->_ptr;
            //auto uniqueLocker = std::unique_lock(PtrList::_locker);
            auto uniqueLocker = QMutexLocker(&PtrList::_locker);
            //uniqueLocker.lock();
            //_locker.lock();
            auto ptrList = &_ptr_list;
            if (listed(oldPtr))
            {
                oldPtr >>= 1;
                if (oldPtr > 0U)
                {
                    (*ptrList)[oldPtr - 1U] = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    return;
                }
            }
            uint32_t ptrListLen = ptrList->size();
            for (uint32_t i = _null_idx; i < ptrListLen; i += 1U)
            {
                if (ptrList->at(i) == nullptr)
                {
                    //_null_idx = i == ptrListLen - 1 ? 0U : i + 1U;
                    ptrList->operator[](i) = const_cast<void*>(reinterpret_cast<const void*>(ptr));
                    this->_ptr = (i << 1U) | 1U;
                    _null_idx = i + 1U;
                    return;
                }
            }
            _null_idx = ptrListLen;
            ptrList->push_back(const_cast<void*>(reinterpret_cast<const void*>(ptr)));
            this->_ptr = static_cast<uint32_t>(((ptrListLen + 1) << 1) | 1);
            //qDebug() << "listPtr: this->_ptr = " << _ptr;
            //uniqueLocker.unlock();
            //_locker.unlock();
        }

        inline void setAddr(std::nullptr_t)
        {
            if (clearList(this->_ptr))
            {
                //qDebug() << "setAddr: ptr = nullptr";
                this->_ptr = 0U;
            }
        }

        void setAddr(T* const ptr)
        {
            //qDebug() << "level = " << level;
            //qDebug() << "setAddr: ptr = " << QString::number(reinterpret_cast<uint64_t>(ptr), 2);
            if (this->addr() == ptr)
            {
                return;
            }
            else if (ptr == nullptr)
            {
                this->setAddr(static_cast<std::nullptr_t>(nullptr));
                return;
            }
            if constexpr(level == -1)
            {
                this->listPtr(ptr);
            }
            else
            {
                uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
                //qDebug() << "4294967296UL << SHIFT_LEN = " << QString::number(4294967296UL << SHIFT_LEN);
                //qDebug() << "SHIFT_LEN = " << QString::number(SHIFT_LEN);
                if (addr < (4294967296UL << SHIFT_LEN))
                //if (addr < (10000UL))
                {
                    //if constexpr(own)
                    {
                        clearList(this->_ptr);
                    }
                    this->_ptr = static_cast<uint32_t>(addr >> SHIFT_LEN);
                    //qDebug() << "setAddr: this->_ptr = " << _ptr;
                }
                else
                {
                    this->listPtr(ptr);
                }
            }
        }

    public:
        inline bool comrpessed() const
        {
            return !listed(this->_ptr);
        }

        inline BaseCmp<T, own, opt, level>() {}

        inline ~BaseCmp<T, own, opt, level>()
        {
            if constexpr(own)
            {
                auto ptr = this->addr();
                if (ptr)
                {
                    clearList(this->_ptr);
                    delete ptr;
                }
            }
        }
#else
#define CMPS_LEVEL COMPRESS_POINTERS < -1 ? COMPRESS_POINTERS + 1 : 0
    template<typename T, const int own = 0, const int opt = -1, const int level = CMPS_LEVEL>
    class BaseCmp : public BasePtr<T, BaseCmp<T, own, opt, level>, opt>
    {
    #if COMPRESS_POINTERS == 0
        static constexpr uint CmpsLengthShift(const int cmpsLevel)
        {
            return cmpsLevel;
        }

        T* _ptr;

    protected:
        inline void setAddr(T* const ptr)
        {
            this->_ptr = ptr;
        }

    public:
        inline bool comrpessed() const
        {
            return false;
        }

        inline T* addr() const
        {
            return this->_ptr;
        }

        inline BaseCmp<T, own, opt, level>()
        {
            this->setPntr(nullptr);
        }
    #else
        static constexpr uint CmpsLengthShift(int cmpsLevel)
        {
            if (cmpsLevel < 0)
            {
                cmpsLevel *= -1;
            }
#if ALIGN_PTR_LOW_BITS > 0
#define ALIGN_POINTERS 1U << ALIGN_PTR_LOW_BITS
            uint32_t bits = ALIGN_PTR_LOW_BITS;
            return static_cast<uint32_t>(cmpsLevel) > bits ? bits : cmpsLevel;
#else
            return cmpsLevel > 2 ? 3 : cmpsLevel;
#endif
        }

        uint32_t _ptr;

    protected:
        inline void setAddr(std::nullptr_t)
        {
            this->_ptr = 0U;
        }

        inline void setAddr(T* const ptr)
        {
            uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
            assert(addr < (4294967296UL << SHIFT_LEN)); //TODO: analyze alternative solutions
            this->_ptr = static_cast<uint32_t>(addr >> SHIFT_LEN);
        }

    public:
        inline bool comrpessed() const
        {
            return true;
        }

        inline T* addr() const
        {
            return reinterpret_cast<T*>(static_cast<uintptr_t>(this->_ptr) << SHIFT_LEN);
        }

        inline BaseCmp<T, own, opt, level>()
        {
            this->_ptr = 0U;
        }
    #endif
    public:
        inline ~BaseCmp<T, own, opt, level>()
        {
            if constexpr(own)
            {
                auto ptr = this->addr();
                if (ptr)
                {
                    delete ptr;
                }
            }
        }
#endif
    protected:
        static constexpr int SHIFT_LEN = CmpsLengthShift(level);

        inline void copy(const BaseCmp<T, own, opt, level>& cloned)
        {
            static_assert(own < 1, "Attempting to clone unique pointer.");
            this->_ptr = cloned._ptr;
            if constexpr(own < 0)
            {
                const_cast<BaseCmp<T, own, opt, level>&>(cloned)._ptr = 0U;
            }
        }

        inline void move(BaseCmp<T, own, opt, level>&& cloned)
        {
            this->_ptr = cloned._ptr;
            cloned._ptr = 0U;
        }

        inline void setPntr(std::nullptr_t)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(static_cast<std::nullptr_t>(nullptr));
        }

        inline void setPntr(T* const ptr)
        {
            static_assert(!own, "Attempting to change unique pointer.");
            this->setAddr(ptr);
        }

    public:
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::BasePtr;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator*;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator->;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator();
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator bool;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator==;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator!=;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator>=;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator<=;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator>;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator<;
        using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::operator=;
        //using BasePtr<T, BaseCmp<T, own, opt, level>, opt>::setRef;

        template <typename, typename, typename, const int> friend struct RefData;
        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template <typename, typename, typename, const int> friend struct ShrData;
        template <typename, typename, const int> friend struct PtrData;
        template <typename, class, const int> friend class BasePtr;

    };

    template <typename C>
    struct CntData
    {
    private:
        BaseCmp<C, 0, 2, 9> _ref_cnt;

    protected:
        inline CntData<C>& countData()
        {
            return *this;
        }

        inline BaseCmp<C, 0, 2, 9>& cntDataRef()
        {
            return this->_ref_cnt;
        }

        template <typename, typename, const int> friend struct PtrData;
        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template <typename, typename, typename, const int> friend struct RefData;
    };

    template <typename T, typename C, const int level>
    struct PtrData : CntData<C>
    {
    private:
        BaseCmp<T, 0, 2, level> _ptr;

    protected:
        inline PtrData<T, C, level>& countData()
        {
            return *this;
        }

        inline BaseCmp<T, 0, 2, level>& ptrDataRef()
        {
            return this->_ptr;
        }

        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template <typename, typename, typename, const int> friend struct RefData;
    };

    template <typename T, typename P, typename C, const int level>
    struct ShrData : T
    {
    private:
        BaseCmp<std::vector<BaseCmp<P, 0, 2, 9>>, 0, 2, 9> _weak_vct;
        BaseCmp<QMutex, 0, 2, 9> _locker;

    protected:
        inline ShrData<T, P, C, level>& countData()
        {
            return *this;
        }

        inline BaseCmp<std::vector<BaseCmp<P, 0, 2, 9>>, 0, 2, 9>& vctDataRef()
        {
            return this->_weak_vct;
        }

        inline BaseCmp<QMutex, 0, 2, 9>& lckDataRef()
        {
            return this->_locker;
        }

        void track(P& weakRef)
        {
            auto locker = this->_locker.ptr();
            if (locker == nullptr)
            {
                locker = new QMutex;
                this->_locker.setPtr(locker);
            }
            auto uniqueLocker = QMutexLocker(locker);
            auto weakVct = this->_weak_vct.ptr();
            if (weakVct == nullptr)
            {
                weakVct = new std::vector<BaseCmp<P, 0, 2, 9>>;
                this->_weak_vct.setPtr(weakVct);
            }
            weakVct.push_back(BaseCmp<P, 0, 2, 9>(weakRef));
            //uniqueLocker.unlock();
        }

        void untrack(P* weakRef)
        {
            auto locker = this->_locker.ptr();
            if (locker)
            {
                auto uniqueLocker = QMutexLocker(locker);
                auto weakVct = this->_weak_vct.ptr();
                if (weakVct)
                {
                    auto weakEnd = weakVct->end();
                    for (auto itr = weakVct->begin(); itr != weakEnd; ++itr)
                    {
                        if (itr->addr() == weakRef)
                        {
                            weakVct->erase(itr);
                            break;
                        }
                    }
                }
                //uniqueLocker.unlock();
            }

        }

        void nullify()
        {
            auto locker = this->_locker.ptr();
            if (locker)
            {
                auto uniqueLocker = QMutexLocker(locker);
                auto weakVct = this->_weak_vct.ptr();
                if (weakVct)
                {
                    const int vctSize = weakVct->size();
                    for (unsigned int i = 0; i < vctSize; i += 1)
                    {
                        ((*weakVct)[i])->setAddr(nullptr);
                    }
                }
                delete weakVct;
                this->_locker.setPtr(nullptr);
                //this->_weak_vct.setPtr(nullptr);
                uniqueLocker.unlock();
                delete locker;
            }
        }

        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template <typename, typename, typename, const int> friend struct RefData;
        template <typename, class, const int> friend class BasePtr;
    };

    template <typename T, typename P, typename C, const int level>
    struct RefData
    {
    private:
        BaseCmp<ShrData<T, P, C, level>, 1, 2, 9> _ref_data;

    protected:
        inline ShrData<T, P, C, level>& countData()
        {
            return *(this->_ref_data.ptr());
        }

        inline BaseCmp<std::vector<BaseCmp<P, 0, 2, 9>>, 0, 2, 9>& vctDataRef()
        {
            return this->countData().vctDataRef();
        }

        inline BaseCmp<QMutex, 0, 2, 9>& lckDataRef()
        {
            return this->countData().lckDataRef();
        }

        inline BaseCmp<T, 0, 2, level>& ptrDataRef()
        {
            return this->countData().ptrRef();
        }

        inline BaseCmp<C, 0, 2, 9>& cntDataRef()
        {
            return this->countData().cntDataReF();
        }

        inline void track(P& weakRef)
        {
            this->countData().track(weakRef);
        }

        inline void untrack(P* weakRef)
        {
            this->countData().untrack(weakRef);
        }

        inline void nullify()
        {
            this->countData().nullify();
        }

        inline RefData<T, P, C, level>()
        {
            this->_ref_data = BaseCmp<ShrData<T, P, C, level>, 1, 2, 9>(new ShrData<T, P, C, level>);
        }

        template <typename, const int, const bool, const int, typename, const int> friend class BaseCnt;
        template <typename, class, const int> friend class BasePtr;
    };

    template <typename T, const int cow = 0, const bool weak = false, const int opt = -1,
              typename C = std::atomic<uint32_t>, const int level = CMPS_LEVEL>
    class BaseCnt : public std::conditional_t<cow == 0, RefData<PtrData<T, C, level>, BaseCnt<T, 0, true, opt, C, level>, C, level>, PtrData<T, C, level>>,
                    public BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>
    {
        static_assert(cow < 1 || !weak, "Copy-on-write not allowed for weak references.");
        //BaseCmp<C, 0, 2, 3> _ref_cnt;
        //BaseCmp<T, 0, 2, level> _ptr;

    protected:
        template<typename R = void>
        inline auto increase() -> std::enable_if_t<(!weak), R>
        {
            auto cnt = this->cntDataRef();
            if (cnt)
            {
                (*cnt) += 1U;
            }
        }

        template<typename R = void>
        inline auto decrease() -> std::enable_if_t<(!weak), R>
        {
            auto& tData = this->countData();
            auto ptr = tData.ptrDataRef().addr();
            if (ptr)
            {
                if constexpr(cow == 0 && !weak) //tracking weak references
                {
                    this->nullify();
                }
                auto cnt = tData.cntDataRef().addr();
                if (--(*cnt) == 0U)
                {
                    delete ptr;
                    delete cnt;
                }
            }
        }

        inline T* addr() const
        {
            /*if constexpr(cow == 0 && weak) //tracking weak references
            {
                auto locker = tData.lckDataRef().ptr();
                if (locker)
                {
                    auto uniqueLocker = QMutexLocker(locker);
                    return tData.ptrDataRef().addr();
                }
            }*/
            return const_cast<BaseCnt<T, cow, weak, opt, C, level>*>(this)->countData().ptrDataRef().addr();
        }

        inline void setAddr(T* const ptr)
        {
            auto& tData = this->countData();
            tData.cntDataRef().setPntr(ptr ? new C(1U) : nullptr);
            tData.ptrDataRef().setPntr(ptr);
        }

        inline void copy(const BaseCnt<T, cow, weak, opt, C, level>& cloned)
        {
            if constexpr(!weak)
            {
                cloned.increase();
            }
            auto& tData = this->countData();
            auto& cData = cloned.countData();
            tData.cntDataRef() = cData.cntDataRef();
            tData.ptrDataRef() = cData.ptrDataRef();
            if constexpr(cow == 0) //tracking weak references
            {
                tData.vctDataRef() = cData.vctDataRef();
                tData.lckDataRef() = cData.lckDataRef();
            }
        }

        inline void move(BaseCnt<T, cow, weak, opt, C, level>&& cloned)
        {
            this->copy(cloned);
            auto& cData = cloned.countData();
            if constexpr(cow == 0) //tracking weak references
            {
                cData.vctDataRef()._ptr = 0U;
                cData.lckDataRef()._ptr = 0U;
            }
            cData.cntDataRef()._ptr = 0U;
            cData.ptrDataRef()._ptr = 0U;
        }

        template<typename R = void>
        inline auto setPntr(T* const ptr) -> std::enable_if_t<(!weak), R>
        //inline void setPntr(T* const ptr)
        {
            this->decrease();
            this->setAddr(ptr);
        }

    public:
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::BasePtr;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator*;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator->;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator();
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator bool;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator==;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator!=;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator>=;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator<=;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator>;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator<;
        using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::operator=;
        //using BasePtr<T, BaseCnt<T, cow, weak, opt, C, level>, weak ? -2 : opt>::setRef;

        template<typename R = void>
        inline auto detach(const bool always = true) const -> std::enable_if_t<(cow != 0 && !weak), R>
        {
            auto& tData = this->countData();
            auto ptr = tData.ptrDataRef().addr();
            if (ptr)
            {
                if (always || (*tData.cntDataRef()) > 1)
                {
                    this->setPntr(new T(*ptr));
                }
            }
        }

        template<typename R = std::conditional_t<weak, const BaseCnt<T, 0, false, opt, C, level>, const T*>>
        inline auto ptr() const -> std::enable_if_t<(opt > 1), R>
        {
            if constexpr(weak)
            {
                return *this;
            }
            else
            {
                return this->addr();
            }
        }

        template<typename R = std::conditional_t<weak, BaseCnt<T, 0, false, opt, C, level>, T*>>
        inline auto ptr() -> std::enable_if_t<(opt > 1), R>
        {
            if constexpr(weak)
            {
                return *this;
            }
            else
            {
                if constexpr(cow > 0)
                {
                    this->detach(false);
                }
                return this->countData().ptrDataRef().addr();
            }
        }

        template<typename R = BaseCnt<T, 0, true, opt, C, level>>
        inline auto weakRef() -> std::enable_if_t<(cow > 0 && !weak), R>
        {
            if constexpr(cow == 0) //tracking weak references
            {
                BaseCnt<T, 0, true, opt, C, level> weakRef = *this;
                this->track(weakRef);
                return weakRef;
            }
            else
            {
                return *this;
            }
        }

        template<typename R = BaseCnt<T, 0, false, opt, C, level>>
        inline auto sharedRef() -> std::enable_if_t<(weak), R>
        {
            return *this;
        }

        /*inline CmprShr(const BaseCmp<T, false, level>& cloned)
        {
            this->setAddr(cloned.addr());
        }*/

        inline BaseCnt<T, cow, weak, opt, C, level>()
        {
            this->setAddr(nullptr);
        }

        inline ~BaseCnt<T, cow, weak, opt, C, level>()
        {
            if constexpr(weak)
            {
                if constexpr(cow == 0) //tracking weak references
                {
                    this->untrack(this);
                }
            }
            else
            {
                this->decrease();
            }
        }

        template <typename, typename, typename, const int> friend struct ShrData;
        template <typename, typename, const int> friend struct PtrData;
        template <typename, class, const int> friend class BasePtr;
    };

    /*template <typename T, const int cow = 0, const bool weak = false, const int opt = -1,
              typename C = std::atomic<uint32_t>, const int level = CMPS_LEVEL>
    class BaseShr : public BasePtr<T, BaseShr<T, cow, weak, opt, C, level>, weak ? -2 : opt>
    {
        BaseCmp<BaseCnt<T, cow, weak, opt, C, level>, 1, 2, 9> _shr_ref;

    public:
        template <typename, class, const int> friend class BasePtr;
    };*/
}

#if ALIGN_POINTERS
    #ifdef _MSC_VER
inline void* alloc(const std::size_t size)
{
    return _aligned_malloc(size, ALIGN_POINTERS);
}

inline void clear(void* ptr) noexcept
{
    _aligned_free(ptr);
}
    #else
inline void clear(void* ptr) noexcept
{
    free(ptr);
}
        #ifdef Q_OS_ANDROID
inline void* alloc(const std::size_t size)
{
    void* ptr;
    posix_memalign(&ptr, ALIGN_POINTERS, size);
    return ptr;
}
        #else
inline void* alloc(const std::size_t size)
{
    return std::aligned_alloc(ALIGN_POINTERS, size);
}
        #endif
    #endif
inline void* operator new(const std::size_t size, std::nothrow_t)
{
    return alloc(size);
}

inline void* operator new(const std::size_t size)
{
    auto ptr = alloc(size);
    if (ptr)
    {
        return ptr;
    }
    else
    {
        throw std::bad_alloc {};
    }
}

inline void operator delete(void* ptr) noexcept
{
    clear(ptr);
}
#else
inline void* alloc(const std::size_t size)
{
    return std::malloc(size);
}

inline void clear(void* ptr) noexcept
{
    free(ptr);
}
#endif
namespace tbrpgsca
{
    template<typename T, const int own = 0, const int opt = -1, const int level = CMPS_LEVEL>
    using CmpsPtr = BaseCmp<T, own, opt, level>;
    template<typename T, const bool weak = false, const int cow = -1, const int opt = -1, typename C = std::atomic<uint32_t>, const int level = CMPS_LEVEL>
    using CmpsCnt = BaseCnt<T, cow, weak, opt, C, level>;

    class Ability;
    class Costume;
    class Actor;
    class State;

    class Play
    {
        //Q_DECLARE_TR_FUNCTIONS(Play)
        PROP_CUSTOM_FIELD(Play, playFlags, setPlayFlags, swapPlayFlags, withPlayFlags, int, public, protected, _play_flags)
    public:
        static void FreeDemoMemory();
        static QVector<Actor*>& Players();
        static QVector<const State*>& States();
        static QVector<QVector<const Ability*>*>& Abilities();
        static QVector<QMap<const State*, int>*>& StateMasks();
        static QVector<QVector<Actor*>*>& Enemies();
        static QVector<const Ability*>& PartyItems();
        static QVector<const Costume*>& Races();
        static QVector<const Costume*>& Jobs();

        inline bool hasAllFlags(unsigned int const flag) const
        {
            return (this->_play_flags & flag) == flag;
        }
        inline bool hasAnyFlag(unsigned int const flag) const
        {
            return (this->_play_flags & flag) != 0;
        }
    protected:
        inline static QVector<Actor*> _Players;
        inline static QVector<QVector<Actor*>*> _Enemies;
        inline static QVector<QMap<const State*, int>*> _State_Masks;
        inline static QVector<QVector<const Ability*>*> _Abilities;
        inline static QVector<const Costume*> _Races, _Jobs;
        inline static QVector<const Ability*> _PartyItems;
        inline static QVector<const State*> _States;

#if USE_BIT_FIELDS
        unsigned int _play_flags: 16;
#else
        unsigned int _play_flags;
#endif
        inline void setFlag(int const flag, bool const value)
        {
            int const flags = this->_play_flags;
            if (value != ((flags & flag) == flag))
            {
                this->_play_flags = flags ^ flag;
            }
        }

        inline Play& withFlag(int const flag, bool const value)
        {
            this->setFlag(flag, value);
            return *this;
        }

        Play();

        Play(int const flags);

        ~Play();
    };

}

#endif // PLAY_HPP
