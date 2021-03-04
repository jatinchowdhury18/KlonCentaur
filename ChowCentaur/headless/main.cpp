#include "benchmarks.h"
#include "screenshots.h"

void usage()
{
    std::cout << "ChowCentaurHeadless supports the following modes:" << std::endl;
    std::cout << "    bench" << std::endl;
    std::cout << "    screenshot" << std::endl;
}

int main (int argc, char** argv)
{
    std::cout << "Running ChowCentaur version " << ProjectInfo::versionString << " in headless mode..." << std::endl;
    ScopedJuceInitialiser_GUI scopedJuce;

    if (argc < 1)
    {
        usage();
        return 1;
    }

    if (argc > 1 && std::string (argv[1]) == "--help")
    {
        usage();
        return 0;
    }

    if (argc > 1 && std::string (argv[1]) == "bench")
    {
        return benchmarks (argc - 1, &argv[1]);
    }

    if (argc > 1 && std::string (argv[1]) == "screenshot")
    {
        takeScreenshots();
        return 0;
    }

    usage();
    return 1;
}
