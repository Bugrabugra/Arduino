// ArduinoJson - arduinojson.org
<<<<<<< HEAD
// Copyright Benoit Blanchon 2014-2019
=======
// Copyright Benoit Blanchon 2014-2020
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
// MIT License

#pragma once

#include <sstream>

class CustomReader {
  std::stringstream _stream;

 public:
  CustomReader(const char* input) : _stream(input) {}

  int read() {
    return _stream.get();
  }

  size_t readBytes(char* buffer, size_t length) {
    _stream.read(buffer, static_cast<std::streamsize>(length));
    return static_cast<size_t>(_stream.gcount());
  }

 private:
  CustomReader(const CustomReader&);
};
