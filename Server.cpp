//
// Created by Barak Rozenkvit on 04/12/2024.

#include "MSTAlgo.hpp"

int main(){

    Graph graph;
    graph.newGraph(5);
    /**
     *  0, 1, 3, 11, 0,
        1, 0, 0, 0, 11,
        3, 0, 0, 4, 1,
        11, 0, 4, 0, 0,
        0, 11, 1, 0, 0,
     */
    graph.addEdge(0,1,1);
    graph.addEdge(0,2,3);
    graph.addEdge(0,3,11);
    graph.addEdge(1,4,11);
    graph.addEdge(2,3,4);
    graph.addEdge(4,2,1);

    Tree d = MSTAlgo::Prim(graph);
    int f = d.getTotalWeight();





}