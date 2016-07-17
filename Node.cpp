#include "Node.h"
void Node::setX(double x)
{
    this->x = x;
}

void Node::setY(double y)
{
    this->y = y;
}

void Node::setDemand(double demand)
{
    this->demand = demand;
}

double Node::getX()
{
    return this->x;
}

double Node::getY()
{
    return this->y;
}
Node::Node(double x, double y, double demand){
    this->x = x;
    this->y = y;
    this->demand = demand;
}
