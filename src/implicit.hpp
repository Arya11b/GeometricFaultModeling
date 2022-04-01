#ifndef CONTINUOUSOBJECT_H
#define CONTINUOUSOBJECT_H

#include <igl/marching_cubes.h>
#include <igl/voxel_grid.h>
#include <igl/MeshBooleanType.h>
#include "hrbf.hpp"
#include "object.hpp"
#include <igl/opengl/glfw/Viewer.h>
#include <igl/signed_distance.h>

#include <optional>


class Implicit {
private:
	HRBF hrbf;
	bool fitted{};

	Eigen::RowVector3i res;

	// bounding box
	Eigen::MatrixXd BV;
	Eigen::MatrixXi BF;

	// surface interpolation evaluation
	// after fit algorithm will be initialized
	Eigen::VectorXd evals;
	Eigen::MatrixX3d GV;




public:

	Eigen::MatrixX3d control_vertex, control_normal;

	void showGVPoints(igl::opengl::glfw::Viewer& viewer) const;
	Implicit() = default;
	Implicit(Eigen::MatrixXd control_vertex, Eigen::MatrixXd control_normal);

	virtual void clear();

	void add_data(Eigen::MatrixXd appending_vertex, Eigen::MatrixXd appending_normal);
	void add_data(Eigen::MatrixXd appending_vertex, Eigen::MatrixXd appending_normal, Eigen::RowVector3d offset);
	void reset_data(Eigen::MatrixXd new_vertex, Eigen::MatrixXd new_normal);
	void print_vertices();



	void create_mesh(int resolution, const Eigen::MatrixX3d& bounds, Object& object);
	void print_num(int i);

	double eval(const Eigen::RowVector3d point) const;
	Eigen::RowVector3d grad(const Eigen::RowVector3d point) const;

	Eigen::VectorXd getEvals() const;
	Eigen::MatrixX3d getGrid() const;
	Eigen::RowVector3i getRes() const;


};

#endif