#ifndef ANT_H_INCLUDED
#define ANT_H_INCLUDED
#include <vector>

class Ant{
    private:
        std::vector<int> tour;
        double objectiveFunction;
        int n;
  public:
      Ant(int n);
      ~Ant();
      std::vector<int>& getTour();
      void setObjectiveFunction(double value);
      double getObjectiveFunction();




};
#endif // ANT_H_INCLUDED
