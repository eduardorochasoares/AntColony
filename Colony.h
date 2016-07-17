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
        double limit;
        Edges** edges;
        double** pheromone;
        Node** points;
        Node** hotels;
        Node** costumers;
        void resetPaths();
        void cleanEdges();
        void normalize(Ant* ant);
        int verifyHotel(double cost, int i, int j);
    public:
        Colony(int population_size);
        ~Colony();
        void readInstance();
        void UpdatePheromone();
        Ant* findBestAnt();
        int chooseEdge(int i);
        void firstConstruction();
        void constructSolution();

};


#endif // COLONY_H_INCLUDED
