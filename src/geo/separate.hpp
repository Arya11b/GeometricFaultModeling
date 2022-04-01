#ifndef SEPARATE_H
#define SEPARATE_H

#include "../object.hpp"
#include <Eigen/Dense>


namespace geo{
    void inline separate(
        const Object& O,
        const Eigen::VectorXi& label,
        Object &O1,
        Object &O2
    ){
        const auto& V = O.getVertices();
        const auto& F = O.getFaces();
        
        Eigen::MatrixX3d V1_output, V2_output;
        Eigen::MatrixX3i F1_output, F2_output;

        F1_output.conservativeResize(F.rows(),3);
        F2_output.conservativeResize(F.rows(),3);

        int j=0, k=0;
        for (int i=0; i < F.rows(); i++){
            if (label(i) == 1){
                F2_output.row(j++) = F.row(i);
            }
            else{
                F1_output.row(k++) = F.row(i);
            }
        }
        F1_output.conservativeResize(k,Eigen::NoChange);
        F2_output.conservativeResize(j,Eigen::NoChange);
        V1_output.conservativeResize(V.rows(),Eigen::NoChange);
        V2_output.conservativeResize(V.rows(),Eigen::NoChange);

        auto set_vertex_indices = [&V](Eigen::MatrixX3i& Fo, Eigen::MatrixX3d& Vo){
            std::unordered_map<int,int> vmap;
            int newvsize = 0;
            for(int i =0; i<Fo.rows();i++){
                for(int j=0; j<Fo.cols()/*3*/;j++){
                    if(!vmap.count(Fo(i,j))){
                        Vo.row(newvsize) = V.row(Fo(i,j));
                        vmap[Fo(i,j)] = newvsize++;
                    }
                    Fo(i,j) = vmap[Fo(i,j)];
                }
            }
            Vo.conservativeResize(newvsize,Eigen::NoChange);
        };
        
        set_vertex_indices(F1_output,V1_output);
        set_vertex_indices(F2_output,V2_output);

        O1.init(V1_output,F1_output);
        O2.init(V2_output,F2_output);

    }
}

#endif