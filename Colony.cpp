#include "Colony.h"
#include <fstream>
#include <vector>
#include <math.h>


#define p 0.4
#define mi 5
#define phi 2
#define alfa 0.8
#define beta 0.2
Colony::Colony()
{
   readInstance();

   this->pheromone =  new double*[points_number];
   for(int i = 0; i < points_number; ++i){
       this->pheromone[i] = new double[points_number];
   }
   this->edges = new Colony::Edges*[points_number];
   for(int i = 0; i < points_number; ++i){
      this->edges[i] = new Colony::Edges[points_number];
   }

   for(int i = 0 ; i < points_number; ++i)
      for(int j = 0; j < points_number; ++j)
         this->edges[i][j].visited = false;


}

Colony::~Colony()
{

}

void Colony::readInstance()
{
    std::ifstream file;
    file.open("pr07.k15.tsphs");
    Node* node;
    file>>hotels_number;
    file>>costumer_number;
    points_number = hotels_number + costumer_number;
    double x, y, demand;
    for(int i  = 0; i < (hotels_number + costumer_number); ++i){
        if(i < hotels_number){
            file>>x;
            file>>y;
            demand = 0;
            node = new Node(x, y, demand);
            hotels.push_back(node);
        }else{
            file>>x;
            file>>y;
            file>>demand;
            node = new Node(x,y, demand);
            costumers.push_back(node);
        }
        points.push_back(node);

    }
}
void Colony::UpdatePheromone()
{
    Ant* ant = findBestAnt();
    std::vector<int> tour;
    tour = ant->getTour();
    //atualizacao de feromonio da formiga com melhor solução
    for(int i = 0; i < tour.size() - 1; ++i){
        //evapora o feromonio
        pheromone[tour.at(i)][tour.at(i+1)] = pheromone[tour.at(i)][tour.at(i+1)] * (1 - p);
        //deposita feromonio
        pheromone[tour.at(i)][tour.at(i+1)] = pheromone[tour.at(i)][tour.at(i+1)] + mi* (1/ant->getObjectiveFunction());
    }

    //atualizacao de feromonio das demais formigas
    for(int i = 0 ; i < population_size; i++){
        if(ants[i] != ant){
            tour = ants[i]->getTour();
            for(int i = 0; i < tour.size() - 1; ++i){
                //evaporação e deposito de feromonio a uma taxa menor
                pheromone[tour.at(i)][tour.at(i+1)] = pheromone[tour.at(i)][tour.at(i+1)] * (1 - p)
                    + phi*(1/ants[i]->getObjectiveFunction());
            }
        }
    }


}

Ant* Colony::findBestAnt(){
    std::vector<int> tour;
    double sum = 0;
    double best;
    Ant* bestAnt;
    tour = ants[0]->getTour();
    for(int i = 0; i < tour.size() - 1; ++i){
        sum += edges[tour.at(i)][tour.at(i + 1)].distance;
    }
    best = sum;
    bestAnt = ants[0];

    for(int i = 0; i < population_size; ++i){
        sum = 0;
        tour = ants[i]->getTour();

        for(int j = 0; j < tour.size() - 1; ++j){
            sum+= edges[tour.at(j)][tour.at(j + 1)].distance;
        }
        ants[i]->setObjectiveFunction(sum);
        if(sum < best){
            best = sum;
            bestAnt = ants[i];
        }
    }
    return bestAnt;
}

int Colony::chooseEdge(int i)
{
    std::list<int> l;
    double sum = 0;
    for(int j = 0; j < points_number; ++j){
        if(!edges[i][j].visited && i != j)
            sum += pow(pheromone[i][j], alfa) * pow(1/(edges[i][j].distance), beta);
    }
    for(int j = 0; j < points_number; ++j){
        if(!edges[i][j].visited && i != j){
            edges[i][j].probability = pow(pheromone[i][j], alfa) * pow(1/(edges[i][j].distance), beta) / sum;
            l.push_back(j);
        }
    }
    int number = rand()%100;
    int index = 0;
    for(auto it = l.begin(); it != l.end(); ++it){
        if(number >= index || number < (edges[i][*it].probability)*100){
            return *it;
        }
        index += (edges[i][*it].probability)*100;
    }



}
