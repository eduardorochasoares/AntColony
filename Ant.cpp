#include "Ant.h"
#include "Colony.h"
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
