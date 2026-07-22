#include "graph.h"

Graph::Graph(float starttime) : starttime(starttime) {

    for (int z = 0; z < CUBELETSPERSIDE; z++) {
        for (int y = 0; y < CUBELETSPERSIDE; y++) {
            for (int x = 0; x < CUBELETSPERSIDE; x++) {
                int idx = x + (y * CUBELETSPERSIDE) + (z * CUBELETSPERSIDE * CUBELETSPERSIDE);
                
                float px = (CENTERX - SCALE) + (x * DELTA);
                float py = (CENTERY - SCALE) + (y * DELTA);
                float pz = (CENTERZ - SCALE) + (z * DELTA);
                
                cubes[idx].setposition({px, py, pz});
            }
        }
    }
}

void Graph::draw(float curtime,std::vector<float>& graphed){
    f.t = curtime - starttime;

    for(int i =0;i<TOTALCUBELETS;i++){
        cubes[i].draw(graphed,f);
    }
}