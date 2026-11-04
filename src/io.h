#pragma once

#include <string>
#include <vector>

[[noreturn]] void die(const std::string& msg);

std::vector<char> readFile(const std::string& path);

char* skipSeparators(char* q);

std::vector<float> loadMatrix(const std::string& path, int rows, int cols);
