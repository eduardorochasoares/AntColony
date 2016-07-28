#ifndef ANT_H_INCLUDED
#define ANT_H_INCLUDED
#include <vector>

class Ant{
    private:
        std::vector<int> tour;
        double objectiveFunction;
  public:
      Ant();
      ~Ant();
      std::vector<int>& getTour();
      void setObjectiveFunction(double value);
      double getObjectiveFunction();
      void setPath(std::vector<int> path);
      void setTour(std::vector<int> tour);
      std::vector<std::vector<int>> breakTrips(int hotel_numbers);

};
#endif // ANT_H_INCLUDED
