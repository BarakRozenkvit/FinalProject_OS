//
// Created by Barak Rozenkvit on 04/12/2024.
//

#ifndef FINALPROJECT_OS_TREE_HPP
#define FINALPROJECT_OS_TREE_HPP

#include "Graph.hpp"

using namespace std;

class Tree: public Graph{

public:
    Tree() = default;
    int getTotalWeight();

    int longestDistance();

    int avarageDistance();

    int shortestDistance();

};


#endif //FINALPROJECT_OS_TREE_HPP
