#ifndef SEPARATE_OBJECT_BY_INTERPOLANT
#define SEPARATE_OBJECT_BY_INTERPOLANT
#include "../object.hpp"
#include "../fcurve.hpp"
#include "../implicit.hpp"
#include "../geo/separate.hpp"

#include <omp.h>
#include <ctime>

namespace functions{
    // separates object by interpolant and curve
    // inputs:
    //      O: object to be divided into two parts
    //      I: implicit surface, used to evaluate Object position
    // outputs:
    //      O1: output object 1
    //      O2: output object 2
    // [TODO] change O1,O2 to object vector
    void inline separate_object_by_interpolant(
        const Object& O,
        const Implicit& I,
        Object& O1,
        Object& O2
    ){
        const auto& V = O.getVertices();
        const auto& F = O.getFaces();
        Eigen::VectorXi label;
        label.conservativeResize(F.rows(),Eigen::NoChange);

        // [TODO] parallelize
        std::clock_t start;
        double duration;

        start = std::clock();
        #pragma omp parallel for
        for(int i=0; i<F.rows();i++){
            Eigen::RowVector3d face_center = Eigen::RowVector3d::Zero();
            for(int j=0;j<F.cols();j++){
                face_center += V.row(F(i,j));
            }
            face_center /= F.cols();
            if(I.eval(face_center) > 0){
                label(i) = 1;
            }
            else {
                label(i) = -1;
            }
        }
        
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"duration: "<< duration <<'\n';

        geo::separate(O,label,O1,O2);
    }
}

#endif