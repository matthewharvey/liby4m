CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(y4m C)
ADD_LIBRARY(y4m SHARED liby4m.c parse.c)

CONFIGURE_FILE(liby4m.pc.in ${CMAKE_BINARY_DIR}/liby4m.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/liby4m.pc DESTINATION "lib/pkgconfig")

INSTALL(TARGETS y4m DESTINATION lib)
