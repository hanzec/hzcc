find_package(glog CONFIG REQUIRED)

# vcpkg's Linux libunwind enables LZMA-compressed debug information, while glog's
# FindUnwind exports only libunwind itself. Attach its missing dependency to the
# imported unwind target so static links place liblzma after libunwind.
# Imported targets are directory-scoped, so each glog consumer includes this file.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND TARGET unwind::unwind)
    find_package(LibLZMA REQUIRED)
    get_target_property(hzcc_unwind_dependencies unwind::unwind INTERFACE_LINK_LIBRARIES)
    if(NOT "LibLZMA::LibLZMA" IN_LIST hzcc_unwind_dependencies)
        set_property(TARGET unwind::unwind APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES LibLZMA::LibLZMA)
    endif()
    unset(hzcc_unwind_dependencies)
endif()
