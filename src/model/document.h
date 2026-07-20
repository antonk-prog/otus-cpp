#pragma once
#include <vector>
#include "figures.h"
#include "file_worker.h"
class Document
{
public:
    Document(FileWorker& file_worker) : m_file_worker{file_worker} {}
    
    void AddShape(std::shared_ptr<Shape> shape)
    {
        m_figures.push_back(shape);
        std::cout << "[Document::AddShape] shapes count: " << m_figures.size() << "\n";
    }
    void RemoveShape(size_t index)
    {
        if (index < m_figures.size())
        {
            m_figures.erase(m_figures.begin() + index);
            std::cout << "[Document::RemoveShape] removed index: " << index
                      << ", shapes count: " << m_figures.size() << "\n";
        }
        else
        {
            std::cout << "[Document::RemoveShape] index out of range: " << index << "\n";
        }
    }
    const std::vector<std::shared_ptr<Shape>>& getShapes() const
    {
        return m_figures;
    }
    void LoadFromFile(const std::string& path)
    {
        std::cout << "[Document::LoadFromFile] path: " << path << "\n";
        m_file_worker.LoadFromFile(path);
    }
    void SaveToFile(const std::string& path)
    {
        std::cout << "[Document::SaveToFile] path: " << path << "\n";
        m_file_worker.SaveToFile(path);
    }

private:
    FileWorker& m_file_worker;
    std::vector<std::shared_ptr<Shape>> m_figures {};
    std::string m_filename;

};