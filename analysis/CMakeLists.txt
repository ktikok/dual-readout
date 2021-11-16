project(analysis)

file(GLOB sources
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB headers
  ${PROJECT_SOURCE_DIR}/include/*.h
)

add_executable(analysis analysis.cpp)
add_executable(DRvis vis.cpp)
add_library(DRanalysis SHARED ${sources} ${headers})

target_link_libraries(
  DRanalysis
  ${ROOT_LIBRARIES}
  podio::podioRootIO
  edm4dr
  edm4dr::edm4drDict
  DRsegmentation
)

target_include_directories(DRanalysis PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(analysis DRanalysis)
target_link_libraries(DRvis DRanalysis)

set_target_properties(DRanalysis PROPERTIES PUBLIC_HEADER "${headers}")

install(TARGETS DRanalysis EXPORT analysisTargets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(TARGETS analysis EXPORT analysisTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
)

install(TARGETS DRvis EXPORT analysisTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT bin
)
