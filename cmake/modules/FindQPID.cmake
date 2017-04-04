SET(QPID_INCLUDE_DIRS
    $ENV{QPID_ROOT}/include
)

FIND_LIBRARY(QPID_MESSAGING
    qpidmessaging
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_BROKER
    qpidbroker
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_COMMON
    qpidcommon
    PATHS
    $ENV{QPID_ROOT}/lib/
)

FIND_LIBRARY(QPID_TYPES
    qpidtypes
    PATHS
    $ENV{QPID_ROOT}/lib/
)

if(${QPID_COMMON} AND ${QPID_TYPES} AND ${QPID_BROKER} AND ${QPID_MESSAGING})
    set(QPID_FOUND TRUE)
    message(STATUS "Found QPID libraries: ${QPID_COMMON}")
endif()

SET(QPID_LIBRARIES 
    ${QPID_COMMON}
    ${QPID_TYPES}
    ${QPID_BROKER}
    ${QPID_MESSAGING}
    dl
    pthread
)