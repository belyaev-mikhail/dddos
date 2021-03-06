/*
 * json.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: belyaev
 */

#ifndef JSON_HPP_
#define JSON_HPP_

#include <jsoncpp/json/json.h>

#include <memory>
#include <string>

#include "Util/meta.hpp"

#include "Util/macros.h"

namespace callophrys {
namespace util {

////////////////////////////////////////////////////////////////////////////////

typedef Json::Value JsonValue;

template<class T, typename SFINAE = void>
struct json_traits;

template<>
struct json_traits<JsonValue> {
    typedef std::unique_ptr<JsonValue> optional_ptr_t;

    static JsonValue toJson(const JsonValue& val) {
        return val;
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return optional_ptr_t{ new JsonValue{ json } };
    }
};

template<>
struct json_traits<bool> {
    typedef std::unique_ptr<bool> optional_ptr_t;

    static JsonValue toJson(bool val) {
        return JsonValue(val);
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return json.isBool() ? optional_ptr_t{ new bool{json.asBool()} } :
                               nullptr;
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_floating_point<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static JsonValue toJson(T val) {
        return JsonValue(val);
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return json.isDouble() || json.isIntegral()
               ? optional_ptr_t{ new T{json.asDouble()} }
               : nullptr;
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_integral<T>::value && std::is_signed<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static JsonValue toJson(T val) {
        return JsonValue(static_cast<Json::Int>(val));
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return json.isIntegral() ? optional_ptr_t{ new T{json.asInt()} } :
                                   nullptr;
    }
};

template< class T >
struct json_traits<T, GUARD(std::is_integral<T>::value && std::is_unsigned<T>::value)> {
    typedef std::unique_ptr<T> optional_ptr_t;

    static JsonValue toJson(T val) {
        return JsonValue(static_cast<Json::UInt>(val));
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return json.isIntegral() ? optional_ptr_t{ new T{json.asUInt()} } :
                                   nullptr;
    }
};

template<>
struct json_traits<std::string> {
    typedef std::unique_ptr<std::string> optional_ptr_t;

    static JsonValue toJson(const std::string& val) {
        return JsonValue(val);
    }

    static optional_ptr_t fromJson(const JsonValue& json) {
        return json.isString()
               ? optional_ptr_t{ new std::string{json.asString()} }
               : nullptr;
    }
};

////////////////////////////////////////////////////////////////////////////////

template<class T>
JsonValue toJson(const T& val) {
    return json_traits<T>::toJson(val);
}

template<class T>
typename json_traits<T>::optional_ptr_t fromJson(const JsonValue& json) {
    return json_traits<T>::fromJson(json);
}

template<class T>
typename json_traits<T>::optional_ptr_t read_as_json(std::istream& ist) {
    Json::Reader reader;
    JsonValue val;
    reader.parse(ist, val, false);
    return json_traits<T>::fromJson(val);
}

template<class T>
void write_as_json(std::ostream& ost, const T& val) {
    Json::StyledStreamWriter writer;
    writer.write(ost, json_traits<T>::toJson(val));
}

////////////////////////////////////////////////////////////////////////////////

template<class T>
struct Jsoner {
    T* value;

    friend std::ostream& operator<< (std::ostream& ost, Jsoner<T> json) {
        write_as_json(ost, *json.value);
        return ost;
    }

    friend std::istream& operator>> (std::istream& ist, Jsoner<T> json) {
        auto read = read_as_json<T>(ist);
        if(read) *json.value = *read;
        else ist.clear(std::ios::failbit);
        return ist;
    }

};

template<class T>
struct const_Jsoner {
    const T* value;

    friend std::ostream& operator<< (std::ostream& ost, const_Jsoner<T> json) {
        write_as_json(ost, *json.value);
        return ost;
    }
};

template<class T>
Jsoner<T> jsonify(T& value) { return Jsoner<T>{ &value }; }
template<class T>
const_Jsoner<T> jsonify(const T& value) { return const_Jsoner<T>{ &value }; }

////////////////////////////////////////////////////////////////////////////////

namespace impl {
static bool allptrs() {
    return true;
}
template<class H, class ...T>
static bool allptrs(const H& h, const T&... t) {
    if(!h) return false;
    else return allptrs(t...);
}
} //namespace impl

template<class Obj, class ...Args>
struct json_object_builder {
    std::vector<std::string> keys;

    // this is a constructor that takes a number of std::strings
    // exactly the same as the number of types in Args
    json_object_builder(const typename type_K_comb<std::string, Args>::type&... keys) : keys { keys... } {}

    template<size_t ...Ixs>
    Obj* build_(const JsonValue& val, util::indexer<Ixs...>) const {
        if(!val.isObject()) return nullptr;

        std::tuple<typename json_traits<const typename util::index_in_row<Ixs, Args...>::type>::optional_ptr_t...> ptrs {
            util::fromJson<typename util::index_in_row<Ixs, Args...>::type>(val[keys[Ixs]])...
        };

        if(!impl::allptrs(std::get<Ixs>(ptrs)...)) return nullptr;

        return new Obj {
            *std::get<Ixs>(ptrs)...
        };
    }

    Obj* build(const JsonValue& val) const {
        return build_(val, typename util::make_indexer<Args...>::type());
    }
};

} // namespace util
} // namespace callophrys

#include "Util/unmacros.h"

#endif /* JSON_HPP_ */
