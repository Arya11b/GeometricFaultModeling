#ifndef SKETCHPLANE_H
#define SKETCHPLANE_H

#include <CurveN.hpp>
#include "object.hpp"
#include "fcurve.hpp"


class SketchPlane
{
	friend class SketchCWidget;
public:
	SketchPlane();
	SketchPlane(double width, double height);
	void init(double width, double height);
	
	void addCurveToPlane(const Curve2D& curve);
	// returing the for vertex positions of the sketch plane, up-left, up-right, down-left and down-right
	Eigen::RowVector3d ul();
	Eigen::RowVector3d ur();
	Eigen::RowVector3d dl();
	Eigen::RowVector3d dr();
	Eigen::RowVector3d getNormal();
	Object object;

	void makeZ(double w, double h);
	void makeY(double w, double h);
	void makeX(double w, double h);
private:
	FCurve fcurve;
	double width{};
	double height{};
};


#endif // !SKETCHPLANE_H
