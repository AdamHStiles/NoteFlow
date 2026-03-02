# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ProjectLaunchProject_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ProjectLaunchProject_autogen.dir\\ParseCache.txt"
  "ProjectLaunchProject_autogen"
  )
endif()
