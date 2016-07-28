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
        }
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


    for(int i  = 0; i < population_size; ++i)
        delete ants[i];
    delete [] ants;
}

void Colony::readInstance()
{
    std::ifstream file;
    file.open("rc201.k40.tsphs");
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

   /* for(int i = 0; i < points_number; ++i){
        for(int j = 0; j < points_number; ++j){
            std::cout<<pheromone[i][j]<<" ";
        }
        std::cout<<"\n";
    }*/

}

Ant* Colony::findBestAnt(){
    std::vector<int> tour;
    double sum = 0;
    double best;
    Ant* bestAnt;
    tour = ants[0]->getTour();
    for(int j = 0; j < tour.size() ; ++j){

         if(tour[j] >= hotels_number){
                sum+= points[tour.at(j)]->getDemand();
          }
          if(j < tour.size() - 1)
                sum+= edges[tour.at(j)][tour.at(j + 1)].distance;
            else
                sum+= edges[tour.at(0)][tour.at(j)].distance;
    }

    best = sum;
    bestAnt = ants[0];

    for(int i = 0; i < population_size; ++i){
        sum = 0;
        tour = ants[i]->getTour();

        for(int j = 0; j < tour.size(); ++j){
            if(tour[j] >= hotels_number){
                sum+= points[tour.at(j)]->getDemand();
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
    std::cout<<"Best Solution of this generation: "<<bestAnt->getObjectiveFunction()<<std::endl;
    /*for(auto it = bestAnt->getTour().begin(); it != bestAnt->getTour().end(); ++it){
        bestTour.push_back(*it);
        std::cout<<*it <<" "<<std::endl;
    }*/
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


        normalize(ants[i]);
        //}
        twoOpt(ants[i]);
        //std::cout<<"ANT Number "<<i<<std::endl;
        //normalize(ants[i]);
        //ants[i]->getTour().insert(ants[i]->getTour().begin(), 0);
        //normalize(ants[i]);
        /*for(int j = 0; j < ants[i]->getTour().size(); ++j)
            std::cout<<ants[i]->getTour().at(j)<< " "<<std::endl;*/


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
    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<"Best Solution Ever: "<<best<<std::endl;


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
        currentNode = 0;

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
        vnd(ants[i]);
       // std::cout<<"ANT Nº "<<i<<std::endl;
    }
}

void Colony::normalize(Ant* ant){

    std::vector<int> hSolution;
    int  current = 0;
    double cost = 0;
    int count = 0;

    hSolution.push_back(0);

    int  i = 0;
    while (i < ant->getTour().size()){
        current = verifyHotel(cost, current, ant->getTour().at(i));
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

void Colony::twoOpt(Ant *ant) { //intratrip
    std::vector<std::vector<int>> matrixTrips = ant->breakTrips(hotels_number);
    std::vector<int> tour = ant->getTour();
    std::vector<int> initialTour = tour;
    std::vector<int> aux;
    std::vector<int> aux2;
    Ant *auxA = new Ant();
    int improve = 0;
    int maxIter = 20;

    for(int i = 0; i < matrixTrips.size(); i++) {
        aux.clear();
        for(int k = 2; k < matrixTrips[i].size()-1;k++){
            aux.push_back(matrixTrips[i][k]);
        }
        for(int a = 0; a < aux.size()-1; a++) {
            for(int b = a+1; b < aux.size(); b++){
                aux2.clear();
                for(int c = 0; c < a; c++)
                    aux2.push_back(aux[c]);
                int rev = 0;
                for(int c = a; c <= b; c++){
                    aux2.push_back(aux[b-rev]);
                    rev++;
                }
                for(int c = b+1; c < aux.size(); c++)
                    aux2.push_back(aux[c]);

                int l = 0;
                int j = (matrixTrips[i][0]+1) % tour.size();
                while(l < aux2.size()){
                    tour[j] = aux2[l];
                    l++;
                    j = (j+1) % tour.size();
                }
                auxA->setTour(tour);
                if(functionValue(auxA)<functionValue(ant)){
                        ant->setTour(tour);
                        matrixTrips = ant->breakTrips(hotels_number);
                        aux.clear();
                        for(int k = 2; k < matrixTrips[i].size()-1;k++){
                                aux.push_back(matrixTrips[i][k]);
                        }
                        improve = 0;
                }
                tour = ant->getTour();
            }
        }
        improve++;
        if(improve > maxIter)
            break;
    }
    delete auxA;
    if(!checkSolution(ant)){
        ant->setTour(initialTour);
    }
}

void Colony::twoOptInter(Ant *ant) {
    std::vector<int> tour = ant->getTour();
    std::vector<int> initialTour = tour;
    std::vector<int> aux;
    Ant *auxA = new Ant();
    int improve = 0;
    int maxIter = 20;

    while(improve < maxIter) {
        for(int a = 0; a < tour.size()-1; a++) {
            for(int b = a+1; b < tour.size(); b++){
                aux.clear();
                for(int c = 0; c < a; c++)
                    aux.push_back(tour[c]);
                int rev = 0;
                for(int c = a; c <= b; c++){
                    aux.push_back(tour[b-rev]);
                    rev++;
                }
                for(int c = b+1; c < tour.size(); c++)
                    aux.push_back(tour[c]);
                tour = aux;
                auxA->setTour(tour);
                if(functionValue(auxA)<functionValue(ant)){
                        ant->setTour(tour);
                        improve = 0;
                }
                tour = ant->getTour();
            }
        }
        improve++;
    }
    delete auxA;
    if(!checkSolution(ant)){
        ant->setTour(initialTour);
    }
}

std::vector<int> Colony::threeOpt(Ant *ant) {



}

std::vector<int> Colony::changeHotels(Ant *ant) {

    std::vector<int> tour = ant->getTour();
    Ant *aux = new Ant();
    double mim = ULONG_MAX;
    double value = 0;
    int pos = 0;
    int hotel = 0;
    int change = 0;
    for(int i = 0; i < tour.size(); i++) {
        if(tour[i] < hotels_number){
            for(int j = 0; j < hotels_number; j++) {
                if(tour[i] != j){
                    change = tour[i];
                    tour[i] = j;
                    aux->setTour(tour);
                    if(checkSolution(aux)){
                        value = 0;
                        value = functionValue(aux);
                        if(value < mim){
                            mim = value;
                            pos = i;
                            hotel = j;
                        }
                    }
                    tour[i] = change;
                    aux->setTour(tour);
                }
            }
        }
    }
    delete aux;
    if(mim < functionValue(ant)){
        tour[pos] = hotel;
        return tour;
    }
    return ant->getTour();
}

std::vector<int> Colony::exchange(Ant *ant) {
    std::vector<std::vector<int>> matrixTrips = ant->breakTrips(hotels_number);
    std::vector<int> tour = ant->getTour();
    std::vector<int> aux;
    std::vector<int> bestRoute;
    Ant *auxA = new Ant();
    int m = 3;
    int k = 2;
    int l;
    double mim = ULONG_MAX;
    double value;
    while(m > 0) {
        for(int i = 0; i < matrixTrips.size()-1; i++){
            if(matrixTrips[i].size()-3 >= m){
                for(int j = i+1; j < matrixTrips.size(); j++){
                    if(matrixTrips[j].size()-3 >= m) {
                        k = 2;
                        while(k+m-1<matrixTrips[i].size()-1) {
                            aux.clear();
                            for(int a = k; a < k+m; a++) {
                                aux.push_back(matrixTrips[i][a]);
                            }
                            l = 2;
                            while(l+m-1 < matrixTrips[j].size()-1){
                                int n = 0;
                                int index2 = matrixTrips[j][0]+l-1 % tour.size();
                                int index = matrixTrips[i][0]+k-1 % tour.size();
                                while(n < m){
                                    tour[index] = tour[index2];
                                    tour[index2] = aux[n];
                                    index = (index+1) % tour.size();
                                    index2 = (index2+1) % tour.size();
                                    n++;
                                }
                                auxA->setTour(tour);
                                if(checkSolution(auxA)){
                                    value = 0;
                                    value = functionValue(auxA);
                                    if(value < mim){
                                        bestRoute = tour;
                                        mim = value;
                                    }
                                }
                                tour = ant->getTour();
                                l++;
                            }
                            k++;
                        }
                    }
                }
            }
        }
        m--;
    }
    delete auxA;
    if(mim < functionValue(ant)){
        return bestRoute;
    }
    return ant->getTour();
}

bool Colony::checkSolution(Ant *ant) {
    std::vector<int> tour = ant->getTour();
    double sum = 0;
    int j = 0;

    while(tour[j] >= hotels_number)
        j++;

    int i = (j == tour.size()-1)?0:j+1;
    int k = j;
    while(i != k) {
        sum = 0;
        while (tour[i] >= hotels_number){
            sum += edges[tour[j]][tour[i]].distance + points[tour[i]]->getDemand();
            //std::cout << tour[j] << "-" << tour[i] << "/" << points[tour[i]]->getDemand() << "/" << edges[tour[j]][tour[i]].distance << "/" << edges[tour[j]][tour[i]].distance + points[tour[i]]->getDemand() << std::endl;
            j = i;
            i = (j == tour.size()-1)?0:j+1;
        }
        sum += edges[tour[j]][tour[i]].distance;
        //std::cout << tour[j] << "-" << tour[i] << "/" << points[tour[i]]->getDemand() << "/" << edges[tour[j]][tour[i]].distance << "/" << edges[tour[j]][tour[i]].distance + points[tour[i]]->getDemand() << std::endl;
        if(sum > limit) {
            return false;
        }
        if(i != k) {
            j = i;
            i = (j == tour.size()-1)?0:j+1;
        }
    }
    return true;
}

double Colony::functionValue(Ant* ant){

    std::vector<int> tour = ant->getTour();
    int j = 0;
    int days = 0;
    double sum = 0;

    //Acha o primeiro hotel
    while(tour[j] >= hotels_number)
        j++;

    int i = (j == tour.size()-1)?0:j+1;
    int k = j;
    while(i != k) {
        days++;
        while (tour[i] >= hotels_number){
            sum += edges[tour[j]][tour[i]].distance + points[tour[i]]->getDemand();
            j = i;
            i = (j == tour.size()-1)?0:j+1;
        }
        sum += edges[tour[j]][tour[i]].distance;
        if(i != k) {
            j = i;
            i = (j == tour.size()-1)?0:j+1;
        }
    }
    return sum;
}

void Colony::vnd(Ant* ant){
    double xo = functionValue(ant);
    double x;
    int i = 0;
    while(i < 3){

        switch (i){
            case 0:
                twoOpt(ant);
                break;
            case 1:
                ant->setTour(exchange(ant));
                break;
            case 2:
                ant->setTour(changeHotels(ant));
                break;
        }
        x = functionValue(ant);
        if( x < xo){
            xo = x;
            i = 0;
        }else{
            ++i;
        }
    }

}

