# exprtk
# Exports exprtk target
message(STATUS "Fetching exprtk")
FetchContent_Declare(exprtk
        GIT_REPOSITORY https://github.com/ArashPartow/exprtk
        GIT_TAG master # Repository doesn't provide any tags, commit hashes appear to change semi-regularly
        )
if (NOT exprtk_POPULATE)
    FetchContent_Populate(exprtk)

    add_library(exprtk INTERFACE)
    target_sources(exprtk INTERFACE
            ${exprtk_SOURCE_DIR}/exprtk.hpp
            )
    target_include_directories(exprtk INTERFACE ${exprtk_SOURCE_DIR})
endif ()
