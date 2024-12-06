//
// Created by Barak Rozenkvit on 04/12/2024.
//

#include "Tree.hpp"


int Tree::getTotalWeight() {
    int weight = 0;
    for(int i=0;i<vertexNum();i++){
        for(int j=0;j<i;j++){
            weight += at(i,j);
        }
    }
    return weight;
}
