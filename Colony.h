#ifndef COLONY_H_INCLUDED
#define COLONY_H_INCLUDED
#include "Ant.h"
#include "Node.h"
#include <list>
class Colony{
    private:
        typedef struct prob{
            double probability;
            double distance;
            bool visited;
        }Edges;

        Ant** ants;
        int population_size;
        int hotels_number;
        int costumer_number;
        int points_number;
        Edges** edges;
        double** pheromone;
        std::list<Node*> points;
        std::list<Node*> hotels;
        std::list<Node*> costumers;
    public:
        Colony();
        ~Colony();
        void readInstance();
        void UpdatePheromone();
        Ant* findBestAnt();
        int chooseEdge(int i);

};


#endif // COLONY_H_INCLUDED
