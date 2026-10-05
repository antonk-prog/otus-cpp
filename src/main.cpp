#include "duplicates.h"
#include "scan.h"

#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace po = boost::program_options;

namespace {

void printGroups(const std::vector<std::vector<std::filesystem::path>>& groups) {
    bool first = true;
    for (const auto& group : groups) {
        if (!first) {
            std::cout << '\n';
        }
        first = false;
        for (const auto& path : group) {
            std::cout << path.string() << '\n';
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    ScanOptions options;

    po::options_description desc("Usage: bayan -d <dir> [options]");
    desc.add_options()
        ("help,h", "show this help message")
        ("path,d", po::value<std::vector<std::string>>()->composing(),
            "directory to scan (may be repeated)")
        ("exclude,e", po::value<std::vector<std::string>>()->composing(),
            "directory to exclude from scanning (may be repeated)")
        ("level,l", po::value<int>()->default_value(-1),
            "scan depth: 0 - only the specified directories, n - n levels deep, -1 - unlimited")
        ("min-size,m", po::value<std::uint64_t>()->default_value(1),
            "minimum file size in bytes, files must be strictly larger (default: 1)")
        ("mask,n", po::value<std::vector<std::string>>()->composing(),
            "file name mask, * and ? are supported, case-insensitive (may be repeated)")
        ("block-size,s", po::value<std::uint64_t>()->default_value(4096),
            "block size S in bytes (default: 4096)")
        ("hash,a", po::value<std::string>()->default_value("crc32"),
            "block hash function H: crc32 or md5 (default: crc32)");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "bayan: " << e.what() << "\n\n" << desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (!vm.count("path")) {
        std::cerr << "bayan: at least one -d/--path directory is required\n\n" << desc << std::endl;
        return 1;
    }

    try {
        options.algo = parseHashAlgo(vm["hash"].as<std::string>());
    } catch (const std::exception& e) {
        std::cerr << "bayan: " << e.what() << std::endl;
        return 1;
    }

    options.blockSize = vm["block-size"].as<std::uint64_t>();
    if (options.blockSize == 0 || options.blockSize > (1ull << 30)) {
        std::cerr << "bayan: block size must be in range [1, 1073741824]" << std::endl;
        return 1;
    }
    options.level = vm["level"].as<int>();
    options.minSize = vm["min-size"].as<std::uint64_t>();

    for (const auto& dir : vm["path"].as<std::vector<std::string>>()) {
        options.paths.emplace_back(dir);
    }
    if (vm.count("exclude")) {
        for (const auto& dir : vm["exclude"].as<std::vector<std::string>>()) {
            options.exclude.emplace_back(dir);
        }
    }
    if (vm.count("mask")) {
        options.masks = vm["mask"].as<std::vector<std::string>>();
    }

    try {
        const auto files = scanFiles(options);
        const auto groups = findDuplicates(files, options.blockSize, options.algo);
        printGroups(groups);
    } catch (const std::exception& e) {
        std::cerr << "bayan: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
