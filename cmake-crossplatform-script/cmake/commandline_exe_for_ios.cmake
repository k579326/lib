
function(add_executable exename)
    if (IOS)
        _add_executable(${exename} MACOSX_BUNDLE ${ARGN})
        set_target_properties(${exename} PROPERTIES
                                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/service/ios/plist.in
                            )
        #set_target_properties(${exename} PROPERTIES MACOSX_FRAMEWORK_IDENTIFIER  "com.virbox.doe.test")
    else ()
        _add_executable(${exename} ${ARGN})
    endif (IOS)
endfunction()