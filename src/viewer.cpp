#include "viewer.hpp"
#include "fcurve.hpp"
#include <algorithm>
#include <igl/unproject_onto_mesh.h>
#include <vector>

bool Viewer::exists(int vid) {
	for (const auto& u : vidIndex) {
		if (u == vid) {
			return true;
		}
	}
	return false;
}
void Viewer::addMenu(igl::opengl::glfw::imgui::ImGuiMenu& menu) {
	viewer.plugins.push_back(&plugin);
	plugin.widgets.push_back(&menu);
}
void Viewer::launch() {
	viewer.launch(true, false, "FaultZi",800,600);
}
Viewer::Viewer() {
	cols << .9f, .1f, .1f,
		.7f, .3f, .2f,
		.8f, .1f, .4f,
		.6f, .3f, .31f,
		.7f, .3f, .2f,
		.7f, .3f, .2f,
		.7f, .3f, .2f,
		.7f, .3f, .2f,
		.7f, .3f, .2f,
		.7f, .3f, .2f;

	viewer.data().point_size = 5;
}

void Viewer::resetCallbacks(){
	mouse_state = UNCLICKED;
	auto func = [](igl::opengl::glfw::Viewer& viewer, int, int)->bool{ return false;};
	viewer.callback_mouse_down = func;
	viewer.callback_mouse_up = func;
	viewer.callback_mouse_move = func;
}

void Viewer::setRaycasterCallback(){
	if(raycasterObject.getVertices().rows() > 0){
    viewer.callback_mouse_up =
        [this](igl::opengl::glfw::Viewer& viewer, int mb, int mo)->bool {
		if(mouse_state == DRAGGING) mouse_state = RELEASED;
		else mouse_state = UNCLICKED;
        return false;
    };
    viewer.callback_mouse_down =
        [this](igl::opengl::glfw::Viewer& viewer, int mb, int mo)->bool {
        mouse_state = CLICKED;
        return false;
    };


    viewer.callback_mouse_move =
        [this](igl::opengl::glfw::Viewer& viewer, int mb, int mo)->bool
    {

        if (mouse_state == CLICKED || mouse_state == DRAGGING) {
           int fid;
        Eigen::Vector3f bc;
        // Cast a ray in the view direction starting from the mouse position
        double x = viewer.current_mouse_x;
        double y = viewer.core().viewport(3) - viewer.current_mouse_y;
		const auto& V = raycasterObject.getVertices();
		const auto& F = raycasterObject.getFaces();
        if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), viewer.core().view,
            viewer.core().proj, viewer.core().viewport, V, F, fid, bc))
        {
            // paint hit red
            Eigen::RowVector3d exactV;
            exactV << 0, 0, 0;
            for (size_t i = 0; i < 3; i++)
            {
                exactV += bc(i) * V.row(F(fid, i));
            }
            
            std::cout << exactV << std::endl << std::endl;
			sketchPoints.emplace_back(exactV);
            viewer.data().add_points(exactV, Eigen::RowVector3d(0, 0, 1));
			mouse_state = DRAGGING;
            return true;
        }
        return false; 
        }
		else return false;
    };
	}
}

//returns sketch points, clears it after returning
Eigen::MatrixXd Viewer::getSketchPoints(){
	Eigen::MatrixXd mat;
	mat.conservativeResize(sketchPoints.size(),3);
	for (size_t i = 0; i < mat.rows(); i++)
	{
		mat.row(i) = sketchPoints.at(i);
	}
	sketchPoints.clear();
	return mat;
	
}

bool Viewer::releasedMouse(){
	return mouse_state == RELEASED;
}
void Viewer::setRaycastObject(Object& obj){
	raycasterObject = obj;
}

int Viewer::getSize(ImVec2 end, ImVec2 begin) const {
	return (end.y - begin.y) * (end.x * begin.x);
}

void Viewer::clear() {
	viewer.data().clear();
}

void Viewer::clear(const Drawable& d) {
	if(!d.isRendered()) return;
	viewer.selected_data_index = d.vid;
	viewer.data().clear();
	for (size_t i = 0; i < vidIndex.size(); i++)
	{
		if (vidIndex.at(i) == d.vid) {
			vidIndex.erase(vidIndex.begin() + i);
		}
	}
}


// [DONE] use polymorphism

void Viewer::render(Drawable& d) {
	if (d.vid == -1) {
		/*if(viewerIndex > 0)*/ viewer.append_mesh();
		viewerIndex++;
		d.vid = assignVid();
		vidIndex.push_back(d.vid);
	}
	viewer.selected_data_index = d.vid;
	viewer.data().clear();
	Eigen::RowVector3d col;
	
	d.render(&viewer);
}
void Viewer::render(Eigen::MatrixX3d& v) {
	viewer.data().set_points(v,Eigen::RowVector3d(1.f,.5f,.25f));
}

int Viewer::assignVid(){
	int vid = 0;
	std::sort(vidIndex.begin(),vidIndex.end());
	for (auto it = vidIndex.begin(); it != vidIndex.end();)
	{
		if(vid < *it) {
			break;
		} 
		else if(vid == *it){
			vid++;
			it++;
		}
	}
	
	return vid;
}