# Build
set(shapeworks_sources
  shapeworks.cpp
  Executable.cpp
  Command.cpp
  Commands.cpp
  ParticleCommands.cpp
  )
set(shapeworks_headers
  Executable.h
  Command.h
  Commands.h
  ParticleCommands.h
  SharedCommandData.h
  )
add_executable(shapeworks
  ${shapeworks_sources}
  ${shapeworks_headers})
target_include_directories(shapeworks PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(shapeworks
  Utils
  Image
  Mesh
  Analyze
  Particles
  ${ITK_LIBRARIES}
  )

# Install
set_target_properties(shapeworks PROPERTIES PUBLIC_HEADER
  "${shapeworks_headers}")
install(TARGETS shapeworks
  ARCHIVE DESTINATION bin
  RUNTIME DESTINATION bin
  PUBLIC_HEADER DESTINATION include
  )
