# ArduinoJson - arduinojson.org
# Copyright Benoit Blanchon 2014-2020
# MIT License

add_executable(TextFormatterTests 
	writeFloat.cpp
	writeString.cpp
)

target_link_libraries(TextFormatterTests catch)
<<<<<<< HEAD
=======
set_target_properties(TextFormatterTests PROPERTIES UNITY_BUILD OFF)

>>>>>>> 83396e542db2e19daf6539085d3dc42151f34328
add_test(TextFormatter TextFormatterTests)
