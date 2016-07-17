#include "Colony.h"
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <climits>
#include <iostream>
#define p 0.4
#define mi 50
#define phi 2
#define alfa 0.8
#define beta 0.2
Colony::Colony(int population_size)
{
   readInstance();
   this->population_size = population_size;

   this->pheromone =  new double*[points_number];
   for(int i = 0; i < points_number; ++i){
       this->pheromone[i] = new double[points_number];
   }
   for(int i = 0; i < points_number; ++i)
    for(int j = 0; j < points_number; ++j)
        pheromone[i][j] = 0;
   this->edges = new Colony::Edges*[points_number];
   for(int i = 0; i < points_number; ++i){
      this->edges[i] = new Colony::Edges[points_number];
   }

   for(int i = 0 ; i < points_number; ++i)
      for(int j = 0; j < points_number; ++j)
          this->edges[i][j].visited = false;

    this->ants = new Ant*[population_size];
    for(int i = 0; i < population_size; ++i)
        this->ants[i] = new Ant();

    for(int i = 0; i < points_number; ++i)
        for(int j = 0; j < points_number; ++j){
            edges[i][j].distance = sqrt(pow(points[i]->getX() - points[j]->getX(), 2) +
            pow(points[i]->getY() - points[j]->getY(), 2));
        }

    firstConstruction();
}

Colony::~Colony()
{
    for(int i = 0; i < points_number; ++i)
        delete pheromone[i];

    delete [] pheromone;

    for(int i = 0; i < points_number; ++i)
        delete edges[i];
    delete [] edges;
}

void Colony::readInstance()
{
    std::ifstream file;
    file.open("pr07.k15.tsphs");
    Node* node;
    file>>this->hotels_number;
    file>>this->costumer_number;
    file>>this->limit;
    this->points_number = hotels_number + costumer_number;

    hotels = new Node*[hotels_number];
    for(int i = 0; i < hotels_number; i++)
        hotels[i] = new Node();
    costumers = new Node*[costumer_number];

    for(int i = 0; i < costumer_number; i++)
        costumers[i] = new Node();

     points = new Node*[hotels_number + costumer_number];
     for(int i = 0; i < costumer_number + hotels_number; i++)
        points[i] = new Node();

    double x, y, demand;
    for(int i  = 0; i < (hotels_number + costumer_number); ++i){
        if(i < hotels_number){
            file>>x;
            file>>y;
            demand = 0;
            node = new Node();
            node->setX(x);
            node->setY(y);
            node->setDemand(demand);
            hotels[i] =  node;
        }else{
            file>>x;
            file>>y;
            file>>demand;
            node = new Node();
            node->setX(x);
            node->setY(y);
            node->setDemand(demand);
            costumers[i - hotels_number] = node;
        }
        points[i] = node;

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

            for(int j = 0; j < tour.size() - 1; ++j){

                //evaporação e deposito de feromonio a uma taxa menor
                //std::cout<<j<<std::endl;
                pheromone[tour.at(j)][tour.at(j+1)] = pheromone[tour.at(j)][tour.at(j+1)] * (1 - p)
                    + phi*(1/ants[i]->getObjectiveFunction());
            }
        }
    }

    for(int i = 0; i < points_number; ++i){
        for(int j = 0; j < points_number; ++j){
            std::cout<<pheromone[i][j]<<" ";
        }
        std::cout<<"\n";
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
    std::cout<<"Best SOlution: "<<bestAnt->getObjectiveFunction();
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
    return -1;

}

void Colony::firstConstruction(){
    Ant* current;
    int count = 0;
    int currentNode = 0;
    for(int i = 0; i < population_size; ++i){
        currentNode = 0;
        current = ants[i];
        count = 0;
        current->getTour().push_back(0);
        while(count < costumer_number){
            currentNode = rand() % (costumer_number) + hotels_number;
            if(find(current->getTour().begin(), current->getTour().end(), currentNode) == current->getTour().end()){
                current->getTour().push_back(currentNode);
                count++;
            }
        }
        //std::cout<<"ANT Number "<<i<<std::endl;
        normalize(ants[i]);
    }
    UpdatePheromone();
    resetPaths();

}

void Colony::constructSolution()
{
    Ant* current;
    int count = 0;
    int currentNode = 0;
    for(int i = 0; i < population_size; ++i){
        cleanEdges();
        currentNode = 0;
        current = ants[i];
        count = 0;
        current->getTour().push_back(0);
        while(count < costumer_number){
            currentNode = chooseEdge(currentNode);
            if(find(current->getTour().begin(), current->getTour().end(), currentNode) == current->getTour().end()){
                current->getTour().push_back(currentNode);
                count++;
            }
        }
        //std::cout<<"ANT Number "<<i<<std::endl;
        normalize(ants[i]);
    }
}

void Colony::normalize(Ant* ant){

    std::vector<int> hSolution;
    int  current = 0;
    double cost = 0;
    int count = 0;

    hSolution.push_back(*ant->getTour().begin());

    int  i = 0;
    while (i < ant->getTour().size() -2){
        current = verifyHotel(cost, current, ant->getTour().at(i+1));
        if(current == ant->getTour().at(i))
            return;

        if(current < hotels_number)
            cost = 0;

        else{
            i = i + 1;
            cost = cost + edges[hSolution.at(count)][current].distance + points[current]->getDemand();
        }
        count = count + 1;
        hSolution.push_back(current);

    }
    ant->setPath(hSolution);
    for(auto it = hSolution.begin(); it != hSolution.end(); ++it);
       // std::cout<<*it<<std::endl;


}

void Colony::resetPaths(){
    for(int i = 0; i < population_size; ++i){
        ants[i]->getTour().erase(ants[i]->getTour().begin(), ants[i]->getTour().end());
    }

}

void Colony::cleanEdges(){
    for(int i = 0; i < points_number; ++i)
        for(int j = 0; j < points_number; ++j)
            edges[i][j].visited = false;
}

int Colony::verifyHotel(double cost, int i, int j)
{
    int index = -1;
    double minimal = ULONG_MAX;
    for (int x = 0; x < hotels_number; ++x){
        if(edges[i][x].distance < minimal && i != x){
            minimal = edges[i][x].distance;
            index = x;
        }
    }

    if(edges[i][j].distance + cost + points[j]->getDemand() + minimal >= limit){
        minimal = ULONG_MAX;
        for (int x = 0; x < hotels_number; x++){
            if(edges[j][x].distance < minimal && j != x){
                minimal = edges[j][x].distance;
                index = x;
            }
        }
        return index;
    }
    else
        return j;

}
