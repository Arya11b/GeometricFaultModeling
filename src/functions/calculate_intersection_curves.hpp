// This file is derived from the libigl library
#ifndef CALCULATE_INTERSECTION_CURVES_H
#define CALCULATE_INTERSECTION_CURVES_H

#include "../object.hpp"
#include "../fcurve.hpp"
#include "../geo/extract_non_manifold_edge_curves.hpp"
#include "../geo/reorder_curve_edgelist.hpp"
#include <igl/combine.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/unique_edge_map.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <algorithm>
#include <CurveN.hpp>
namespace functions {
    // Calculates the intesection curve of two given planar objects
    //
    //
    // Inputs:
    //   O1,    The first intersection object
    //   O2,    The second intersection object
    //
    // Output:
    //   O1,    The first intersection object with the projected curves
    //   O2,    The second intersection object with the projected curves
    //   intersectionCurves: vector of intersection curves
    //   boundaryCurves: vector of boundary curves

    void inline calculate_curve_intersections(
        // inputs
        const Object& O1,
        const Object& O2,
        // outputs
        Object& O1_output,
        Object& O2_output,
        std::vector<FCurve>& intersectionCurves,
        std::vector<FCurve>& boundaryCurves
    ) {
        intersectionCurves.clear();

        Eigen::MatrixX3d V;
        Eigen::MatrixX3i F;
        //std::vector<Eigen::MatrixX3d> pointCol;
        //std::vector<Eigen::MatrixX3i> faceCol;
        const auto& V1 = O1.getVertices();
        const auto& V2 = O2.getVertices();
        const auto& F1 = O1.getFaces();
        const auto& F2 = O2.getFaces();

        // refractor this parts
        // this approach leads to copying the content of the two matrices
        // the reason this approach is used is that emplacing in vector will result in\
        // std::bad_alloc error
        std::vector<Eigen::MatrixX3d> Vtemp;
        std::vector<Eigen::MatrixX3i> Ftemp;
        Vtemp.emplace_back(V1);
        Vtemp.emplace_back(V2);
        Ftemp.emplace_back(F1);
        Ftemp.emplace_back(F2);
        igl::combine(Vtemp, Ftemp, V, F);

        Eigen::MatrixXd VFinal;
        Eigen::MatrixXi FFinal;
        Eigen::MatrixXi ifintersection;
        Eigen::VectorXi btflist, vvlist;


        igl::copyleft::cgal::remesh_self_intersections(
            V, F,
            igl::copyleft::cgal::RemeshSelfIntersectionsParam(false, false, true),
            VFinal, FFinal,
            ifintersection, btflist, vvlist);
        if(ifintersection.rows() == 0 ){
            O1_output = O1;
            O2_output = O2;
            return;
        }

        // seperate output meshes
        Eigen::MatrixX3d V1_output, V2_output;
        Eigen::MatrixX3i F1_output, F2_output;

        F1_output.conservativeResize(FFinal.rows(),3);
        F2_output.conservativeResize(FFinal.rows(),3);

        // std::cout << "ifint:\n" << ifintersection << std::endl;
        // std::cout << "bflist:\n" << btflist << std::endl;
        // std::cout << "vvlist:\n" << vvlist << std::endl;
        int j=0, k=0;
        for (int i=0; i < FFinal.rows(); i++){
            if (btflist(i) >= F1.rows()){
                F2_output.row(j++) = FFinal.row(i);
            }
            else{
                F1_output.row(k++) = FFinal.row(i);
            }
        }
        F1_output.conservativeResize(k,Eigen::NoChange);
        F2_output.conservativeResize(j,Eigen::NoChange);
        V1_output.conservativeResize(VFinal.rows(),Eigen::NoChange);
        V2_output.conservativeResize(VFinal.rows(),Eigen::NoChange);

        auto set_vertex_indices = [&VFinal](Eigen::MatrixX3i& F, Eigen::MatrixX3d& V){
            std::unordered_map<int,int> vmap;
            int newvsize = 0;
            for(int i =0; i<F.rows();i++){
                for(int j=0; j<F.cols()/*3*/;j++){
                    if(!vmap.count(F(i,j))){
                        V.row(newvsize) = VFinal.row(F(i,j));
                        vmap[F(i,j)] = newvsize++;
                    }
                    F(i,j) = vmap[F(i,j)];
                }
            }
            V.conservativeResize(newvsize,Eigen::NoChange);
        };
        
        set_vertex_indices(F1_output,V1_output);
        set_vertex_indices(F2_output,V2_output);
        //std::cout << "F1O:\n" << F1_output << std::endl;
        //std::cout << "F2O:\n" << F2_output << std::endl;
        //std::cout << "V1O:\n" << V1_output << std::endl;
        //std::cout << "V2O:\n" << V2_output << std::endl;
        O1_output.init(V1_output,F1_output);
        O2_output.init(V2_output,F2_output);

        
              


        //Get the edges for the combined and remeshed faces
        Eigen::MatrixXi E, uE;
        Eigen::MatrixXi EMAP;
        std::vector<std::vector<size_t>> uE2E;
        igl::unique_edge_map(FFinal, E, uE, EMAP, uE2E);

        //Actually get the collection of intersecting edges
        std::vector<std::pair<std::vector<size_t>, int>> curve_col;

        geo::extract_non_manifold_edge_curves(FFinal, EMAP, uE2E, curve_col);

        for (int i = 0; i < curve_col.size(); i++)
        {
            //if (curve_col.at(i).second == 1) continue;
            const auto& csize = curve_col.at(i).first.size();
            std::vector<int> curve_indices;

            // add intersecting edgecurves to matrix:
            Eigen::MatrixX2i unordered_edgelists;
            unordered_edgelists.conservativeResize(csize,Eigen::NoChange);
            for (int j = 0; j < csize; j++)
            {
                unordered_edgelists(j,0) = uE(curve_col.at(i).first.at(j), 0);
                unordered_edgelists(j,1) = uE(curve_col.at(i).first.at(j), 1);
            }
            geo::reorder_curve_edgelist(unordered_edgelists,false,curve_indices);
            Eigen::MatrixX3d verts;
            verts.conservativeResize(curve_indices.size(),Eigen::NoChange);
            for (int i=0;i<verts.rows();i++)
            {
                verts.row(i) = VFinal.row(curve_indices.at(i));
            }
            

            //std::cout << std::endl;
            FCurve fc;
            fc.init(verts);
            if (curve_col.at(i).second == 1)
                boundaryCurves.emplace_back(fc);
            else if (curve_col.at(i).second > 2)
                intersectionCurves.emplace_back(fc);
        }

    }

    
    void inline calculate_curve_intersections(
        // inputs
        const Object& O1,
        const Object& O2,
        // outputs
        std::vector<Curve2D>& curve
    ) {
        curve.clear();
        Eigen::MatrixX3d V;
        Eigen::MatrixX3i F;
        //std::vector<Eigen::MatrixX3d> pointCol;
        //std::vector<Eigen::MatrixX3i> faceCol;
        const auto& V1 = O1.getVertices();
        const auto& V2 = O2.getVertices();
        const auto& F1 = O1.getFaces();
        const auto& F2 = O2.getFaces();

        // refractor this parts
        // this approach leads to copying the content of the two matrices
        // the reason this approach is used is that emplacing in vector will result in\
        // std::bad_alloc error
        std::vector<Eigen::MatrixX3d> Vtemp;
        std::vector<Eigen::MatrixX3i> Ftemp;
        Vtemp.emplace_back(V1);
        Vtemp.emplace_back(V2);
        Ftemp.emplace_back(F1);
        Ftemp.emplace_back(F2);
        igl::combine(Vtemp, Ftemp, V, F);

        Eigen::MatrixXd VFinal;
        Eigen::MatrixXi FFinal;
        Eigen::MatrixXi ifintersection;
        Eigen::VectorXi btflist, vvlist;


        igl::copyleft::cgal::remesh_self_intersections(
            V, F,
            igl::copyleft::cgal::RemeshSelfIntersectionsParam(false, false, true),
            VFinal, FFinal,
            ifintersection, btflist, vvlist);

        Eigen::MatrixXi E, uE;
        Eigen::MatrixXi EMAP;
        std::vector<std::vector<size_t>> uE2E;
        igl::unique_edge_map(FFinal, E, uE, EMAP, uE2E);

        //Actually get the collection of intersecting edges
        std::vector<std::pair<std::vector<size_t>, int>> curve_col;

        geo::extract_non_manifold_edge_curves(FFinal, EMAP, uE2E, curve_col);

        for (int i = 0; i < curve_col.size(); i++)
        {
            if (curve_col.at(i).second == 1) continue;
            const auto& csize = curve_col.at(i).first.size();
            std::vector<int> curve_indices;

            // add intersecting edgecurves to matrix:
            Eigen::MatrixX2i unordered_edgelists;
            unordered_edgelists.conservativeResize(csize,Eigen::NoChange);
            for (int j = 0; j < csize; j++)
            {
                unordered_edgelists(j,0) = uE(curve_col.at(i).first.at(j), 0);
                unordered_edgelists(j,1) = uE(curve_col.at(i).first.at(j), 1);
            }
            geo::reorder_curve_edgelist(unordered_edgelists,false,curve_indices);
            Eigen::MatrixX3d verts;
            verts.conservativeResize(curve_indices.size(),Eigen::NoChange);
            for (int i=0;i<verts.rows();i++)
            {
                verts.row(i) = VFinal.row(curve_indices.at(i));
            }
            

            //std::cout << std::endl;
            Curve2D c;
            for (size_t i = 0; i < verts.rows(); i++){
                Point2D p(verts(i,0), verts(i,1));
                c.push_back(p);
            }
            curve.emplace_back(c);
            
        }

    }
}


#endif
