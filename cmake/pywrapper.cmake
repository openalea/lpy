

function(pgllib_link_python libwrapname)
    if(NOT APPLE OR NOT USE_CONDA)
        if (Python3_FOUND)
             target_link_libraries(${libwrapname} Python3::Python)
        elseif (Python2_FOUND)
            target_link_libraries(${libwrapname} Python2::Python)
        endif()
    else()
        message(STATUS "Do not link with Python directly : " ${libwrapname})
    endif()

    # Disable Boost Auto-Link
    #target_compile_definitions(${libwrapname} PRIVATE BOOST_ALL_NO_LIB)

    #target_link_libraries(${libwrapname} Boost::system Boost::thread Boost::python )

endfunction()



function(pglwrapper_install libwrapname)
    set_target_properties(${libwrapname} PROPERTIES PREFIX "")

    if (WIN32)
        set_target_properties(${libwrapname} PROPERTIES SUFFIX ".pyd")
    endif()

    if (APPLE)
        set_target_properties(${libwrapname} PROPERTIES SUFFIX ".so")
    endif()

    install(TARGETS ${libwrapname} DESTINATION "${CMAKE_SOURCE_DIR}/src/openalea/lpy")
endfunction()
