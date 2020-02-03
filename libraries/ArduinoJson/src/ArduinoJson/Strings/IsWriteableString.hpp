// ArduinoJson - arduinojson.org
<<<<<<< HEAD
// Copyright Benoit Blanchon 2014-2019
=======
// Copyright Benoit Blanchon 2014-2020
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
// MIT License

#pragma once

#include <ArduinoJson/Configuration.hpp>
#include <ArduinoJson/Polyfills/type_traits.hpp>

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#include <WString.h>
#endif

#if ARDUINOJSON_ENABLE_STD_STRING
#include <string>
#endif

namespace ARDUINOJSON_NAMESPACE {

template <typename>
struct IsWriteableString : false_type {};

#if ARDUINOJSON_ENABLE_ARDUINO_STRING

template <>
struct IsWriteableString< ::String> : true_type {};

#endif

#if ARDUINOJSON_ENABLE_STD_STRING

template <typename TCharTraits, typename TAllocator>
struct IsWriteableString<std::basic_string<char, TCharTraits, TAllocator> >
    : true_type {};

#endif
}  // namespace ARDUINOJSON_NAMESPACE
