#include "fault.hpp"
Fault::Fault() :Drawable() {}
Fault::Fault(const Fault& f){
    bounds = f.bounds;
    ufcurves = f.ufcurves;
    bfcurves = f.bfcurves;
    ifcurves = f.ifcurves;
    implicit = f.implicit;
    object = f.object;
}
Fault& Fault::operator=(const Fault& f){
    bounds = f.bounds;
    ufcurves = f.ufcurves;
    bfcurves = f.bfcurves;
    ifcurves = f.ifcurves;
    implicit = f.implicit;
    object = f.object;
    return *this;
}
void Fault::setBounds(const Eigen::MatrixX3d& bounds) {
    this->bounds = bounds;
}

    Fault::Fault(Fault&& f) = default;

    Fault::~Fault() = default;

//change to move 

void Fault::addUFCurve(FCurve& fcurve) {
    ufcurves.push_back(fcurve);
}
void Fault::addIFCurve(FCurve& fcurve) {
    ifcurves.push_back(fcurve);
}
void Fault::addBFCurve(FCurve& fcurve) {
    bfcurves.push_back(fcurve);
}

void Fault::addBoundaryCurvesLike(const FCurve& fcurve) {
    //fcurve.fc_normal.
}

// returns the intersection curve of the fault with the given fault.
FCurve Fault::intersectionFCurve(Fault& fault)
{
    Eigen::MatrixX3d Vs[2];
    Eigen::MatrixX3i Fs[2];
    Vs[0] = getGeometry().getVertices();
    Vs[1] = fault.getGeometry().getVertices();
    Fs[0] = getGeometry().getFaces();
    Fs[1] = fault.getGeometry().getFaces();
    Eigen::MatrixX3d VC;
    Eigen::MatrixX3i FC;
    //igl::combine(Vs, Fs,VC,FC);
    //The second step is to remesh the combination


    //And then "curves" has a bunch of stuff that I don't know what to do with.
    return FCurve();
}

void Fault::render(igl::opengl::glfw::Viewer* viewer) {
    Eigen::RowVector3d col;
    col(0) = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    col(1) = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    col(2) = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    viewer->data(vid).clear();
    viewer->data(vid).show_lines = false;
    viewer->data(vid).double_sided = true;

    //viewer->data().add_points(GV2l, Eigen::RowVector3d(1.f, 1.f, 1.f));
	viewer->data(vid).set_mesh(object.getVertices() + object.position().replicate(object.getVertices().rows(),1), object.getFaces());
	viewer->data(vid).set_colors(col);

    viewer->data(vid).point_size = 10;

    //viewer->data().set_colors(color);
    Eigen::RowVector3d ucol(1.f, 0.f, .2f);
    Eigen::RowVector3d bcol(.3f, 0.f, 1.f);
    Eigen::RowVector3d icol(0.f,1.f,.2f);
    for (auto& fc : ufcurves)
    {
        fc.setVid(vid);
        fc.render(viewer);
    }
    for (auto& fc : bfcurves)
    {
        fc.setVid(vid);
        fc.render(viewer);
    }
    for (auto& fc : ifcurves)
    {
        fc.setVid(vid);
        fc.render(viewer);
    }
}


int Fault::size() {
    return ufcurves.size();
}

void Fault::interpolate() {
    implicit.clear();
    for (int i = 0; i < ufcurves.size(); i++)
    {
        implicit.add_data(ufcurves[i].vertex, ufcurves[i].normal);
    }
    implicit.create_mesh(50, bounds,object);
}

Implicit Fault::getImplicit() {
    return implicit;
}

Object Fault::getGeometry()
{
    return object;
}

void Fault::updateGeometry(Object&& object){
    this->object = object;
}


int Fault::UFCurveSize(){ return ufcurves.size();}
int Fault::IFCurveSize(){ return ifcurves.size();}
int Fault::BFCurveSize(){ return bfcurves.size();}
FCurve Fault::UFCurveAt(int i){
    assert(i<ufcurves.size());
    return ufcurves.at(i);
}
FCurve Fault::IFCurveAt(int i){
    assert(i<ifcurves.size());
    return ifcurves.at(i);
}
FCurve Fault::BFCurveAt(int i){
    assert(i<bfcurves.size());
    return bfcurves.at(i);
}
