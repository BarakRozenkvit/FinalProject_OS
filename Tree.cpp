//
// Created by Barak Rozenkvit on 04/12/2024.
//

#include "Tree.hpp"
#include "Algo.hpp"
#include "Graph.hpp"
#include <stdexcept> // For standard exception types


void Tree::calculateDistances() {
    _distances = DistanceAlgo::FloyedWarshall(*this);
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

double Tree::averageDistance() {
    // Ensure shortest paths are calculated
    if (!_calculated) {
        calculateDistances(); // Run Floyd-Warshall to update _distances
    }

    double totalDistance = 0; // Total sum of distances
    int count = 0;           // Count of unique pairs

    // Iterate over all unique pairs (i < j)
    for (int i = 0; i < vertexNum(); i++) {
        for (int j = i + 1; j < vertexNum(); j++) {
            if (_distances.at(i, j) != INT_MAX) { // Include valid shortest paths
                totalDistance += _distances.at(i, j);
                count++;
            }
        }
    }

    // Compute and return average distance
    return count > 0 ? (totalDistance / count) : 0;
}