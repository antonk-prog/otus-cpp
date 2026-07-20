#pragma once
#include <iostream>
#include <string>
class FileWorker
{
public:
    void LoadFromFile(const std::string &path)
    {
        std::cout << "[FileWorker::LoadFromFile] path: " << path << "\n";
    }
    void SaveToFile(const std::string &path)
    {
        std::cout << "[FileWorker::SaveToFile] path: " << path << "\n";
    }
};