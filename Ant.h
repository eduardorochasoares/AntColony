#ifndef ANT_H_INCLUDED
#define ANT_H_INCLUDED
#include <list>

class Ant{
    private:
        std::list<int> tour;
        double objectiveFunction;
        int n;
  public:
      Ant(int n);
      ~Ant();
      std::list<int>& getTour();
      void setObjectiveFunction(double value);
      double getObjectiveFunction();



};
#endif // ANT_H_INCLUDED
