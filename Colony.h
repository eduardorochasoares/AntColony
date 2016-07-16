#ifndef COLONY_H_INCLUDED
#define COLONY_H_INCLUDED
#include "Ant.h"
class Colony{
    private:
        typedef struct node{
            double x, y, demand;
            bool visited;
        }Node;
        Ant** ants;
        int population_size;
        int hotels_number;
        int costumer_number;
        int points_number;
        double** distance;
        double** pheromone;
        std::list<Node*> points;
        std::list<Node*> hotels;
        std::list<Node*> costumers;
    public:
        Colony();
        ~Colony();
        void readInstance();
};


#endif // COLONY_H_INCLUDED
