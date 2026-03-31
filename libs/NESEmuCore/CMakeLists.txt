
file(GLOB headers CONFIGURE_DEPENDS include/NESEmuCore/*.hpp)
file(GLOB sources CONFIGURE_DEPENDS src/*.cpp)

add_library(NESEmuCore)
target_sources(NESEmuCore
    PRIVATE
        ${sources}

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            ${headers}
)
