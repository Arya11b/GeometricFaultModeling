#ifndef FCURVE_H
#define FCURVE_H


#include <igl/opengl/glfw/Viewer.h>
#include <CurveN.hpp>
#include "drawable.hpp"
#include "implicit.hpp"


class FCurve: public Drawable
{
	friend class Fault;
public:
	FCurve();
	FCurve(const Curve2D& curve);
	void init(const Curve2D& curve);
	void init(Eigen::MatrixX3d points);
	void shiftZ(double amount);
	void standardFilter();
	void transformFCurveToPlane(Eigen::RowVector3d b, Eigen::RowVector3d bx, Eigen::RowVector3d by, double width, double height);
	void calculateNormals(Eigen::RowVector3d planeNormal);
	void calculateNormals(const Implicit& surface);
	void render(igl::opengl::glfw::Viewer* viewer) override;
	Eigen::MatrixX3d getVertices() const;




private:
	// user-defined fcurves
	Eigen::MatrixX3d vertex;
	Eigen::MatrixX3d normal;
	
	// curve in 3d space
};


#endif // !FCURVE_H
