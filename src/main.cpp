#include <cstdio>
#include <cstdlib>
#include <string>

#include "dataset.h"
#include "mlp.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s <test.csv> <model_dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Mlp model;
    model.load(argv[2]);
    std::vector<Sample> samples = loadTestData(argv[1]);

    std::printf("%.3f\n", model.evaluate(samples));
    return EXIT_SUCCESS;
}
