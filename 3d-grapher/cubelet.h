#ifndef CUBELET_H
#define CUBELET_H

#include "common.h"

class Function;

class Cubelet{
private:
    bool inside[8];
    float values[8];
    Point points[8];
    static constexpr std::pair<int,int> edges[12] = {{0,1},{2,3},{4,5},{6,7},{0,2},{1,3},{4,6},{5,7},{0,4},{1,5},{2,6},{3,7}};
    float botleftx,botlefty,botleftz;
public:
    Cubelet(){}
    void setposition(const Point& p);
    void draw(std::vector<float>& graphpoints, Function& func);
};

#endif