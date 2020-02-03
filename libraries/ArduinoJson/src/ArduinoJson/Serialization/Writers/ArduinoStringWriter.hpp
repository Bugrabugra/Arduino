// ArduinoJson - arduinojson.org
<<<<<<< HEAD
// Copyright Benoit Blanchon 2014-2019
=======
// Copyright Benoit Blanchon 2014-2020
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
// MIT License

#pragma once

#include <WString.h>

namespace ARDUINOJSON_NAMESPACE {

template <>
class Writer< ::String, void> {
<<<<<<< HEAD
 public:
  explicit Writer(::String &str) : _str(&str) {}

  size_t write(uint8_t c) {
    _str->operator+=(static_cast<char>(c));
=======
  static const size_t bufferCapacity = ARDUINOJSON_STRING_BUFFER_SIZE;

 public:
  explicit Writer(::String &str) : _destination(&str) {
    _size = 0;
  }

  ~Writer() {
    flush();
  }

  size_t write(uint8_t c) {
    ARDUINOJSON_ASSERT(_size < bufferCapacity);
    _buffer[_size++] = static_cast<char>(c);
    if (_size + 1 >= bufferCapacity) flush();
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
    return 1;
  }

  size_t write(const uint8_t *s, size_t n) {
<<<<<<< HEAD
    // CAUTION: Arduino String doesn't have append()
    // and old version doesn't have size() either
    _str->reserve(_str->length() + n);
    while (n > 0) {
      _str->operator+=(static_cast<char>(*s++));
      n--;
=======
    for (size_t i = 0; i < n; i++) {
      write(s[i]);
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
    }
    return n;
  }

 private:
<<<<<<< HEAD
  ::String *_str;
=======
  void flush() {
    ARDUINOJSON_ASSERT(_size < bufferCapacity);
    _buffer[_size] = 0;
    *_destination += _buffer;
    _size = 0;
  }

  ::String *_destination;
  char _buffer[bufferCapacity];
  size_t _size;
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
};

}  // namespace ARDUINOJSON_NAMESPACE
