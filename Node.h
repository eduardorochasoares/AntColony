#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

class Node{
    private:

        double x, y, demand;
        int index;
    public:
        void setX(double x);
        void setY(double y);
        void setIndex(int index);
        void setDemand(double demand);
        Node();
        double getX();
        double getY();
        double getDemand();
        int getIndex();
};

#endif // NODE_H_INCLUDED
