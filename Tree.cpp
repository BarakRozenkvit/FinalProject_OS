//
// Created by Barak Rozenkvit on 04/12/2024.
//

#include "Tree.hpp"
#include "Algo.hpp"
#include "Graph.hpp"
#include <stdexcept> // For standard exception types


int Tree::calculateDistances() {
    _distances = DistanceAlgo::FloyedMarshal(*this);
    _calculated = true;
}

int Tree::getTotalWeight() {
    if (!_calculated){
        throw std::runtime_error("MST Not calculated");
    }
    int weight = 0;
    for(int i=0;i<vertexNum();i++){
        for(int j=0;j<i;j++){
            weight += at(i,j);
        }
    }
    return weight;
}

int Tree::longestDistance() {
    if (!_calculated){
        throw std::runtime_error("MST Not calculated");
    }
    int d=0;
    for(int i=0;i<vertexNum();i++){
        for(int j=0;j<i;j++){
            if (at(i,j) > d){
                d= at(i,j);
            }
        }
    }
    return d;
}

int Tree::shortestDistance() {
    if (!_calculated){
        throw std::runtime_error("MST Not calculated");
    }
    int d=INT_MAX;
    for(int i=0;i<vertexNum();i++){
        for(int j=0;j<i;j++){
            if (at(i,j) > 0 && at(i,j) < d){
                d= at(i,j);
            }
        }
    }
    return d;

}

double Tree::avarageDistance() {
    int d=0;
    for(int i=0;i<vertexNum();i++){
        for(int j=0;j<i;j++){
            d += at(i,j);
        }
    }
    return (double)d/edgeNum();
}
