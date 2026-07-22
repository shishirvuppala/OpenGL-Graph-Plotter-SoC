#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"
#include "cubelet.h"
#include "function.h"

class Graph{
    private:
    Cubelet cubes[TOTALCUBELETS];
    float CENTERX = 0.0;
    float CENTERY = 0.0;
    float CENTERZ = 0.0;
    float SCALE = 5.0;
    Function f;

    float starttime;
public:
    Graph(float starttime);
    void draw(float curtime,std::vector<float>& graphed);
};

#endif