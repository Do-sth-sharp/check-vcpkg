# Minimum CMake required. If available, accept the policy-controlled behavior up
# to 3.26.
cmake_minimum_required(VERSION 3.10...3.26)

# Revert to old behavior for MSVC debug symbols.
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 OLD)
endif()

# Project
set(CHECK_VCPKG_INSTALL_NAME "check-vcpkg")
project(${CHECK_VCPKG_INSTALL_NAME} VERSION 1.0.0 LANGUAGES C CXX)

# Configs
include(CheckCXXSourceCompiles)
set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} -std=c++14)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Check Atomic
check_cxx_source_compiles("
	#include <atomic>
	int main() {
	return static_cast<int>(std::atomic<int64_t>{});
	}
" CHECK_HAVE_BUILTIN_ATOMICS)
message(STATUS "CHECK_HAVE_BUILTIN_ATOMICS: ${CHECK_HAVE_BUILTIN_ATOMICS}")
add_executable(check-atomic "${SOURCE_DIR}/check-atomic.cpp")

# CTest
enable_testing ()
add_test (NAME "Check-Atomic" COMMAND $<TARGET_FILE:check-atomic>)

# Install
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set (CHECK_VCPKG_CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${CHECK_VCPKG_INSTALL_NAME})

if (CHECK_VCPKG_VCPKG_TOOLS_HINT)
	set(TOOLS_DIR tools/${CHECK_VCPKG_INSTALL_NAME})
else ()
	set(TOOLS_DIR ${CMAKE_INSTALL_BINDIR})
endif ()
install (TARGETS check-atomic
	EXPORT ${CHECK_VCPKG_INSTALL_NAME}Targets
	RUNTIME DESTINATION "${TOOLS_DIR}" OPTIONAL
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
)

if (MSVC)
	install (FILES $<TARGET_PDB_FILE:check-atomic> DESTINATION "${TOOLS_DIR}" OPTIONAL)
endif (MSVC)

foreach (ITEM ${SRC_INC})
	file (GLOB_RECURSE HDR_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${ITEM}/*.h")

	foreach (HDR_FILE ${HDR_FILES})
		get_filename_component (HDR_DIR ${HDR_FILE} DIRECTORY)
		install (FILES "${CMAKE_CURRENT_SOURCE_DIR}/${HDR_FILE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CHECK_VCPKG_INSTALL_NAME}/${HDR_DIR}" OPTIONAL)
	endforeach (HDR_FILE ${HDR_FILES})
endforeach (ITEM ${SRC_INC})

write_basic_package_version_file (
	"${CMAKE_CURRENT_BINARY_DIR}/${CHECK_VCPKG_INSTALL_NAME}-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

configure_package_config_file (
	"${CMAKE_CURRENT_LIST_DIR}/${CHECK_VCPKG_INSTALL_NAME}-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/${CHECK_VCPKG_INSTALL_NAME}-config.cmake"
	INSTALL_DESTINATION ${CHECK_VCPKG_CMAKE_CONFIG_INSTALL_DIR}
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install (FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${CHECK_VCPKG_INSTALL_NAME}-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${CHECK_VCPKG_INSTALL_NAME}-config-version.cmake"
	DESTINATION ${CHECK_VCPKG_CMAKE_CONFIG_INSTALL_DIR}
)

install (EXPORT ${CHECK_VCPKG_INSTALL_NAME}Targets
	FILE "${CHECK_VCPKG_INSTALL_NAME}-targets.cmake"
	NAMESPACE ${CHECK_VCPKG_INSTALL_NAME}::
	DESTINATION ${CHECK_VCPKG_CMAKE_CONFIG_INSTALL_DIR}
)
