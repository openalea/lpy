
#include <lpy/lsystem.h>
#include <lpy/lpyrun.h>
#include <plantgl/python/boost_python.h>

LPY_USING_NAMESPACE

int main(int argc, char * argv[]) {
    bool verbose = false;
    if ((argc > 1) && (std::string(argv[1]) == "-v")) {
        verbose = true;
    }
    if (verbose) printf("Lpy_Initialize\n");
    Lpy_Initialize();

    {

        if (verbose) printf("Create Lsystem\n");
        Lsystem l("../fibonacci.lpy");
        AxialTree a = l.derive();
        printf("'%s'\n",a.c_str());
        // LocalContext l;
        // AxialTree a;

    }

    if (verbose) printf("Lpy_Finalize\n");
    Lpy_Finalize();

    if (verbose) printf("Done.\n");
    return EXIT_SUCCESS;
}