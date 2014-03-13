// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal

#ifndef ___OPTIONAL_HPP___
#define ___OPTIONAL_HPP___

#include <utility>
#include <type_traits>
#include <initializer_list>
#include <cassert>
#include <functional>
#include <string>
#include <stdexcept>

#include "Util/Nothing.hpp"

#define REQUIRES(...) typename std::enable_if<__VA_ARGS__::value, bool>::type = false

namespace callophrys {
namespace util {

// 20.5.4, optional for object types
template <class T> class optional;

// 20.5.5, optional for lvalue reference types
template <class T> class optional<T&>;

template <class U>
struct is_not_optional
{
    constexpr static bool value = true;
};

template <class T>
struct is_not_optional<optional<T>>
{
    constexpr static bool value = false;
};

static struct trivial_init_t {} trivial_init{};
static struct only_set_initialized_t{} only_set_initialized{};
static struct in_place_t {} in_place {};

struct bad_optional_access : public std::logic_error {
    bad_optional_access(const std::string& message) : std::logic_error{ message } {}
};

template <class T>
union storage_t
{
    unsigned char dummy_;
    T value_;

    storage_t(trivial_init_t) noexcept : dummy_() {};

    template <class... Args>
    storage_t( Args&&... args ) : value_(std::forward<Args>(args)...) {}

    ~storage_t(){}
};

template <class T>
struct optional_base
{
    bool init_;
    storage_t<T> storage_;

    optional_base() noexcept : init_(false), storage_(trivial_init) {};

    explicit optional_base(only_set_initialized_t, bool init) noexcept : init_(init), storage_(trivial_init) {};

    explicit optional_base(const T& v) : init_(true), storage_(v) {}

    explicit optional_base(T&& v) : init_(true), storage_(std::move(v)) {}

    template <class... Args> explicit optional_base(in_place_t, Args&&... args)
        :init_(true), storage_(std::forward<Args>(args)...) {}

    template <class U, class... Args, REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
    explicit optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
        :init_(true), storage_(il, std::forward<Args>(args)...) {}

    ~optional_base() { if (init_) storage_.value_.T::~T(); }
};

template <class T>
class optional : private optional_base<T>
{
    static_assert( !std::is_same<typename std::decay<T>::type, nothing_t>::value, "bad T" );
    static_assert( !std::is_same<typename std::decay<T>::type, in_place_t>::value, "bad T" );


    bool initialized() const noexcept { return optional_base<T>::init_; }
    T* dataptr() {  return std::addressof(optional_base<T>::storage_.value_); }
    const T* dataptr() const { return std::addressof(optional_base<T>::storage_.value_); }

    constexpr const T& contained_val() const { return optional_base<T>::storage_.value_; }
    T& contained_val() { return optional_base<T>::storage_.value_; }

    void clear() noexcept {
        if (initialized()) dataptr()->T::~T();
        optional_base<T>::init_ = false;
    }

    template <class... Args>
    void initialize(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
    {
        assert(!optional_base<T>::init_);
        new (dataptr()) T(std::forward<Args>(args)...);
        optional_base<T>::init_ = true;
    }

    template <class U, class... Args>
    void initialize(std::initializer_list<U> il, Args&&... args) noexcept(noexcept(T(il, std::forward<Args>(args)...)))
    {
        assert(!optional_base<T>::init_);
        new (dataptr()) T(il, std::forward<Args>(args)...);
        optional_base<T>::init_ = true;
    }

public:
    typedef T value_type;

    optional() noexcept : optional_base<T>()  {};
    optional(nothing_t) noexcept : optional_base<T>() {};

    optional(const optional& rhs):
        optional_base<T>(only_set_initialized, rhs.initialized())
    {
        if (rhs.initialized()) new (dataptr()) T(*rhs);
    }

    optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value):
        optional_base<T>(only_set_initialized, rhs.initialized())
    {
        if (rhs.initialized()) new (dataptr()) T(std::move(*rhs));
    }

    optional(const T& v) : optional_base<T>(v) {}

    optional(T&& v) : optional_base<T>(std::move(v)) {}

    template <class... Args>
    constexpr explicit optional(in_place_t, Args&&... args):
        optional_base<T>(in_place_t{}, std::forward<Args>(args)...) {}

    template <class U, class... Args, REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>

    optional(in_place_t, std::initializer_list<U> il, Args&&... args):
        optional_base<T>(in_place_t{}, il, std::forward<Args>(args)...) {}

    // 20.5.4.2 Destructor
    ~optional() = default;

    // 20.5.4.3, assignment
    optional& operator=(nothing_t) noexcept
    {
        clear();
        return *this;
    }

    optional& operator=(const optional& rhs)
    {
        if      (initialized() == true  && rhs.initialized() == false) clear();
        else if (initialized() == false && rhs.initialized() == true)  initialize(*rhs);
        else if (initialized() == true  && rhs.initialized() == true)  contained_val() = *rhs;
        return *this;
    }

    optional& operator=(optional&& rhs)
    {
        if      (initialized() == true  && rhs.initialized() == false) clear();
        else if (initialized() == false && rhs.initialized() == true)  initialize(std::move(*rhs));
        else if (initialized() == true  && rhs.initialized() == true)  contained_val() = std::move(*rhs);
        return *this;
    }

    template <class U>
    auto operator=(U&& v) -> typename std::enable_if<
                                 std::is_same<
                                     typename std::remove_reference<U>::type,
                                     T
                                 >::value,
                                 optional&
                             >::type
    {
        if (initialized()) { contained_val() = std::forward<U>(v); }
        else               { initialize(std::forward<U>(v));  }
        return *this;
    }


    template <class... Args>
    void emplace(Args&&... args)
    {
        clear();
        initialize(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    void emplace(std::initializer_list<U> il, Args&&... args)
    {
        clear();
        initialize<U, Args...>(il, std::forward<Args>(args)...);
    }

    // 20.5.4.4 Swap
    void swap(optional<T>& rhs)
    {
        if      (initialized() == true  && rhs.initialized() == false) { rhs.initialize(std::move(**this)); clear(); }
        else if (initialized() == false && rhs.initialized() == true)  { initialize(std::move(*rhs)); rhs.clear(); }
        else if (initialized() == true  && rhs.initialized() == true)  { using std::swap; swap(**this, *rhs); }
    }

    const T* operator-> () const {
        return ASSERTED_EXPRESSION(initialized(), dataptr());
    }

    T* operator-> () {
        assert (initialized());
        return dataptr();
    }

    const T& operator *() const {
        return ASSERTED_EXPRESSION(initialized(), contained_val());
    }

    T& operator* () {
        assert (initialized());
        return contained_val();
    }

    T const& value() const {
        return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
    }

    T& value() {
        return initialized() ? contained_val() : (throw bad_optional_access("bad optional access"), contained_val());
    }

    explicit operator bool() const noexcept { return initialized(); }

    template <class V>
    constexpr T value_or(V&& v) const
    {
        return *this ? **this : static_cast<T>(std::forward<V>(v));
    }
};

template <class T>
class optional<T&>
{
    static_assert( !std::is_same<T, nothing_t>::value, "bad T" );
    static_assert( !std::is_same<T, in_place_t>::value, "bad T" );
    T* ref;

public:

    optional() noexcept : ref(nullptr) {}

    optional(nothing_t) noexcept : ref(nullptr) {}

    optional(T& v) noexcept : ref(static_addressof(v)) {}

    optional(T&&) = delete;

    optional(const optional& rhs) noexcept : ref(rhs.ref) {}

    explicit constexpr optional(in_place_t, T& v) noexcept : ref(static_addressof(v)) {}

    explicit optional(in_place_t, T&&) = delete;

    ~optional() = default;

    optional& operator=(nothing_t) noexcept {
        ref = nullptr;
        return *this;
    }

    // optional& operator=(const optional& rhs) noexcept {
    // ref = rhs.ref;
    // return *this;
    // }

    // optional& operator=(optional&& rhs) noexcept {
    // ref = rhs.ref;
    // return *this;
    // }

    template <typename U>
    auto operator=(U&& rhs) noexcept -> typename std::enable_if<
                                            std::is_same<
                                                typename std::decay<U>::type,
                                                optional<T&>
                                            >::value,
                                            optional&
                                        >::type
    {
        ref = rhs.ref;
        return *this;
    }

    template <typename U>
    auto operator=(U&& rhs) noexcept -> typename std::enable_if<
                                            !std::is_same<
                                                typename std::decay<U>::type,
                                                optional<T&>
                                            >::value,
                                            optional&
                                        >::type = delete;

    void emplace(T& v) noexcept {
        ref = std::addressof(v);
    }

    void emplace(T&&) = delete;

    void swap(optional<T&>& rhs) noexcept
    {
        std::swap(ref, rhs.ref);
    }

    // 20.5.5.3, observers
    constexpr T* operator->() const {
        return ASSERTED_EXPRESSION(ref, ref);
    }

    constexpr T& operator*() const {
        return ASSERTED_EXPRESSION(ref, *ref);
    }

    constexpr T& value() const {
        return ref ? *ref : (throw bad_optional_access("bad optional access"), *ref);
    }

    explicit constexpr operator bool() const noexcept {
        return ref != nullptr;
    }

    template <class V>
    constexpr typename std::decay<T>::type value_or(V&& v) const
    {
        return *this ? **this : static_cast<typename std::decay<T>::type>(std::forward<V>(v));
    }
};

template <class T>
class optional<T&&>
{
    static_assert( sizeof(T) == 0, "optional rvalue referencs disallowed" );
};

template <class T>
bool operator==(const optional<T>& x, const optional<T>& y)
{
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

template <class T>
bool operator!=(const optional<T>& x, const optional<T>& y)
{
    return !(x == y);
}

template <class T>
bool operator<(const optional<T>& x, const optional<T>& y)
{
    return (!y) ? false : (!x) ? true : *x < *y;
}

template <class T>
bool operator>(const optional<T>& x, const optional<T>& y)
{
    return (y < x);
}

template <class T>
bool operator<=(const optional<T>& x, const optional<T>& y)
{
    return !(y < x);
}

template <class T>
bool operator>=(const optional<T>& x, const optional<T>& y)
{
    return !(x < y);
}

template <class T>
bool operator==(const optional<T>& x, nothing_t) noexcept
{
    return (!x);
}

template <class T>
bool operator==(nothing_t, const optional<T>& x) noexcept
{
    return (!x);
}

template <class T>
bool operator!=(const optional<T>& x, nothing_t) noexcept
{
    return bool(x);
}

template <class T>
bool operator!=(nothing_t, const optional<T>& x) noexcept
{
    return bool(x);
}

template <class T>
bool operator<(const optional<T>&, nothing_t) noexcept
{
    return false;
}

template <class T> bool operator<(nothing_t, const optional<T>& x) noexcept
{
    return bool(x);
}

template <class T> bool operator<=(const optional<T>& x, nothing_t) noexcept
{
    return (!x);
}

template <class T> bool operator<=(nothing_t, const optional<T>&) noexcept
{
    return true;
}

template <class T> bool operator>(const optional<T>& x, nothing_t) noexcept
{
    return bool(x);
}

template <class T> bool operator>(nothing_t, const optional<T>&) noexcept
{
    return false;
}

template <class T> bool operator>=(const optional<T>&, nothing_t) noexcept
{
    return true;
}

template <class T> bool operator>=(nothing_t, const optional<T>& x) noexcept
{
    return (!x);
}

template <class T> bool operator==(const optional<T>& x, const T& v)
{
    return bool(x) ? *x == v : false;
}

template <class T> bool operator==(const T& v, const optional<T>& x)
{
    return bool(x) ? v == *x : false;
}

template <class T> bool operator!=(const optional<T>& x, const T& v)
{
    return bool(x) ? *x != v : true;
}

template <class T> bool operator!=(const T& v, const optional<T>& x)
{
    return bool(x) ? v != *x : true;
}

template <class T> bool operator<(const optional<T>& x, const T& v)
{
    return bool(x) ? *x < v : true;
}

template <class T> bool operator>(const T& v, const optional<T>& x)
{
    return bool(x) ? v > *x : true;
}

template <class T> bool operator>(const optional<T>& x, const T& v)
{
    return bool(x) ? *x > v : false;
}

template <class T> bool operator<(const T& v, const optional<T>& x)
{
    return bool(x) ? v < *x : false;
}

template <class T> bool operator>=(const optional<T>& x, const T& v)
{
    return bool(x) ? *x >= v : false;
}

template <class T> bool operator<=(const T& v, const optional<T>& x)
{
    return bool(x) ? v <= *x : false;
}

template <class T> bool operator<=(const optional<T>& x, const T& v)
{
    return bool(x) ? *x <= v : true;
}

template <class T> bool operator>=(const T& v, const optional<T>& x)
{
    return bool(x) ? v >= *x : true;
}


// Comparison of optionsl<T&> with T
template <class T> bool operator==(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x == v : false;
}

template <class T> bool operator==(const T& v, const optional<T&>& x)
{
    return bool(x) ? v == *x : false;
}

template <class T> bool operator!=(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x != v : true;
}

template <class T> bool operator!=(const T& v, const optional<T&>& x)
{
    return bool(x) ? v != *x : true;
}

template <class T> bool operator<(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x < v : true;
}

template <class T> bool operator>(const T& v, const optional<T&>& x)
{
    return bool(x) ? v > *x : true;
}

template <class T> bool operator>(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x > v : false;
}

template <class T> bool operator<(const T& v, const optional<T&>& x)
{
    return bool(x) ? v < *x : false;
}

template <class T> bool operator>=(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x >= v : false;
}

template <class T> bool operator<=(const T& v, const optional<T&>& x)
{
    return bool(x) ? v <= *x : false;
}

template <class T> bool operator<=(const optional<T&>& x, const T& v)
{
    return bool(x) ? *x <= v : true;
}

template <class T> bool operator>=(const T& v, const optional<T&>& x)
{
    return bool(x) ? v >= *x : true;
}

template <class T> bool operator==(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x == v : false;
}

template <class T> bool operator==(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v == *x : false;
}

template <class T> bool operator!=(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x != v : true;
}

template <class T> bool operator!=(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v != *x : true;
}

template <class T> bool operator<(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x < v : true;
}

template <class T> bool operator>(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v > *x : true;
}

template <class T> bool operator>(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x > v : false;
}

template <class T> bool operator<(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v < *x : false;
}

template <class T> bool operator>=(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x >= v : false;
}

template <class T> bool operator<=(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v <= *x : false;
}

template <class T> bool operator<=(const optional<const T&>& x, const T& v)
{
    return bool(x) ? *x <= v : true;
}

template <class T> bool operator>=(const T& v, const optional<const T&>& x)
{
    return bool(x) ? v >= *x : true;
}

// 20.5.12 Specialized algorithms
template <class T>
void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
{
    x.swap(y);
}

template <class T>
optional<typename std::decay<T>::type> make_optional(T&& v)
{
    return optional<typename std::decay<T>::type>(std::forward<T>(v));
}

template <class X>
optional<X&> make_optional(std::reference_wrapper<X> v)
{
    return optional<X&>(v.get());
}

template<class X>
optional<X&> nothing() {
    return nothing();
}

} // namespace util
} // namespace callophrys

namespace std {

template <typename T>
struct hash<callophrys::util::optional<T>>
{
    typedef typename hash<T>::result_type result_type;
    typedef callophrys::util::optional<T> argument_type;

    result_type operator()(argument_type const& arg) const {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

template <typename T>
struct hash<callophrys::util::optional<T&>>
{
    typedef typename hash<T>::result_type result_type;
    typedef callophrys::util::optional<T&> argument_type;

    result_type operator()(argument_type const& arg) const {
        return arg ? std::hash<T>{}(*arg) : result_type{};
    }
};

} /* namespace std */


#endif //___OPTIONAL_HPP___
