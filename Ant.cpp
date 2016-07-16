#include "Ant.h"
#include "Colony.h"
#define alpha 0.8
#define beta 0.2
#include <math.h>

Ant::Ant(int n){
    this->n = n;
    this->objectiveFunction = 0;

}


std::list<int>& Ant::getTour(){
    return this->tour;
}

void Ant::setObjectiveFunction(double val){
    this->objectiveFunction = val;
}

double Ant::getObjectiveFunction(){
    return this->objectiveFunction;
}

Ant::~Ant()
{

}
