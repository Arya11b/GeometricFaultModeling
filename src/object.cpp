#include "object.hpp"

Object::Object() : Drawable() {}

Object::Object(const Object& obj):
	vertex(obj.vertex),
	face(obj.face),
	pos(Eigen::RowVector3d().Zero()),
	rotation(Eigen::RowVector3d().Zero()){}

Object& Object::operator=(const Object& obj){
	vertex = obj.vertex;
	face = obj.face;
	pos = Eigen::RowVector3d().Zero();
	rotation =Eigen::RowVector3d().Zero();
	return *this;
	}

Object::Object(Object&& obj):
	vertex(std::move(obj.vertex)),
	face(std::move(obj.face)),
	pos(obj.pos),
	rotation(obj.rotation){}

Object& Object::operator=(Object&& obj){
	vertex = std::move(obj.vertex);
	face = std::move(obj.face);
	pos = std::move(obj.pos);
	rotation =std::move(obj.rotation);
	return *this;
}

void Object::init(Eigen::MatrixX3d vertex, Eigen::MatrixX3i face, Eigen::RowVector3d pos) {
	this->vertex = vertex;
	this->face = face;
	this->pos = pos;
}

Eigen::RowVector3d Object::position() {
	return pos;
}

void Object::setPosition(Eigen::RowVector3d pos) {
	this->pos = pos;
}

Eigen::RowVector3d Object::getVertex(int i) const{
	assert(i < vertex.rows());
	return vertex.row(i) + pos;
}

Eigen::MatrixX3d Object::getVertices() const{
	return vertex;
}

Object Object::get() const{
	auto V = vertex;
	V += pos.replicate(vertex.rows(),1);
	Object o;
	o.init(V,face);
	return o;
}


Eigen::MatrixX3i Object::getFaces() const{
	return face;
}


void Object::setVertices(Eigen::MatrixX3d& vertex) {
	this->vertex.conservativeResizeLike(vertex);
	this->vertex = vertex;
}

void Object::setFaces(Eigen::MatrixX3i& face) {
	this->face.conservativeResizeLike(face);
	this->face = face;
}

void Object::render(igl::opengl::glfw::Viewer* viewer) {
	Eigen::RowVector3d col = Eigen::RowVector3d(.2f,.2f,.2f);
	viewer->data(vid).clear();
	viewer->data(vid).show_lines = false;
	viewer->data(vid).line_width = 1.f;

	viewer->data(vid).set_mesh(vertex + pos.replicate(vertex.rows(),1), face);
	viewer->data(vid).set_colors(col);
	//viewer->data(vid).add_points(vertex,Eigen::RowVector3d(.0f,1.f,0.f));
}

void Object::clear()
{
	vertex.resize(0, 3);
	face.resize(0, 3);
}
