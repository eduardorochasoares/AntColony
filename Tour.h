#ifndef TOUR_H_INCLUDED
#define TOUR_H_INCLUDED
#include "Ant.h"
#include "Colony.h"
class Tour{
    private:
        double** distance;
        int n_points;
        int population_size;
        double** pheromone;
        Colony *colony;

    public:
        Tour(int n_points, int population_size);
        ~Tour();
        void globalUpdatePheromone();
        Ant* findBestAnt();
        int chooseEdge(int i);

};

#endif // TOUR_H_INCLUDED
