#include "scan.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <system_error>
#include <unordered_set>

namespace {

namespace fs = std::filesystem;

std::string escapeRegex(const std::string& glob) {
    static const std::string specials = R"(\.^$|()[]{}*+?)";
    std::string result;
    result.reserve(glob.size() * 2);
    for (char c : glob) {
        if (specials.find(c) != std::string::npos) {
            result.push_back('\\');
        }
        result.push_back(c);
    }
    return result;
}

std::regex makeMaskPattern(const std::string& mask) {
    std::string pattern;
    pattern.reserve(mask.size() * 2 + 2);
    pattern.push_back('^');
    for (std::size_t i = 0; i < mask.size(); ++i) {
        const char c = mask[i];
        if (c == '*') {
            pattern += ".*";
        } else if (c == '?') {
            pattern.push_back('.');
        } else if (static_cast<unsigned char>(c) < 0x80) {
            pattern += escapeRegex(std::string(1, c));
        } else {
            pattern.push_back(c);
        }
    }
    pattern.push_back('$');
    return std::regex(pattern, std::regex_constants::icase | std::regex_constants::ECMAScript);
}

std::string canonicalKey(const fs::path& path) {
    std::error_code ec;
    fs::path canonical = fs::canonical(path, ec);
    if (ec) {
        canonical = fs::absolute(path, ec);
        if (ec) {
            canonical = path;
        }
        canonical = canonical.lexically_normal();
    }
    return canonical.string();
}

class Scanner {
public:
    explicit Scanner(const ScanOptions& options)
        : options_(options) {
        for (const auto& mask : options_.masks) {
            patterns_.push_back(makeMaskPattern(mask));
        }
        for (const auto& dir : options_.exclude) {
            exclude_.insert(canonicalKey(dir));
        }
    }

    std::vector<ScannedFile> run() {
        for (const auto& path : options_.paths) {
            std::error_code ec;
            if (!fs::exists(path, ec)) {
                std::cerr << "bayan: directory does not exist: " << path << '\n';
                continue;
            }
            if (isExcluded(path)) {
                continue;
            }
            scanDir(path, 0);
        }
        std::sort(files_.begin(), files_.end(),
                  [](const ScannedFile& a, const ScannedFile& b) { return a.path < b.path; });
        return std::move(files_);
    }

private:
    bool isExcluded(const fs::path& dir) const {
        return exclude_.count(canonicalKey(dir)) != 0;
    }

    bool matchesMask(const fs::path& file) const {
        if (patterns_.empty()) {
            return true;
        }
        const std::string name = file.filename().string();
        return std::any_of(patterns_.begin(), patterns_.end(),
                           [&](const std::regex& re) { return std::regex_match(name, re); });
    }

    void scanDir(const fs::path& dir, int depth) {
        std::error_code ec;
        fs::directory_iterator it(dir, fs::directory_options::skip_permission_denied, ec);
        if (ec) {
            std::cerr << "bayan: cannot read directory: " << dir << ": " << ec.message() << '\n';
            return;
        }
        const fs::directory_iterator end;
        for (; it != end; it.increment(ec)) {
            if (ec) {
                std::cerr << "bayan: error iterating " << dir << ": " << ec.message() << '\n';
                break;
            }
            const fs::directory_entry& entry = *it;
            const bool isDir = entry.is_directory(ec);
            if (ec) {
                continue;
            }
            if (isDir) {
                if (isExcluded(entry.path())) {
                    continue;
                }
                if (options_.level < 0 || depth < options_.level) {
                    scanDir(entry.path(), depth + 1);
                }
                continue;
            }
            if (!entry.is_regular_file(ec) || ec) {
                continue;
            }
            const std::uint64_t size = entry.file_size(ec);
            if (ec || size <= options_.minSize) {
                continue;
            }
            if (!matchesMask(entry.path())) {
                continue;
            }
            std::error_code absEc;
            fs::path fullPath = fs::absolute(entry.path(), absEc);
            if (absEc) {
                fullPath = entry.path();
            }
            fullPath = fullPath.lexically_normal();
            if (!seen_.insert(canonicalKey(fullPath)).second) {
                continue;
            }
            files_.push_back({std::move(fullPath), size});
        }
    }

    const ScanOptions& options_;
    std::vector<std::regex> patterns_;
    std::unordered_set<std::string> exclude_;
    std::unordered_set<std::string> seen_;
    std::vector<ScannedFile> files_;
};

}  // namespace

std::vector<ScannedFile> scanFiles(const ScanOptions& options) {
    Scanner scanner(options);
    return scanner.run();
}
