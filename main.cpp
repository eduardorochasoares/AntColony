#include <fstream>
#include "Node.h"
#include <time.h>
#include "Colony.h"
int main(){
    srand(time(NULL));
    Colony* c = new Colony(10);
    c->readInstance();
    return 0;
}

void readInstance(){

}
