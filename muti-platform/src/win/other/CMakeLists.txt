add_library(other other.c)
target_link_libraries(other IPHLPAPI)
set_target_properties(other PROPERTIES FOLDER "multi-platform")
