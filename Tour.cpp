#include "Tour.h"
#define alpha 0.8
#define beta 0.2
#define MI 0.8903
#define p 0.5
#include <math.h>





int Tour::chooseEdge(int i)
{
    int rule_size = 2*(n_points);
    double sum = 0;
    int probVect[n_points];
    for(int j = 0; j < n_points; j++){
        if(i != j){
            sum+= pow(pheromone[i][j], alpha)*pow(1/distance[i][j], beta);
        }
    }
    for(int j = 0; j < n_points; ++j){
        if(i == j){
            probVect[j] = -1;
        }else{
            probVect
        }
    }
}
