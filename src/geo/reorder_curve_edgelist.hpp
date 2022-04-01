#ifndef REORDER_CURVE_EDGELIST
#define REORDER_CURVE_EDGELIST

#include <unordered_map>
#include <algorithm>
#include <set>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace geo{

    // orients the unsorted curve indices
    // input:
    //  a S#2 int matrix (the edge list)
    //  a bool indicating whether the edge list is directed (1,2),(2,3) is directed while (1,2),(3,2) is not
    // output
    //  a S#1 int matrix with sorted indices
    //
    //      assumes no region of the input is isolated
    //      assumes the input is not self intersecting
    //      assumes the input is not closed
    //
    // example: 
    //  input:
    //      1,2 | 3,1 | 2,4
    //  output:
    //      3, 2 ,4

    void inline reorder_curve_edgelist(
        const Eigen::Matrix<int, Eigen::Dynamic,2>& unordered_edgelist,
        bool isDirected,
        std::vector<int>& ordered_edgelist
    ){
        std::unordered_map<int,std::set<int>> edge_checklist;
        int vert_size;
        for (int i = 0; i < unordered_edgelist.rows(); i++){
            edge_checklist[unordered_edgelist(i,0)].insert(unordered_edgelist(i,1));
            if(!isDirected)
                edge_checklist[unordered_edgelist(i,1)].insert(unordered_edgelist(i,0));
        }
        auto first = [&]()->int{
            for (auto it = edge_checklist.begin(); it != edge_checklist.end(); it++){
                if (it->second.size() == 1){
                    return it->first;
                }
            }
            return -1;
        }();
        auto current = first;
        for (int i = 0; i < edge_checklist.size(); i++)
        {
            ordered_edgelist.emplace_back(current);
            auto it = edge_checklist[current].begin();
            while(std::find(ordered_edgelist.begin(),ordered_edgelist.end(),*it) != ordered_edgelist.end()){
                if(edge_checklist[current].size() == 1) break;
                if(it++ ==  edge_checklist[current].end()) 
                    throw std::invalid_argument("this is dead wrong");
            }
            current = *it;
        }
         
        
    }
}
#endif