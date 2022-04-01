#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "drawable.hpp"
#include <igl/opengl/glfw/Viewer.h>
#include "object.hpp"


class BoundingBox: public Drawable {

	friend class BoundingBoxCWidget;

	double x_dim{}, y_dim{}, z_dim{};
	std::vector<std::pair<int,int>> edgeList;
	Object object;


public:
	BoundingBox();
	BoundingBox(double x_dim, double y_dim, double z_dim, Eigen::RowVector3d pos = Eigen::RowVector3d::Zero());
	void init(double x_dim, double y_dim, double z_dim, Eigen::RowVector3d pos = Eigen::RowVector3d::Zero());
	void render(igl::opengl::glfw::Viewer* viewer) override;
};

#endif