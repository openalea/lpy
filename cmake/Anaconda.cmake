# Anaconda Check
if (DEFINED ENV{CONDA_PREFIX})
    # Anaconda Environment
    message(STATUS "Anaconda environment detected.")
    
    set(CONDA_ENV "$ENV{CONDA_PREFIX}/Library")
    set(CONDA_PYTHON_ENV "$ENV{CONDA_PREFIX}")
endif()
