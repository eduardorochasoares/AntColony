#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

class Node{
    private:

        double x, y, demand;
    public:
        void setX(double x);
        void setY(double y);
        void setDemand(double demand);
        Node();
        double getX();
        double getY();
        double getDemand();
};

#endif // NODE_H_INCLUDED
