#include <fstream>
#include "Node.h"
#include <time.h>
#include "Colony.h"
#include <stdlib.h>
int main(){
    srand(time(NULL));
    Colony* c = new Colony(10);
    c->readInstance();
    return 0;
}

void readInstance(){

}
