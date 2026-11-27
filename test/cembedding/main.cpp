#include "lsystem.h"
#include "lpyrun.h"

#include <boost/python/errors.hpp>
#include <cstdio>
#include <cstdlib>

LPY_USING_NAMESPACE

int main(int argc, char* argv[])
{
    const bool verbose = argc > 1 && std::string(argv[1]) == "-v";
    int status = EXIT_SUCCESS;

    try {
        if (verbose) std::puts("Lpy_Initialize");
        Lpy_Initialize();

        if (verbose) std::puts("Create Lsystem");
        Lsystem model(LPY_EMBEDDING_EXAMPLE);
        const AxialTree result = model.derive();
        std::printf("'%s'\n", result.str().c_str());
    }
    catch (const boost::python::error_already_set&) {
        PyErr_Print();
        status = EXIT_FAILURE;
    }

    if (Lpy_IsInitialized()) {
        if (verbose) std::puts("Lpy_Finalize");
        Lpy_Finalize();
    }
    if (verbose) std::puts("Done.");
    return status;
}
