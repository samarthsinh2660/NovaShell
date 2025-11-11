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
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

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
    src/ai/command_suggester.cpp
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
)

# Link libraries
target_link_libraries(customos-shell
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

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
message(STATUS "  Network Support: ${ENABLE_NETWORK}")
message(STATUS "  PCAP Available: ${HAVE_PCAP}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Plugins: ${BUILD_PLUGINS}")
