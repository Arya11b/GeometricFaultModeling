#ifndef EIGEN_TO_CURVEN
#define EIGEN_TO_CURVEN
#include <CurveN.hpp>
#include <Eigen/Dense>

// possible improvement: generalize to size n
namespace conversions{
    void inline eigen_to_curveN(
        const Eigen::MatrixX3d& mat,
        Curve3D& curve

    ){
        for (int i = 0; i < mat.rows(); i++)
        {
            curve.add(Point3D(mat(i,0),mat(i,1),mat(i,2)));
        }
        
    }

    void inline curveN_to_eigen(
        const Curve3D& curve,
        Eigen::MatrixX3d& mat

    ){
        mat.conservativeResize(curve.size(),3);
        for (int i = 0; i < curve.size(); i++)
        {
            mat(i,0) = curve.at(i).x();
            mat(i,1) = curve.at(i).y();
            mat(i,2) = curve.at(i).z();
        }
        
    }
}
#endif