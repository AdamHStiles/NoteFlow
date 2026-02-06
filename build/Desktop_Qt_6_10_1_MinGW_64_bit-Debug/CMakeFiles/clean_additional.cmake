# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\NoteFlow_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\NoteFlow_autogen.dir\\ParseCache.txt"
  "NoteFlow_autogen"
  )
endif()
