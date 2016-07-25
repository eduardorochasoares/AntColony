#include "Colony.h"
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <climits>
#include <iostream>
#include "LKMatrix.h"
#define p 0.4
#define mi 50
#include <stdlib.h>
#define phi 2
#define alfa 0.8
#include <time.h>
#define beta 0.2
#define RUNS 10
#define MAX_ITERATIONS_PER_SEED 100
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
      for(int j = 0; j < points_number; ++j){
          this->edges[i][j].visited = false;
          this->edges[i][j].probability = 0;
      }

    this->ants = new Ant*[population_size];
    for(int i = 0; i < population_size; ++i)
        this->ants[i] = new Ant();

    for(int i = 0; i < points_number; ++i){
        for(int j = 0; j < points_number; ++j){
            edges[i][j].distance = sqrt(pow(points[i]->getX() - points[j]->getX(), 2) +
            pow(points[i]->getY() - points[j]->getY(), 2));
          // std::cout<<edges[i][j].distance<<" ";
        }
        //std::cout<<"\n";
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
    int index;
    for(int i  = 0; i < (hotels_number + costumer_number); ++i){
        if(i < hotels_number){
            file>>index;
            file>>x;
            file>>y;
            demand = 0;
            node = new Node();
            node->setIndex(index);
            node->setX(x);
            node->setY(y);
            node->setDemand(demand);
            hotels[i] =  node;
        }else{
            file>>index;
            file>>x;
            file>>y;
            file>>demand;
            node = new Node();
            node->setX(x);
            node->setY(y);
            node->setIndex(index);
            node->setDemand(demand);
            costumers[i - hotels_number] = node;
        }
        points[i] = node;

        //std::cout<<"node "<<i<<" X: "<<node->getX()<<" Y: "<<node->getY()<<std::endl;

    }



}
void Colony::UpdatePheromone()
{
    Ant* ant = findBestAnt();
    std::vector<int> tour;
    tour = ant->getTour();
    //atualizacao de feromonio da formiga com melhor solução
    for(int i = 0; i < tour.size(); ++i){
        //evapora o feromonio
        if(i == tour.size() - 1){
            pheromone[tour.at(i)][tour.at(0)] = pheromone[tour.at(i)][tour.at(0)] * (1 - p);
            //deposita feromonio
            pheromone[tour.at(i)][tour.at(0)] = pheromone[tour.at(i)][tour.at(0)] + mi* (1/ant->getObjectiveFunction());
            pheromone[tour.at(0)][tour.at(i)] = pheromone[tour.at(i)][tour.at(0)];

        }else{
            pheromone[tour.at(i)][tour.at(i+1)] = pheromone[tour.at(i)][tour.at(i+1)] * (1 - p);
            //deposita feromonio
            pheromone[tour.at(i)][tour.at(i+1)] = pheromone[tour.at(i)][tour.at(i+1)] + mi* (1/ant->getObjectiveFunction());
            pheromone[tour.at(i + 1)][tour.at(i)] = pheromone[tour.at(i)][tour.at(i+1)];
        }
    }

    //atualizacao de feromonio das demais formigas
    for(int i = 0 ; i < population_size; i++){
        if(ants[i] != ant){
            tour = ants[i]->getTour();

            for(int j = 0; j < tour.size(); ++j){
                if(j == tour.size()- 1){
                    pheromone[tour.at(j)][tour.at(0)] = pheromone[tour.at(j)][tour.at(0)] * (1 - p)
                        + phi*(1/ants[i]->getObjectiveFunction());
                    pheromone[tour.at(0)][tour.at(j)] = pheromone[tour.at(j)][tour.at(0)];
                }else{
                //evaporação e deposito de feromonio a uma taxa menor
                //std::cout<<j<<std::endl;
                    pheromone[tour.at(j)][tour.at(j+1)] = pheromone[tour.at(j)][tour.at(j+1)] * (1 - p)
                        + phi*(1/ants[i]->getObjectiveFunction());
                    pheromone[tour.at(j+1)][tour.at(j)] = pheromone[tour.at(j)][tour.at(j+1)];
                }
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

        for(int j = 0; j < tour.size(); ++j){
            if(tour[j] >= hotels_number){
                sum+= points[j]->getDemand();
            }
            if(j < tour.size() - 1)
                sum+= edges[tour.at(j)][tour.at(j + 1)].distance;
            else
                sum+= edges[tour.at(0)][tour.at(j)].distance;
        }

        ants[i]->setObjectiveFunction(sum);
        if(sum < best){
            best = sum;
            bestAnt = ants[i];
        }
    }
    bestSolutions.push_back(bestAnt->getObjectiveFunction());
    std::cout<<"Best Solution: "<<bestAnt->getObjectiveFunction()<<std::endl;
    for(auto it = bestAnt->getTour().begin(); it != bestAnt->getTour().end(); ++it){
        bestTour.push_back(*it);
        std::cout<<*it <<" "<<std::endl;
    }
    return bestAnt;
}

int Colony::chooseEdge(int i)
{
    double sum = 0;

    std::list<int> l;

    for(int j = hotels_number; j < points_number; ++j){
        if(!edges[i][j].visited && i != j){
            if(edges[i][j].distance == 0){
                edges[i][j].visited = true;
                edges[j][i].visited = true;
                return j;
            }
            //std::cout<<"parcial sum: "<<sum<<std::endl;

            sum += pow(pheromone[i][j], alfa) * pow(1/(edges[i][j].distance), beta);
        }
    }
    //std::cout<<"sum: "<<sum<<std::endl;

    for(int j = hotels_number; j < points_number; ++j){

        if(!edges[i][j].visited && i != j){
            edges[i][j].probability = pow(pheromone[i][j], alfa) * pow(1/(edges[i][j].distance), beta) / sum;
            //std::cout<<"Prob Edge ("<<i<<" "<<j<<") : "<<edges[i][j].probability<<std::endl;

            if(edges[i][j].probability != 0){
                l.push_back(j);
                //std::cout<<j<<std::endl;
            }
        }
    }
    int number = rand()%10000;
    double index = 0;
    int j;
    //std::cout<<"number: "<<number<<std::endl;
    if(l.size() == 0) return 0;
    for(auto it = l.begin(); it != l.end(); ++it){
        j = *it;
        if(number >= index && number < ((edges[i][j].probability)*10000  + index)){
            edges[i][j].visited = true;
            edges[j][i].visited = true;

            return j;
        }
        index += (edges[i][j].probability)*10000;
    }
    int num = rand() % points_number + hotels_number;
    while(!edges[i][num].visited){
        num = rand() % points_number;

    }
    return num;

}

void Colony::firstConstruction(){
    Ant* current;
    int count = 0;
    vector<int> aux;
    vector<int> id;
    int index = 0;
    vector<pair<double,double>> coord;
    srand(time(NULL));
    for(int i = 0 ; i < costumer_number; ++i){
        id.push_back(i + hotels_number);
        coord.push_back(make_pair(costumers[i]->getX(), costumers[i]->getY()));
    }
    LKMatrix mat(coord, id);
    int currentNode = 0;
    for(int i = 0; i < population_size; ++i){
        currentNode = 0;
        current = ants[i];

        if(i == 0){
            current->setTour(mat.optimizeTour());
            for(int j = 0; j < ants[i]->getTour().size(); ++j){
                aux.push_back(ants[i]->getTour().at(j) + hotels_number);
                //std::cout<<aux.at(j)<<" "<<std::endl;
            }
            ants[i]->setTour(aux);


        }else if(i < 0.7*population_size){
                cheapestInsertion(ants[i]);
        }else{
            count = 0;
            //current->getTour().push_back(0);
            while(count < costumer_number){
                currentNode = rand() % (costumer_number) + hotels_number;
                if(find(current->getTour().begin(), current->getTour().end(), currentNode) == current->getTour().end()){
                    current->getTour().push_back(currentNode);
                    count++;
                }
            }
        }



        //}

        //std::cout<<"ANT Number "<<i<<std::endl;
        //normalize(ants[i]);
        //ants[i]->getTour().insert(ants[i]->getTour().begin(), 0);
        //normalize(ants[i]);
        for(int j = 0; j < ants[i]->getTour().size(); ++j)
            std::cout<<ants[i]->getTour().at(j)<< " "<<std::endl;


    }
    //cheapestInsertion(ants[1]);

    int num = 0;
    int iterations = 0;
    while(num < RUNS){
        srand(time(NULL) + num);
        while(iterations < MAX_ITERATIONS_PER_SEED){
            UpdatePheromone();
            cleanEdges();
            resetPaths();
            constructSolution();
            Ant* a = findBestAnt();
            auto it = a->getTour().begin();
            while(it != a->getTour().end()){
                //std::cout <<*it<<std::endl;
                ++it;
            }
            iterations++;
        }

        //resetPaths();
        num++;
    }
    double best = *bestSolutions.begin() ;
    for(auto it = bestSolutions.begin(); it != bestSolutions.end(); ++it ){
        if( *it < best)
            best = *it;
    }

    std::cout<<"best solution ever: "<<best<<std::endl;


}

void Colony::constructSolution()
{
    Ant* current;
    int count = 0;
    int currentNode = 0;
    for(int i = 0; i < population_size; ++i){
        cleanEdges();
        current = ants[i];
        count = 0;
        int index = rand() % costumer_number + hotels_number;
        current->getTour().push_back(index);
        currentNode = *ants[i]->getTour().begin();

        while(count < costumer_number - 1){
            currentNode = chooseEdge(currentNode);
            if(currentNode == -1){
                break;
            }
            if(find(current->getTour().begin(), current->getTour().end(), currentNode) == current->getTour().end()){
                current->getTour().push_back(currentNode);
                //std::cout<<" node : "<<currentNode<<std::endl;
                count++;
            }
        }
        //std::cout<<"ANT Number "<<i<<std::endl;
        normalize(ants[i]);
       // std::cout<<"ANT Nº "<<i<<std::endl;
    }
}

void Colony::normalize(Ant* ant){

    std::vector<int> hSolution;
    int  current = 0;
    double cost = 0;
    int count = 0;

    hSolution.push_back(*ant->getTour().begin());

    int  i = 0;
    while (i < ant->getTour().size() -1){
        current = verifyHotel(cost, current, ant->getTour().at(i+1));
        /*if(current == ant->getTour().at(i))
            return;*/

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

/**Incomplete**/
void Colony::cheapestInsertion(Ant* ant)
{
    std::vector<int> tour = ant->getTour();
    int num;
    for(int i = 0; i < 3; ++i){
        num = rand() % costumer_number + hotels_number;
        if(find(tour.begin(), tour.end(), num) == tour.end()){
            tour.push_back(num);
        }
    }

    int count = 0;
    auto bestPostion = tour.begin();
    double bestDistance  = ULONG_MAX;
    int i  = 0;
    while(count < costumer_number - 3){
        num = rand() % costumer_number + hotels_number;
        i = 0;
        bestDistance = ULONG_MAX;
        bestPostion = tour.begin();
        if(find(tour.begin(), tour.end(), num) == tour.end()){
            for(auto it = tour.begin(); it != tour.end(); ++it){
                if(i  == 0){
                    if(edges[*it][num].distance + edges[num][tour.back()].distance - edges[*it][tour.back()].distance < bestDistance){
                        bestDistance = edges[*it][num].distance + edges[num][tour.back()].distance - edges[*it][tour.back()].distance;
                        bestPostion = it;
                    }
                }else if( i == tour.size() -1){
                            if(edges[*it][num].distance + edges[num][tour.front()].distance - edges[*it][tour.front()].distance < bestDistance){
                                bestDistance = edges[*it][num].distance + edges[num][tour.front()].distance - edges[*it][tour.front()].distance;
                                bestPostion = it;
                            }
                    }else{
                        if(edges[*it][num].distance + edges[num][*it+1].distance - edges[*it][*it+1].distance < bestDistance){
                            bestDistance = edges[*it][num].distance + edges[num][*it+1].distance - edges[*it][*it+1].distance;
                            bestPostion = it;
                        }
                    }



             ++i;
            }
            tour.insert(bestPostion, num);
            count++;
        }

    }
    //std::cout<<"cheapest"<<std::endl;
    for(auto it = tour.begin(); it != tour.end(); ++it)
        //std::cout<<*it<<std::endl;
    ant->setTour(tour);
}


