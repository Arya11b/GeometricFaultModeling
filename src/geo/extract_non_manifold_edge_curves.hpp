// This file is derived from the libigl library
#ifndef NON_MANIFOLD_EDGE_CURVES
#define NON_MANIFOLD_EDGE_CURVES

#include <Eigen/Dense>
#include <vector>
#include <algorithm>
#include <cassert>
#include <list>
#include <unordered_map>

namespace geo {
    // Extract non-manifold curves from a given mesh.
    // A non-manifold curves are a set of connected non-manifold edges that
    // does not touch other non-manifold edges except at the end points.
    // They are also maximal in the sense that they cannot be expanded by
    // including more edges.
    //
    // Assumes the input mesh have all self-intersection resolved.  See
    // ``igl::cgal::remesh_self_intersection`` for more details.
    //
    // Inputs:
    //   F  #F by 3 list representing triangles.
    //   EMAP  #F*3 list of indices of unique undirected edges.
    //   uE2E  #uE list of lists of indices into E of coexisting edges.
    //
    // Output:
    //   curves  An array of arries of unique edge indices.
    template<
        typename DerivedF,
        typename DerivedEMAP,
        typename uE2EType>
        void extract_non_manifold_edge_curves(
            const Eigen::MatrixBase<DerivedF>& F,
            const Eigen::MatrixBase<DerivedEMAP>& EMAP,
            const std::vector<std::vector<uE2EType> >& uE2E,
            std::vector<std::pair<std::vector<size_t>, int> >& curves) {
        const size_t num_faces = F.rows();
        assert(F.cols() == 3);
        //typedef std::pair<size_t, size_t> Edge;
        auto edge_index_to_face_index = [&](size_t ei) { return ei % num_faces; };
        auto edge_index_to_corner_index = [&](size_t ei) { return ei / num_faces; };
        auto get_edge_end_points = [&](size_t ei, size_t& s, size_t& d) {
            const size_t fi = edge_index_to_face_index(ei);
            const size_t ci = edge_index_to_corner_index(ei);
            s = F(fi, (ci + 1) % 3);
            d = F(fi, (ci + 2) % 3);
        };

        curves.clear();
        const size_t num_unique_edges = uE2E.size();
        std::unordered_multimap<size_t, size_t> vertex_edge_adjacency;
        std::vector<size_t> non_manifold_edges;
        for (size_t i = 0; i < num_unique_edges; i++) {
            const auto& adj_edges = uE2E[i];
            if (adj_edges.size() == 2) continue;

            const size_t ei = adj_edges[0];
            size_t s, d;
            get_edge_end_points(ei, s, d);
            vertex_edge_adjacency.insert({ {s, i}, {d, i} });
            non_manifold_edges.push_back(i);
            assert(vertex_edge_adjacency.count(s) > 0);
            assert(vertex_edge_adjacency.count(d) > 0);
        }

        auto expand_forward = [&](std::list<size_t>& edge_curve,
            size_t& front_vertex, size_t& end_vertex) {
                while (vertex_edge_adjacency.count(front_vertex) == 2 &&
                    front_vertex != end_vertex) {
                    auto adj_edges = vertex_edge_adjacency.equal_range(front_vertex);
                    for (auto itr = adj_edges.first; itr != adj_edges.second; itr++) {
                        const size_t uei = itr->second;
                        assert(uE2E.at(uei).size() != 2);
                        const size_t ei = uE2E[uei][0];
                        if (uei == edge_curve.back()) continue;
                        size_t s, d;
                        get_edge_end_points(ei, s, d);
                        edge_curve.push_back(uei);
                        if (s == front_vertex) {
                            front_vertex = d;
                        }
                        else if (d == front_vertex) {
                            front_vertex = s;
                        }
                        else {
                            throw "Invalid vertex/edge adjacency!";
                        }
                        break;
                    }
                }
        };

        auto expand_backward = [&](std::list<size_t>& edge_curve,
            size_t& front_vertex, size_t& end_vertex) {
                while (vertex_edge_adjacency.count(front_vertex) == 2 &&
                    front_vertex != end_vertex) {
                    auto adj_edges = vertex_edge_adjacency.equal_range(front_vertex);
                    for (auto itr = adj_edges.first; itr != adj_edges.second; itr++) {
                        const size_t uei = itr->second;
                        assert(uE2E.at(uei).size() != 2);
                        const size_t ei = uE2E[uei][0];
                        if (uei == edge_curve.front()) continue;
                        size_t s, d;
                        get_edge_end_points(ei, s, d);
                        edge_curve.push_front(uei);
                        if (s == front_vertex) {
                            front_vertex = d;
                        }
                        else if (d == front_vertex) {
                            front_vertex = s;
                        }
                        else {
                            throw "Invalid vertex/edge adjcency!";
                        }
                        break;
                    }
                }
        };

        std::vector<bool> visited(num_unique_edges, false);
        for (const size_t i : non_manifold_edges) {
            if (visited[i]) continue;
            std::list<size_t> edge_curve;
            edge_curve.push_back(i);

            const auto& adj_edges = uE2E[i];
            assert(adj_edges.size() != 2);
            const size_t ei = adj_edges[0];
            size_t s, d;
            get_edge_end_points(ei, s, d);

            expand_forward(edge_curve, d, s);
            expand_backward(edge_curve, s, d);
            std::pair < std::vector<size_t>, int >  p;
            for (const auto& e:edge_curve)
            {
                p.first.emplace_back(e);
            }
            p.second = adj_edges.size();
            curves.emplace_back(p);
            for (auto itr = edge_curve.begin(); itr != edge_curve.end(); itr++) {
                visited[*itr] = true;
            }
        }

    }
}


#endif
