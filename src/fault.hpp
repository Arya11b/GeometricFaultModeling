#ifndef FAULT_H
#define FAULT_H

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include "fcurve.hpp"
#include "implicit.hpp"
#include "drawable.hpp"
#include <CurveN.hpp>

class Fault: public Drawable {
private:
    // user-defined curves
    std::vector<FCurve> ufcurves;
    // intersection curves
    std::vector<FCurve> ifcurves;
    // boundary curves
    std::vector<FCurve> bfcurves;
    //refractor
    Implicit implicit;
    Eigen::MatrixX3d bounds;
    Object object;

    void render(igl::opengl::glfw::Viewer* viewer) override;



public:
	

    Fault();
    Fault(const Fault& f);
    Fault& operator=(const Fault& f);
    Fault(Fault&& f);
    //Fault& operator=(Fault&& f);
    ~Fault();

    void setBounds(const Eigen::MatrixX3d& bounds);

    void addUFCurve(FCurve& fcurve);
    void addIFCurve(FCurve& fcurve);
    void addBFCurve(FCurve& fcurve);

    int UFCurveSize();
    int IFCurveSize();
    int BFCurveSize();

    FCurve UFCurveAt(int i);
    FCurve IFCurveAt(int i);
    FCurve BFCurveAt(int i);

    void addBoundaryCurvesLike(const FCurve& fcurve);
    FCurve intersectionFCurve(Fault& fault);

    int size();

    void interpolate();

    // returns the implicit surface
    Implicit getImplicit();
    
    // returns the fault mesh, explicit geometry
    Object getGeometry();
    // updates the fault geometry (explicit), use with caution. It does NOT change the interpolant.
    void updateGeometry(Object&& object);
};

#endif