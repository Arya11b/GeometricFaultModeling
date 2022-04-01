#ifndef MENU_H
#define MENU_H

#include <igl/opengl/glfw/imgui/ImGuiPlugin.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include  <memory>
#include "sketchcwidget.hpp"
#include "boundingboxcwidget.hpp"
#include "faultcwidget.hpp"
#include "viewer.hpp"
#include "faultdb.hpp"

// Menu/Controller class
class Menu
{
public:
    Menu() = delete;
    Menu(std::shared_ptr<Viewer> v);

private:


    std::shared_ptr<Viewer> v;

    SketchCWidget sw{};
    BoundingBoxCWidget bbw{};
    FaultCWidget fw{};

    igl::opengl::glfw::imgui::ImGuiMenu menu;

    void setCallBacks();
};

#endif