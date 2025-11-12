cmake_minimum_required(VERSION 3.15)
project(CustomOSShell VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_PLUGINS "Build sample plugins" ON)
option(ENABLE_NETWORK "Enable network packet analyzer features" ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find dependencies
find_package(OpenSSL)

if(NOT OpenSSL_FOUND)
    # Try manual detection on Windows with explicit paths
    if(WIN32)
        set(OPENSSL_POSSIBLE_PATHS
            "C:/Program Files/OpenSSL"
            "C:/Program Files/OpenSSL-Win64"
            "C:/OpenSSL"
            "C:/OpenSSL-Win64"
            "$ENV{ProgramFiles}/OpenSSL"
            "$ENV{ProgramFiles}/OpenSSL-Win64"
            "$ENV{ProgramW6432}/OpenSSL"
        )
        
        foreach(OPENSSL_PATH ${OPENSSL_POSSIBLE_PATHS})
            if(EXISTS "${OPENSSL_PATH}/include/openssl/ssl.h")
                set(OPENSSL_ROOT_DIR "${OPENSSL_PATH}")
                set(OPENSSL_INCLUDE_DIR "${OPENSSL_PATH}/include")
                
                # Try different library paths
                set(OPENSSL_LIB_PATHS
                    "${OPENSSL_PATH}/lib/VC/x64/MD"
                    "${OPENSSL_PATH}/lib/VC/x64/MT"
                    "${OPENSSL_PATH}/lib"
                    "${OPENSSL_PATH}/lib64"
                )
                
                foreach(LIB_PATH ${OPENSSL_LIB_PATHS})
                    if(EXISTS "${LIB_PATH}/libcrypto.lib" AND EXISTS "${LIB_PATH}/libssl.lib")
                        set(OPENSSL_CRYPTO_LIBRARY "${LIB_PATH}/libcrypto.lib")
                        set(OPENSSL_SSL_LIBRARY "${LIB_PATH}/libssl.lib")
                        set(OPENSSL_LIBRARIES "${OPENSSL_SSL_LIBRARY};${OPENSSL_CRYPTO_LIBRARY}")
                        break()
                    endif()
                endforeach()
                
                if(OPENSSL_CRYPTO_LIBRARY AND OPENSSL_SSL_LIBRARY)
                    set(OpenSSL_FOUND TRUE)
                    set(OPENSSL_VERSION "3.0.0")
                    message(STATUS "Found OpenSSL manually at: ${OPENSSL_PATH}")
                    message(STATUS "  Libraries: ${OPENSSL_CRYPTO_LIBRARY}, ${OPENSSL_SSL_LIBRARY}")
                    break()
                endif()
            endif()
        endforeach()
    endif()
endif()

if(OpenSSL_FOUND)
    set(HAVE_OPENSSL TRUE)
    message(STATUS "OpenSSL support: ENABLED")
else()
    set(HAVE_OPENSSL FALSE)
    message(WARNING "OpenSSL not found. Cryptographic features will be disabled.")
    message(WARNING "Install OpenSSL: Run .\\setup-dependencies.ps1")
endif()

find_package(Threads REQUIRED)

# Find SQLite3 (required for internal database)
find_library(SQLITE3_LIBRARY 
    NAMES sqlite3 libsqlite3 sqlite3.lib
    PATHS
        "C:/Program Files/SQLite"
        "C:/SQLite"
        "C:/MinGW/lib"
        "C:/MinGW-w64/lib"
        "$ENV{ProgramFiles}/SQLite"
)

if(NOT SQLITE3_LIBRARY)
    # Try to find in PATH directories
    find_file(SQLITE3_LIB_FILE 
        NAMES sqlite3.lib libsqlite3.a sqlite3.dll
        PATHS $ENV{PATH}
    )
    if(SQLITE3_LIB_FILE)
        get_filename_component(SQLITE3_LIB_DIR ${SQLITE3_LIB_FILE} DIRECTORY)
        find_library(SQLITE3_LIBRARY 
            NAMES sqlite3 libsqlite3
            PATHS ${SQLITE3_LIB_DIR}
            NO_DEFAULT_PATH
        )
    endif()
endif()

if(NOT SQLITE3_LIBRARY)
    message(STATUS "SQLite3 not found. Internal database features will be limited.")
    set(HAVE_SQLITE3 FALSE)
else()
    set(HAVE_SQLITE3 TRUE)
    message(STATUS "SQLite3 found: ${SQLITE3_LIBRARY}")
endif()

if(ENABLE_NETWORK AND UNIX)
    find_library(PCAP_LIBRARY pcap)
    if(PCAP_LIBRARY)
        set(HAVE_PCAP TRUE)
    endif()
endif()

# Source files organized by module
set(CORE_SOURCES
    src/core/shell.cpp
    src/core/command_processor.cpp
    src/core/command_registry.cpp
    src/core/tab_completion.cpp
)

set(VFS_SOURCES
    src/vfs/virtual_filesystem.cpp
    src/vfs/mount_manager.cpp
    src/vfs/file_recovery.cpp
)

set(AUTH_SOURCES
    src/auth/user_manager.cpp
    src/auth/authentication.cpp
    src/auth/access_control.cpp
)

set(NETWORK_SOURCES
    src/network/packet_analyzer.cpp
    src/network/packet_capture.cpp
    src/network/protocol_parser.cpp
)

set(VAULT_SOURCES
    src/vault/password_manager.cpp
    src/vault/encryption.cpp
    src/vault/vault_storage.cpp
)

set(SCRIPTING_SOURCES
    src/scripting/script_engine.cpp
    src/scripting/script_parser.cpp
    src/scripting/macro_manager.cpp
)

set(PLUGIN_SOURCES
    src/plugins/plugin_manager.cpp
    src/plugins/plugin_loader.cpp
)

set(LOGGING_SOURCES
    src/logging/logger.cpp
    src/logging/audit_trail.cpp
    src/logging/log_manager.cpp
)

set(UTILS_SOURCES
    src/utils/string_utils.cpp
    src/utils/file_utils.cpp
    src/utils/crypto_utils.cpp
)

set(AI_SOURCES
    # Disabled - requires libcurl which is not installed
    # src/ai/command_suggester.cpp
)

set(REMOTE_SOURCES
    src/remote/ssh_server.cpp
)

set(CONTAINERS_SOURCES
    src/containers/container_manager.cpp
)

set(NOTES_SOURCES
    src/notes/snippet_manager.cpp
)

set(MONITOR_SOURCES
    src/monitor/system_monitor.cpp
)

set(ENV_SOURCES
    src/env/environment_manager.cpp
)

set(P2P_SOURCES
    src/p2p/file_sharing.cpp
)

set(SCHEDULER_SOURCES
    src/scheduler/task_scheduler.cpp
)

set(DATABASE_SOURCES
    # Disabled - requires SQLite3 which is not installed
    # src/database/internal_db.cpp
    # src/database/db_manager.cpp
)

set(GIT_SOURCES
    src/git/git_manager.cpp
)

# Main executable
add_executable(customos-shell
    src/main.cpp
    ${CORE_SOURCES}
    ${VFS_SOURCES}
    ${AUTH_SOURCES}
    ${NETWORK_SOURCES}
    ${VAULT_SOURCES}
    ${SCRIPTING_SOURCES}
    ${PLUGIN_SOURCES}
    ${LOGGING_SOURCES}
    ${UTILS_SOURCES}
    ${AI_SOURCES}
    ${REMOTE_SOURCES}
    ${CONTAINERS_SOURCES}
    ${NOTES_SOURCES}
    ${MONITOR_SOURCES}
    ${ENV_SOURCES}
    ${P2P_SOURCES}
    ${SCHEDULER_SOURCES}
    ${DATABASE_SOURCES}
    ${GIT_SOURCES}
)

# Link libraries
target_link_libraries(customos-shell
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

if(HAVE_OPENSSL)
    target_link_libraries(customos-shell
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
    )
    target_include_directories(customos-shell PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_compile_definitions(customos-shell PRIVATE HAVE_OPENSSL)
endif()

if(HAVE_SQLITE3)
    target_link_libraries(customos-shell ${SQLITE3_LIBRARY})
    target_compile_definitions(customos-shell PRIVATE HAVE_SQLITE3)
endif()

if(HAVE_PCAP)
    target_link_libraries(customos-shell ${PCAP_LIBRARY})
    target_compile_definitions(customos-shell PRIVATE HAVE_PCAP)
endif()

if(WIN32)
    target_link_libraries(customos-shell ws2_32)
endif()

# Platform-specific compiler flags
if(MSVC)
    target_compile_options(customos-shell PRIVATE /W4 /WX-)
else()
    target_compile_options(customos-shell PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install targets
install(TARGETS customos-shell
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/customos
    FILES_MATCHING PATTERN "*.h"
)

# Sample plugins
if(BUILD_PLUGINS)
    add_subdirectory(plugins)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
add_custom_target(docs
    COMMAND echo "Documentation generation not yet implemented"
    COMMENT "Generating documentation"
)

# Print configuration summary
message(STATUS "CustomOS Shell Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenSSL Support: ${HAVE_OPENSSL}")
message(STATUS "  SQLite3 Support: ${HAVE_SQLITE3}")
message(STATUS "  Network Support: ${ENABLE_NETWORK}")
message(STATUS "  PCAP Available: ${HAVE_PCAP}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Plugins: ${BUILD_PLUGINS}")
