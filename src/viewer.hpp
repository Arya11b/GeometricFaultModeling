#ifndef VIEWER_H
#define VIEWER_H

// TODO:
// [done] make viewport a seperate class
// make GUI a seperate class
// [done] overload ImVec2 operators instead of function declarations
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiPlugin.h>
#include <igl/opengl/glfw/Viewer.h>
#include "drawable.hpp"
#include "object.hpp"
#include <vector>
#include <utility>


enum MOUSE_STATE{
    UNCLICKED,
    CLICKED,
    DRAGGING,
    RELEASED

};

class Viewer
{
private:
    // sketching on surface variables
    // likely to be refractored to another class
    MOUSE_STATE mouse_state = UNCLICKED;
    std::vector<Eigen::RowVector3d> sketchPoints;

    int width, height;
    int viewerIndex{};
    Eigen::Matrix<double, 10, 3> cols;
    igl::opengl::glfw::Viewer viewer;
    igl::opengl::glfw::imgui::ImGuiPlugin plugin;




    std::vector<int> vidIndex;

    bool exists(int id);
    int assignVid();

    Object raycasterObject;

public:
    void addMenu(igl::opengl::glfw::imgui::ImGuiMenu& menu);
    void launch();
    Viewer();

    Eigen::MatrixXd getSketchPoints();


    int getSize(ImVec2 end, ImVec2 begin) const;

    void clear();
    void clear(const Drawable& d);

    void setRaycastObject(Object& obj);

    void resetCallbacks();
    void setRaycasterCallback();

    bool releasedMouse();


    // [DONE] use polymorphism

    void render(Drawable& d);
    //refractor
    void render(Eigen::MatrixX3d& v);


};

#endif // !VIEWER_H
