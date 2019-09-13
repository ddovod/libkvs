
// clang-format off
#include <iostream>
#include "TestRunner.hpp"
#include "utility.hpp"

#define BACKWARD_HAS_BFD 1
#include <backward.hpp>

int main(int argc, char** argv)
{
    backward::SignalHandling sh;

    argh::parser args(argc, argv);
    TestOptions opts;
    if (!parseParam(args, "threads", opts.threadsCount) ||
        !parseParam(args, "cacheSize", opts.cacheSize) ||
        !parseParam(args, "recordSize", opts.recordSize) ||
        !parseParam(args, "dataSize", opts.dataSize) ||
        !parseParam(args, "maxNodes", opts.maxNodes) ||
        !parseParam(args, "reads", opts.readsWeight) ||
        !parseParam(args, "writes", opts.writesWeight) ||
        !parseParam(args, "deletes", opts.deletesWeight) ||
        !parseParam(args, "mounts", opts.mountsWeight) ||
        !parseParam(args, "unmounts", opts.unmountsWeight) ||
        !parseParam(args, "krqs", opts.krqsWeight) ||
        !parseParam(args, "testTime", opts.testTime) ||
        !parseParam(args, "window", opts.windowSize) ||
        !parseParam(args, "keyLength", opts.keyLength) ||
        !parseParam(args, "rndSeed", opts.rndSeed)) {
        return 1;
    }

    auto result = TestRunner(opts).run();

    for (const auto& sample : result.samples) {
        std::cout << sample.totalOps << " : "
                  << sample.readOps << " , "
                  << sample.writeOps << " , "
                  << sample.deleteOps << " , "
                  << sample.mountOps << " , "
                  << sample.unmountOps << " , "
                  << sample.keyRangeQueryOps
                  << std::endl;
    }

    return 0;
}
