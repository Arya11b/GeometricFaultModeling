#ifndef OBJECT_H
#define OBJECT_H
// move this class definition to the 'geometries' namespace

#include "drawable.hpp"
#include <igl/Viewport.h>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>



class Object: public Drawable {
	friend class Viewer;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	//default constructor
	Object();
	// copy constructor
	Object(const Object& obj);
	Object& operator=(const Object& obj);
	Object(Object&& obj);
	Object& operator=(Object&& obj);
	~Object() = default;

	void init(Eigen::MatrixX3d vertex, Eigen::MatrixX3i face, Eigen::RowVector3d pos = Eigen::RowVector3d::Zero());
	//Object(const Object& obj) = default;
	//Object& operator=(const Object& obj) = default;
	//Object(Object&& obj) {}
	//Object& operator=(Object&& obj) {}

	Eigen::RowVector3d position();
	void setPosition(Eigen::RowVector3d pos);
	Eigen::RowVector3d getVertex(int i) const;

	Eigen::MatrixX3d getVertices() const;
	Object get() const;
	Eigen::MatrixX3i getFaces() const;

	void setVertices(Eigen::MatrixX3d& vertex);
	void setFaces(Eigen::MatrixX3i& face);

	void clear();
protected:
	Eigen::MatrixX3d vertex;
	Eigen::MatrixX3i face;
	Eigen::RowVector3d pos;
	Eigen::RowVector3d rotation;
private: 
	void render(igl::opengl::glfw::Viewer* viewer) override;
};

#endif 