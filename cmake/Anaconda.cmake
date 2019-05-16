# Anaconda Check
if (DEFINED ENV{CONDA_PREFIX})
    # Anaconda Environment
    message(STATUS "Anaconda environment detected.")
    
    file(TO_CMAKE_PATH $ENV{CONDA_PREFIX} TMP_CONDA_ENV)
    
    if (WIN32)
        set(CONDA_ENV "${TMP_CONDA_ENV}/Library/")
    else()
        set(CONDA_ENV "${TMP_CONDA_ENV}/")
    endif()

    set(CONDA_PYTHON_ENV "${TMP_CONDA_ENV}/")
endif()
