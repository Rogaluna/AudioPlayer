# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AudioPlayer_autogen"
  "CMakeFiles\\AudioPlayer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AudioPlayer_autogen.dir\\ParseCache.txt"
  "thirdparty\\QHotkey\\CMakeFiles\\QHotkey_autogen.dir\\AutogenUsed.txt"
  "thirdparty\\QHotkey\\CMakeFiles\\QHotkey_autogen.dir\\ParseCache.txt"
  "thirdparty\\QHotkey\\QHotkey_autogen"
  )
endif()
