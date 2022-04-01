#include "implicit.hpp"




void Implicit::showGVPoints(igl::opengl::glfw::Viewer& viewer) const {
	Eigen::RowVector3d col;
	col << 0.f, 1.f, 0.f;

	if (BV.size() != 0)
		viewer.data().add_points(BV, col);
}

Implicit::Implicit(Eigen::MatrixXd control_vertex, Eigen::MatrixXd control_normal) {
	//std::cout << vertex_data.rows() << std::endl;
	assert(control_vertex.rows() == control_normal.rows());
	this->control_vertex.conservativeResize(control_vertex.rows(), 3);
	this->control_normal.conservativeResize(control_vertex.rows(), 3);
	this->control_vertex = control_vertex;
	this->control_normal = control_normal;

}

void Implicit::clear() {
	control_vertex.conservativeResize(0, control_vertex.cols());
	control_normal.conservativeResize(0, control_normal.cols());
}

void Implicit::add_data(Eigen::MatrixXd appending_vertex, Eigen::MatrixXd appending_normal) {
	Eigen::RowVector3d offset;
	offset.setZero();
	add_data(appending_vertex, appending_normal, offset);
}

void Implicit::add_data(Eigen::MatrixXd appending_vertex, Eigen::MatrixXd appending_normal, Eigen::RowVector3d offset) {
	assert(appending_normal.rows() == appending_vertex.rows());
	for (int i = 0; i < appending_vertex.rows(); i++)
	{
		appending_vertex.row(i) += offset;
	}
	int new_data_index = control_vertex.rows();
	this->control_vertex.conservativeResize(control_vertex.rows() + appending_vertex.rows(), 3);
	this->control_normal.conservativeResize(control_normal.rows() + appending_vertex.rows(), 3);
	control_vertex.block(new_data_index, 0, appending_vertex.rows(), 3) = appending_vertex;
	control_normal.block(new_data_index, 0, appending_normal.rows(), 3) = appending_normal;
}

void Implicit::reset_data(Eigen::MatrixXd new_vertex, Eigen::MatrixXd new_normal) {
	assert(new_vertex.rows() == new_normal.rows());
	control_vertex.conservativeResize(new_vertex.rows(), 3);
	control_normal.conservativeResize(new_vertex.rows(), 3);
	control_vertex = new_vertex;
	control_normal = new_normal;
}

void Implicit::print_vertices() {
	std::cout << control_vertex << std::endl;
}

void Implicit::create_mesh(int resolution, const Eigen::MatrixX3d& bounds, Object& object) {
	// Solve linear system;

	//std::cout << N;
	//// Plot the mesh

	hrbf.fit(control_vertex, control_normal);
	fitted = true;

	igl::voxel_grid(bounds, 0, resolution, 0, GV, res);

	Eigen::VectorXd S;
	S.conservativeResize(GV.rows());
	evals.conservativeResize(GV.rows());
	for (int i = 0; i < GV.rows(); i++)
	{
		S(i) = eval(GV.row(i));
		evals(i) = S(i) > 0? 1: S(i) < 0? -1:0;
	}
	Eigen::MatrixX3d vertex;
	Eigen::MatrixX3i face;
	igl::marching_cubes(S, GV, res(0), res(1), res(2), 0, vertex, face);
	object.setVertices(vertex);
	object.setFaces(face);
}

void Implicit::print_num(int i)
{
	std::cout << i << '\n';
}

Eigen::RowVector3i Implicit::getRes() const{
	return res;
}

double Implicit::eval(const Eigen::RowVector3d point) const {
	assert(fitted);
	return hrbf.eval(point);
}

Eigen::RowVector3d Implicit::grad(const Eigen::RowVector3d point) const {
	assert(fitted);
	return hrbf.grad(point);
}

Eigen::VectorXd Implicit::getEvals() const{
	assert(fitted);
	return evals;
}

Eigen::MatrixX3d Implicit::getGrid() const{
	assert(fitted);
	return GV;
}
