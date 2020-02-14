// Copyright 2019 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_CONFIG_JSONCONS_CONFIG_HPP
#define JSONCONS_CONFIG_JSONCONS_CONFIG_HPP

#include <jsoncons/config/compiler_support.hpp>
#include <jsoncons/config/binary_config.hpp>

#if !defined(JSONCONS_HAS_STD_STRING_VIEW)
#include <jsoncons/detail/string_view.hpp>
namespace jsoncons {
using jsoncons::detail::basic_string_view;
using string_view = basic_string_view<char, std::char_traits<char>>;
using wstring_view = basic_string_view<wchar_t, std::char_traits<wchar_t>>;
}
#else 
#include <string_view>
namespace jsoncons {
using std::basic_string_view;
using std::string_view;
using std::wstring_view;
}
#endif

#if !defined(JSONCONS_HAS_SPAN)
#include <jsoncons/detail/span.hpp>
namespace jsoncons {
using jsoncons::detail::span;
}
#else 
#include <span>
namespace jsoncons {
using std::span;
}
#endif

#if !defined(JSONCONS_HAS_OPTIONAL)
#include <jsoncons/detail/optional.hpp>
namespace jsoncons {
using jsoncons::detail::optional;
}
#else 
#include <optional>
namespace jsoncons {
using std::optional;
}
#endif // !defined(JSONCONS_HAS_OPTIONAL)

#if !defined(JSONCONS_HAS_ENDIAN)
#include <jsoncons/detail/endian.hpp>
namespace jsoncons {
using jsoncons::detail::endian;
}
#else
#include <bit>
namespace jsoncons 
{
    using std::endian;
}
#endif

#if !defined(JSONCONS_HAS_MAKE_UNIQUE)

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace jsoncons {

    template<class T> 
    struct unique_if 
    {
        typedef std::unique_ptr<T> value_is_not_array;
    };

    template<class T> 
    struct unique_if<T[]> 
    {
        typedef std::unique_ptr<T[]> value_is_array_of_unknown_bound;
    };

    template<class T, size_t N> 
    struct unique_if<T[N]> {
        typedef void value_is_array_of_known_bound;
    };

    template<class T, class... Args>
    typename unique_if<T>::value_is_not_array
    make_unique(Args&&... args) 
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    typename unique_if<T>::value_is_array_of_unknown_bound
    make_unique(size_t n) 
    {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]());
    }

    template<class T, class... Args>
    typename unique_if<T>::value_is_array_of_known_bound
    make_unique(Args&&...) = delete;
} // jsoncons

#else

#include <memory>
namespace jsoncons 
{
    using std::make_unique;
}

#endif // !defined(JSONCONS_HAS_MAKE_UNIQUE)

namespace jsoncons {
namespace detail {

    // native_to_big

    template<typename T, class OutputIt, class Endian=endian>
    typename std::enable_if<Endian::native == Endian::big,void>::type
    native_to_big(T val, OutputIt d_first)
    {
        uint8_t buf[sizeof(T)];
        std::memcpy(buf, &val, sizeof(T));
        for (auto item : buf)
        {
            *d_first++ = item;
        }
    }

    template<typename T, class OutputIt, class Endian=endian>
    typename std::enable_if<Endian::native == Endian::little,void>::type
    native_to_big(T val, OutputIt d_first)
    {
        T val2 = byte_swap(val);
        uint8_t buf[sizeof(T)];
        std::memcpy(buf, &val2, sizeof(T));
        for (auto item : buf)
        {
            *d_first++ = item;
        }
    }

    // native_to_little

    template<typename T, class OutputIt, class Endian = endian>
    typename std::enable_if<Endian::native == Endian::little,void>::type
    native_to_little(T val, OutputIt d_first)
    {
        uint8_t buf[sizeof(T)];
        std::memcpy(buf, &val, sizeof(T));
        for (auto item : buf)
        {
            *d_first++ = item;
        }
    }

    template<typename T, class OutputIt, class Endian=endian>
    typename std::enable_if<Endian::native == Endian::big, void>::type
    native_to_little(T val, OutputIt d_first)
    {
        T val2 = byte_swap(val);
        uint8_t buf[sizeof(T)];
        std::memcpy(buf, &val2, sizeof(T));
        for (auto item : buf)
        {
            *d_first++ = item;
        }
    }

    // big_to_native

    template<class T,class Endian=endian>
    typename std::enable_if<Endian::native == Endian::big,T>::type
    big_to_native(const uint8_t* first, const uint8_t* last, const uint8_t** endp)
    {
        if (first + sizeof(T) > last)
        {
            *endp = first;
            return 0;
        }
        *endp = first + sizeof(T);
        T val;
        std::memcpy(&val,first,sizeof(T));
        return val;
    }

    template<class T,class Endian=endian>
    typename std::enable_if<Endian::native == Endian::little,T>::type
    big_to_native(const uint8_t* first, const uint8_t* last, const uint8_t** endp)
    {
        if (first + sizeof(T) > last)
        {
            *endp = first;
            return 0;
        }
        *endp = first + sizeof(T);
        T val;
        std::memcpy(&val,first,sizeof(T));
        return byte_swap(val);
    }

    // little_to_native

    template<class T,class Endian=endian>
    typename std::enable_if<Endian::native == Endian::little,T>::type
    little_to_native(const uint8_t* first, const uint8_t* last, const uint8_t** endp)
    {
        if (first + sizeof(T) > last)
        {
            *endp = first;
            return 0;
        }
        *endp = first + sizeof(T);
        T val;
        std::memcpy(&val,first,sizeof(T));
        return val;
    }

    template<class T,class Endian=endian>
    typename std::enable_if<Endian::native == Endian::big,T>::type
    little_to_native(const uint8_t* first, const uint8_t* last, const uint8_t** endp)
    {
        if (first + sizeof(T) > last)
        {
            *endp = first;
            return 0;
        }
        *endp = first + sizeof(T);
        T val;
        std::memcpy(&val,first,sizeof(T));
        return byte_swap(val);
    }

} // detail
} // jsoncons

#define JSONCONS_EXPAND(X) X    
#define JSONCONS_QUOTE(Prefix, A) JSONCONS_EXPAND(Prefix ## #A)

#define JSONCONS_STRING_LITERAL(name, ...) \
   template <class CharT> \
   const jsoncons::basic_string_view<CharT>& name() {\
       static constexpr CharT s[] = { __VA_ARGS__};\
       static constexpr jsoncons::basic_string_view<CharT> sv(s, sizeof(s) / sizeof(CharT));\
       return sv;\
   }

#define JSONCONS_ARRAY_OF_CHAR(CharT, name, ...) \
    static constexpr CharT name[] = { __VA_ARGS__,0};

#define JSONCONS_DEFINE_LITERAL( name ) \
template<class CharT> CharT const* name##_literal(); \
template<> inline char const * name##_literal<char>() { return JSONCONS_QUOTE(,name); } \
template<> inline wchar_t const* name##_literal<wchar_t>() { return JSONCONS_QUOTE(L,name); } \
template<> inline char16_t const* name##_literal<char16_t>() { return JSONCONS_QUOTE(u,name); } \
template<> inline char32_t const* name##_literal<char32_t>() { return JSONCONS_QUOTE(U,name); }

#endif // JSONCONS_CONFIG_JSONCONS_CONFIG_HPP


