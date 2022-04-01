#include "fcurve.hpp"
#include "defs/eigen_curveN_conversions.hpp"

FCurve::FCurve() : Drawable() {}

FCurve::FCurve(const Curve2D& curve) : Drawable() {
	init(curve);
}

Eigen::MatrixX3d FCurve::getVertices() const{
	return vertex;
}

void FCurve::init(const Curve2D& curve) {
	vertex.conservativeResize(curve.size(), 3);
	for (int i = 0; i < curve.size(); i++)
	{
		vertex.row(i) << curve.at(i).x(), curve.at(i).y(), 0.0f;
	}
}
void FCurve::init(Eigen::MatrixX3d points) {
	vertex = points;
}

void FCurve::shiftZ(double amount) {
	vertex.col(2).array() += amount;
}

void FCurve::transformFCurveToPlane(Eigen::RowVector3d b, Eigen::RowVector3d bx, Eigen::RowVector3d by, double width, double height) {
	Eigen::RowVector3d ux = (bx - b).normalized();
	Eigen::RowVector3d uy = (by - b).normalized();
	for (int i = 0; i < vertex.rows(); i++)
	{
		vertex.row(i) = vertex(i, 0) * ux * width + vertex(i, 1) * uy * height + b;
	}
}

void FCurve::calculateNormals(Eigen::RowVector3d planeNormal) {
	normal.conservativeResizeLike(vertex);
	for (int i = 1; i < vertex.rows(); i++)
	{
		normal.row(i) = (vertex.row(i) - vertex.row(i - 1)).normalized().cross(planeNormal);
		//std::cout << "normals: " << fc_normal.row(i) << std::endl;
	}
	normal.row(0) = (vertex.row(1) - vertex.row(0)).normalized().cross(planeNormal);


}

void FCurve::calculateNormals(const Implicit& surface) {
	normal.conservativeResizeLike(vertex);
	for (int i = 1; i < vertex.rows(); i++)
	{
		normal.row(i) = (vertex.row(i) - vertex.row(i - 1)).normalized().cross(surface.grad(vertex.row(i)));
		//normal.row(i) = surface.grad(vertex.row(i));
		//std::cout << "normals: " << fc_normal.row(i) << std::endl;
	}
	//normal.row(0) = surface.grad(vertex.row(0));
	normal.row(0) = (vertex.row(1) - vertex.row(0)).normalized().cross(surface.grad(vertex.row(0)));
}

void FCurve::render(igl::opengl::glfw::Viewer* viewer) {
	//viewer->data(vid).point_size = 20;
	//viewer->data(vid).add_points(fc_vertex, Eigen::RowVector3d(1.f, 0.f, 1.f));
	 Eigen::RowVector3d col;
	 col(0) = 1.f;
	 col(1) = 0.f;
	 col(2) = 0.f;
	viewer->data(vid).line_width = 80.f;
	viewer->data(vid).clear_edges();
	for (int i = 1; i<vertex.rows(); i++) {
		viewer->data(vid).add_edges(vertex.row(i - 1), vertex.row(i), col);
	}
	// for (int i = 0; i<vertex.rows(); i++) {
	// 	viewer->data(vid).add_edges(vertex.row(i), vertex.row(i) + normal.row(i), col);
	// }

}

void FCurve::standardFilter(){
	Curve3D curve;
	conversions::eigen_to_curveN(vertex,curve);
	curve.lineFilter(0.002f, 8);
	curve.meanFilter();
	curve.meanFilter();
	conversions::curveN_to_eigen(curve,vertex);

}
