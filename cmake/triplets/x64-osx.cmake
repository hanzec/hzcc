set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES x86_64)

# Abseil's sanitizer-dependent container layout must match HZCC Debug objects.
set(VCPKG_C_FLAGS_DEBUG "-fsanitize=address -fno-omit-frame-pointer")
set(VCPKG_CXX_FLAGS_DEBUG "-fsanitize=address -fno-omit-frame-pointer")
set(VCPKG_LINKER_FLAGS_DEBUG "-fsanitize=address")
