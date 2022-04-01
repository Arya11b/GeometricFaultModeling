#include "boundingbox.hpp"
#include "boundingboxcwidget.hpp"

BoundingBox::BoundingBox() :Drawable() {}

BoundingBox::BoundingBox(double x_dim, double y_dim, double z_dim, Eigen::RowVector3d pos) : x_dim(x_dim), y_dim(y_dim), z_dim(z_dim) {
	init(x_dim, y_dim, z_dim, pos);
}

void BoundingBox::init(double x_dim, double y_dim, double z_dim, Eigen::RowVector3d pos) {
	this->x_dim = x_dim;
	this->y_dim = y_dim;
	this->z_dim = z_dim;

	Eigen::Matrix<double,8,3> vertex;
	Eigen::Matrix<int,12,3> face;
	vertex <<
		-x_dim / 2, -y_dim / 2, -z_dim / 2,
		-x_dim / 2, -y_dim / 2, z_dim / 2,
		-x_dim / 2, y_dim / 2, -z_dim / 2,
		-x_dim / 2, y_dim / 2, z_dim / 2,
		x_dim / 2, -y_dim / 2, -z_dim / 2,
		x_dim / 2, -y_dim / 2, z_dim / 2,
		x_dim / 2, y_dim / 2, -z_dim / 2,
		x_dim / 2, y_dim / 2, z_dim / 2;
	face <<
		1, 7, 5,
		1, 3, 7,
		1, 4, 3,
		1, 2, 4,
		3, 8, 7,
		3, 4, 8,
		5, 7, 8,
		5, 8, 6,
		1, 5, 6,
		1, 6, 2,
		2, 6, 8,
		2, 8, 4;
	face = face.array() - 1;
	object.init(vertex, face);
	// add edges that we want to render
	edgeList.push_back({ 0,1 });
	edgeList.push_back({ 0,2 });
	edgeList.push_back({ 0,4 });
	edgeList.push_back({ 1,3 });
	edgeList.push_back({ 1,5 });
	edgeList.push_back({ 2,3 });
	edgeList.push_back({ 2,6 });
	edgeList.push_back({ 3,7 });
	edgeList.push_back({ 4,5 });
	edgeList.push_back({ 4,6 });
	edgeList.push_back({ 5,7 });
	edgeList.push_back({ 6,7 });
}

void BoundingBox::render(igl::opengl::glfw::Viewer* viewer)  {
	Eigen::RowVector3d col= Eigen::RowVector3d(0.f, 0.f, 0.f);
	//viewer->data().point_size = 25;
	//viewer->data(vid).add_points(object.getVertices(), Eigen::RowVector3d(0.f, 1.f, 0.f));

	for (int i = 0; i < edgeList.size(); i++)
	{
		viewer->data(vid).add_edges(object.getVertex(edgeList.at(i).first), object.getVertex(edgeList.at(i).second), col);
	}
}
