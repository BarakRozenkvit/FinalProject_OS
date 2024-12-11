//
// Created by Barak Rozenkvit on 04/12/2024.
//

#ifndef FINALPROJECT_OS_TREE_HPP
#define FINALPROJECT_OS_TREE_HPP

#include "Graph.hpp"
#include <climits>
#pragma once

using namespace std;

class Tree: public Graph{

    Graph _distances;
    bool _calculated = false;

public:
    Tree()=default;

    int getTotalWeight();

    int longestDistance();

    double averageDistance();

    int shortestDistance();

    void calculateDistances();

};


#endif //FINALPROJECT_OS_TREE_HPP
