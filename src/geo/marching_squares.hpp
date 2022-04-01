#ifndef MARCHING_SQUARES
#define MARCHING_SQUARES

#include <Eigen/Dense>
#include <algorithm>
#include <utility>
#include <iostream>
#include <array>

#include "../implicit.hpp"

#include "reorder_curve_edgelist.hpp"

// This is one messy code
// Don't try to debug this at home

namespace geo{
    enum marching_squares_axis{
        x,y,z
    };
    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1, T2> &pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };
    static void get_2d_grid(
        const Eigen::MatrixX3d& grid_3d,
        int nx, int ny, int nz,
        marching_squares_axis a,
        int index,

         Eigen::MatrixX3d& grid_2d,
         int& nx_o
    ){
        int _nsize, _1size, _2size;
        int _nindex, _1index, _2index;
        switch (a)
        {
        case marching_squares_axis::x:
            _nsize = nx;
            _1size = ny;
            _2size = nz;
            _nindex = 1;
            _1index = nx;
            _2index = nx*ny;
            break;
        case marching_squares_axis::y:
            _nsize = ny;
            _1size = nx;
            _2size = nz;
            _nindex = nx;
            _1index = 1;
            _2index = nx*ny;

            break;
        case marching_squares_axis::z:
            _nsize = nz;
            _1size = nx;
            _2size = ny;
            _nindex = nx*ny;
            _1index = 1;
            _2index = nx;

            break;
        
        default:
            throw("axis should be either x, y or z love");
            break;
        }

        assert(index < _nsize);
        grid_2d.conservativeResize(_1size * _2size, Eigen::NoChange);
        for (size_t i = 0; i < _2size; i++){
            for (size_t j = 0; j < _1size; j++){
                grid_2d.row(j + i*_1size) = grid_3d.row(index * _nindex + j * _1index + i *_2index);
            }
        }
        nx_o = _1size;
    }

    void marching_squares(
        marching_squares_axis an,
        Eigen::RowVector3d boxbegin,
        Eigen::RowVector3d boxend,
        double dn,
        const Implicit& implicit,
        
        Eigen::Matrix<double,Eigen::Dynamic,2> curve

    ){
        marching_squares_axis a1, a2;
        a1 = marching_squares_axis::x;
        a2 = marching_squares_axis::y;
        if(an == marching_squares_axis::x){
            a1 = marching_squares_axis::y;
            a2 = marching_squares_axis::z;
        }
        if(an == marching_squares_axis::y){
            a1 = marching_squares_axis::x;
            a2 = marching_squares_axis::z;
        }


        int resIndex = [&]()->int{
            double _nl = (boxend - boxbegin)(an);
            double _dn = dn - boxbegin(an);
            int index = (int) (_dn / (_nl/implicit.getRes()(an)));
            return index;
        }();

        int nx = implicit.getRes()(0);
        int ny = implicit.getRes()(1);
        int nz = implicit.getRes()(2);

        auto evalgrid = implicit.getEvals();
        auto grid_3d = implicit.getGrid();

        Eigen::MatrixX3d grid_2d;
        int nx_2d;
        get_2d_grid(grid_3d,nx,ny,nz,an,resIndex,grid_2d,nx_2d);

        int vertIndex=0;
        Eigen::MatrixX3d msq;
        std::unordered_map<std::pair<int,int>,int,pair_hash> edgemap;
        msq.conservativeResize(grid_2d.rows(),Eigen::NoChange);

        // edge index
        int ei = 0;
        Eigen::MatrixX2i edgematrix;
        edgematrix.conservativeResize(grid_2d.rows(),Eigen::NoChange);

        for(size_t i=0; i<grid_2d.rows();i++){
            if((i+1)%nx_2d ==0) continue;
            if((i + nx_2d) >= grid_2d.rows()) continue;
            std::array<int,4> square;
            square[0] = i;
            square[1] = i+1;
            square[2] = i+nx_2d;
            square[3] = (i+1)+nx_2d;
            int scase =0;
            for (int j = 0; j < square.size(); j++){
                if(evalgrid(square[j])>0) scase += (1<<j);
            }
            if(scase == 0 || scase == 15) continue;
            int i1 = 0, i2 = 0;
            int i3 = 0, i4 = 0;
            switch(scase){
                case 1:
                    i1 = i+nx_2d,i2 =i;
                    i3 = i, i4= i+1;
                    break;
                case 2:
                    i1 = i,i2 =i+1;
                    i3 = i+1, i4= i+1+nx_2d;
                    break;
                case 3:
                    i1 = i+nx_2d,i2 =i;
                    i3 = i+1, i4= i+1+nx_2d;
                    break;
                case 4:
                    i1 = i+1,i2 =i+1+nx_2d;
                    i3 = i+nx_2d+1, i4= i+nx_2d;
                    break;
                case 5:
                    i1 = i+nx_2d,i2 =i;
                    i3 = i, i4= i+1;
                    if(!edgemap.count({i1,i2})){
                        edgemap[{i1,i2}] = vertIndex;
                        msq.row(vertIndex++) = (grid_2d.row(i1) + grid_2d.row(i2))/2;
                    }
                    if(!edgemap.count({i3,i4})){
                        edgemap[{i3,i4}] = vertIndex;
                        msq.row(vertIndex++) = (grid_2d.row(i3) + grid_2d.row(i4))/2;
                    }
                    i1 = i+1,i2 =i+1+nx_2d;
                    i3 = i+nx_2d+1, i4= i+nx_2d;
                    edgematrix(ei,0) = edgemap[{i1,i2}];
                    edgematrix(ei++,1) = edgemap[{i3,i4}];
                    break;
                case 6:
                    i1 = i,i2 =i+1;
                    i3 = i+1, i4= i+nx_2d+1;
                    break;
                case 7:
                    i1 = i+nx_2d,i2 =i;
                    i3 = i+nx_2d+1, i4= i+nx_2d;
                    break;
                case 8:
                    i3 = i+nx_2d,i4 =i;
                    i1 = i+nx_2d+1, i2= i+nx_2d;
                    break;
                case 9:
                    i3 = i,i4 =i+1;
                    i1 = i+1, i2= i+nx_2d+1;
                    break;
                case 10:
                    i3 = i+1,i1 =i+1+nx_2d;
                    i1 = i+nx_2d+1, i2= i+nx_2d;
                    if(!edgemap.count({i1,i2})){
                        edgemap[{i1,i2}] = vertIndex;
                        msq.row(vertIndex++) = (grid_2d.row(i1) + grid_2d.row(i2))/2;
                    }
                    if(!edgemap.count({i3,i4})){
                        edgemap[{i3,i4}] = vertIndex;
                        msq.row(vertIndex++) = (grid_2d.row(i3) + grid_2d.row(i4))/2;
                    }
                    i3 = i+nx_2d,i4 =i;
                    i1 = i, i2= i+1;
                    
                    edgematrix(ei,0) = edgemap[{i1,i2}];
                    edgematrix(ei++,1) = edgemap[{i3,i4}];
                    break;
                case 11:
                    i3 = i+1,i1 =i+1+nx_2d;
                    i1 = i+nx_2d+1, i2= i+nx_2d;
                    break;
                case 12:
                    i3 = i+nx_2d,i4 =i;
                    i1 = i+1, i2= i+1+nx_2d;
                    break;
                case 13:
                    i3 = i,i4 =i+1;
                    i1 = i+1, i2= i+1+nx_2d;
                    break;
                case 14:
                    i3 = i+nx_2d,i4 =i;
                    i1 = i, i2= i+1;
                    break;
            }
            if(!edgemap.count({i1,i2})){
                edgemap[{i1,i2}] = vertIndex;
                msq.row(vertIndex++) = (grid_2d.row(i1) + grid_2d.row(i2))/2;
            }
            if(!edgemap.count({i3,i4})){
                edgemap[{i3,i4}] = vertIndex;
                msq.row(vertIndex++) = (grid_2d.row(i3) + grid_2d.row(i4))/2;
            }
            edgematrix(ei,0) = edgemap[{i1,i2}];
            edgematrix(ei++,1) = edgemap[{i3,i4}];
        }
        edgematrix.conservativeResize(ei,Eigen::NoChange);
        msq.conservativeResize(vertIndex,Eigen::NoChange);
        
        std::cout << edgematrix <<std::endl;


        curve.conservativeResize(edgematrix.rows(),Eigen::NoChange);
        for(size_t i=0; i<curve.rows(); i++){
            curve(i,0)= msq(edgematrix(i,0),a1);
            curve(i,1)= msq(edgematrix(i,0),a2);
        }
        curve.rowwise().normalize();
        curve.array() += 1;
        std::cout << curve << std::endl;
    }
}

#endif