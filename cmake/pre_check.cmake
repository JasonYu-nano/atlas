function(REQUIRE_JAVA)
    unset(Java_JAVA_EXECUTABLE CACHE)
    find_package(Java 21 REQUIRED)

    message(STATUS "Java version: ${Java_VERSION}")

    if(NOT ${Java_FOUND})
        message(FATAL_ERROR "JAVA_HOME is not set and no 'java' command could be found.")
    endif()
endfunction()

macro(DO_CHECK)
    REQUIRE_JAVA()
endmacro()