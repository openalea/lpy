# Anaconda Check
if (DEFINED ENV{CONDA_PREFIX})
    # Anaconda Environment
    message(STATUS "Anaconda environment detected.")
    
    if (WIN32)
        set(CONDA_ENV "$ENV{CONDA_PREFIX}/Library")
    else()
        set(CONDA_ENV "$ENV{CONDA_PREFIX}")
    endif()
endif()
