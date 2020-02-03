// ArduinoJson - arduinojson.org
<<<<<<< HEAD
// Copyright Benoit Blanchon 2014-2019
=======
// Copyright Benoit Blanchon 2014-2020
>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
// MIT License

#pragma once

// Reproduces Arduino's Stream class
class Stream  // : public Print
{
 public:
  virtual ~Stream() {}
  virtual int read() = 0;
  virtual size_t readBytes(char *buffer, size_t length) = 0;
};
