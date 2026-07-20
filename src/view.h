#pragma once

#include "model/document.h"

class View
{
public:
    void render(const Document& doc)
    {
        std::cout << "[View::render] shapes count: " << doc.getShapes().size() << "\n";
        for (const auto& shape : doc.getShapes())
        {
            shape->Draw();
        }
    }

};