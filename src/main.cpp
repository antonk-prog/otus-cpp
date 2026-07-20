#include "controller.h"

namespace
{
    std::unique_ptr<Controller> controller = std::make_unique<Controller>();
}

void OnCreateDocument()
{
    controller->CreateNewDocument();
}

void OnImportDocument()
{
    std::string path = "input.txt";
    controller->ImportDocument(path);
}

void OnExportDocument()
{
    std::string path = "output.txt";
    controller->ExportDocument(path);
}

void OnCreateRectangle()
{
    auto rect = std::make_shared<Rectangle>(15.0, 10.0);
    controller->AddShape(rect);
}

void OnCreateCircle() {
    auto circ = std::make_shared<Circle>(5.0);
    controller->AddShape(circ);
}

void OnDeleteShape() {
    size_t index = 0;
    controller->RemoveShape(index);
}

int main()
{
    OnCreateDocument();
    OnCreateRectangle();
    OnCreateCircle();
    OnExportDocument();
    OnDeleteShape();
    return 0;
}