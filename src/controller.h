#pragma once
#include <memory>

#include "model/document.h"
#include "model/file_worker.h"
#include "view.h"

class Controller
{
private:
    FileWorker m_file_worker;
    std::shared_ptr<Document> m_document;
    std::shared_ptr<View> m_view;
public:
    Controller()
    {
        m_document = std::make_shared<Document>(m_file_worker);
        m_view = std::make_shared<View>();
    }

    void CreateNewDocument()
    {
        std::cout << "[Controller::CreateNewDocument]" << "\n";
        m_document = std::make_shared<Document>(m_file_worker);
        UpdateView();
    }

    void ImportDocument(const std::string& path)
    {
        std::cout << "[Controller::ImportDocument] path: " << path << "\n";
        m_document->LoadFromFile(path);
        UpdateView();
    }

    void ExportDocument(const std::string& path)
    {
        std::cout << "[Controller::ExportDocument] path: " << path << "\n";
        m_document->SaveToFile(path);
        UpdateView();
    }

    void AddShape(std::shared_ptr<Shape> shape)
    {
        std::cout << "[Controller::AddShape]" << "\n";
        m_document->AddShape(shape);
        UpdateView();
    }

    void RemoveShape(size_t index)
    {
        std::cout << "[Controller::RemoveShape] index: " << index << "\n";
        m_document->RemoveShape(index);
        UpdateView();
    }

private:
    void UpdateView()
    {
        m_view->render(*m_document);
    }
};