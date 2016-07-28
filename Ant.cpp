#include "Ant.h"
#include "Colony.h"
#include <iostream>
#define alpha 0.8
#define beta 0.2
#include <math.h>

Ant::Ant(){
    this->objectiveFunction = 0;

}


std::vector<int>& Ant::getTour(){
    return this->tour;
}

void Ant::setObjectiveFunction(double val){
    this->objectiveFunction = val;
}


Ant::~Ant()
{

}
double Ant::getObjectiveFunction()
{
    return this->objectiveFunction;
}

void Ant::setPath(std::vector<int> path){
    this->tour = path;
}

void Ant::setTour(std::vector<int> tour)
{
    this->tour = tour;
}

std::vector<std::vector<int>> Ant::breakTrips(int hotel_numbers){
    std::vector<std::vector<int>> matrixTrips;
    std::vector<int> trip;
    std::vector<int> tour = this->tour;
    int j = 0;
    int num_trips = 0;
    while(tour[j] >= hotel_numbers)
        j++;
    int k = j;
    trip.clear();
    trip.push_back(j);
    trip.push_back(tour[j]);
    j = (j == tour.size()-1)?0:j+1;
    while(j != k) {
        while(tour[j] >= hotel_numbers){
            trip.push_back(tour[j]);
            j = (j == tour.size()-1)?0:j+1;
        }
        trip.push_back(tour[j]);
        matrixTrips.push_back(trip);
        trip.clear();
        trip.push_back(j);
        trip.push_back(tour[j]);
        if(j != k)
            j = (j == tour.size()-1)?0:j+1;
    }
    return matrixTrips;
}
