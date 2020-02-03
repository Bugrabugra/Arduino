// ArduinoJson - arduinojson.org
<<<<<<< HEAD
// Copyright Benoit Blanchon 2014-2019
=======
// Copyright Benoit Blanchon 2014-2020
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
// MIT License

#pragma once

namespace ARDUINOJSON_NAMESPACE {

template <typename TDestination>
class Writer<
    TDestination,
    typename enable_if<is_base_of< ::Print, TDestination>::value>::type> {
 public:
  explicit Writer(::Print& print) : _print(&print) {}

  size_t write(uint8_t c) {
    return _print->write(c);
  }

  size_t write(const uint8_t* s, size_t n) {
    return _print->write(s, n);
  }

 private:
  ::Print* _print;
};

}  // namespace ARDUINOJSON_NAMESPACE
