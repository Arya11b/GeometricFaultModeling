#include "sketchplane.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>

SketchPlane::SketchPlane()
{
}

SketchPlane::SketchPlane(double width, double height) {
	init(width, height);
}

void SketchPlane::addCurveToPlane(const Curve2D& curve) {
	fcurve.init(curve);
	fcurve.transformFCurveToPlane(ul(), ur(), dl(), width, height);
	fcurve.calculateNormals(getNormal());
}

void SketchPlane::init(double width, double height) {
	this->width = width;
	this->height = height;

	makeZ(width,height);
}

// returing the for vertex positions of the sketch plane, up-left, up-right, down-left and down-right

Eigen::RowVector3d SketchPlane::ul() {
	return object.getVertex(1);
}

Eigen::RowVector3d SketchPlane::ur() {
	return object.getVertex(3);
}

Eigen::RowVector3d SketchPlane::dl() {
	return object.getVertex(0);
}

Eigen::RowVector3d SketchPlane::dr() {
	return object.getVertex(2);
}

Eigen::RowVector3d SketchPlane::getNormal() {
	return
		(dr() - dl()).normalized().cross((ul() - dl()).normalized());
}

void SketchPlane::makeZ(double w, double h)
{
	Eigen::Matrix<double,4,3> vertex;
	Eigen::Matrix<int, 2, 3> face;
	vertex <<
		-width / 2, -height / 2, 0.f,
		-width / 2, height / 2, 0.f,
		width / 2, -height / 2, 0.f,
		width / 2, height / 2, 0.f;
	face << 2, 1, 0,
		3, 1, 2;
	object.init(vertex, face);
}

void SketchPlane::makeY(double w, double h)
{
	Eigen::Matrix<double,4,3> vertex;
	Eigen::Matrix<int, 2, 3> face;
	vertex <<
		-width / 2, 0.f, -height / 2,
		-width / 2, 0.f, height / 2,
		width / 2, 0.f, -height / 2,
		width / 2, 0.f, height / 2;
	face << 2, 1, 0,
		3, 1, 2;
	object.init(vertex, face);
}

void SketchPlane::makeX(double w, double h)
{
	Eigen::Matrix<double,4,3> vertex;
	Eigen::Matrix<int, 2, 3> face;
	vertex <<
		0.f, -width / 2, -height / 2,
		0.f, -width / 2, height / 2,
		0.f, width / 2, -height / 2,
		0.f, width / 2, height / 2;
	face << 2, 1, 0,
			3, 1, 2;
	object.init(vertex, face);
}
