#include "common.h"
#include "cubelet.h"
#include "function.h"

bool func(Point p,float& val){
    val = p.x+p.y+p.z;
    return (val < 0);
};

void Cubelet::setposition(const Point& p){
    botleftx = p.x;botlefty = p.y;botleftz = p.z;
    for(int i = 0;i<8;i++){
        float posx = botleftx + (i%2)*DELTA;
        float posy = botlefty + ((i/2) % 2)*DELTA;
        float posz = botleftz + ((i/4) % 2)*DELTA;
        points[i] = {posx,posy,posz};
    }
}

void Cubelet::draw(std::vector<float>& graphpoints,Function& func){

    for(int i = 0;i<8;i++){
        inside[i] = func(points[i],values[i]);
    }

    std::vector<Point> curs;

    for(int i = 0;i<12;i++){
        int ix = edges[i].first;
        int jx = edges[i].second;
        if((inside[ix] ^ inside[jx]) == 1){
            curs.push_back( ( points[ix] * std::abs(values[jx]) + points[jx] * std::abs(values[ix]) ) / (std::abs(values[jx]) + std::abs(values[ix])) ); 
        }
    }

    int size = curs.size();

    if(size < 3) return;

    Point centroid = {0,0,0};
    for(auto p : curs){
        centroid = centroid + p;
    }

    centroid = centroid / size;

    Point normal = {0, 0, 0};
    Point v1 = curs[1] - curs[0];
    for (size_t i = 2; i < curs.size(); i++) {
        normal = cross(v1, curs[i] - curs[0]);
        if (dot(normal, normal) > 1e-6) break;
    }

    if(dot(normal, normal) < 1e-6) return;

    Point ref = curs[0] - centroid;

    std::sort(curs.begin(), curs.end(), [&](const Point& a, const Point& b) {
        Point dirA = a - centroid;
        Point dirB = b - centroid;
        
        float angleA = std::atan2(dot(cross(ref, dirA), normal), dot(ref, dirA));
        float angleB = std::atan2(dot(cross(ref, dirB), normal), dot(ref, dirB));
        
        return angleA < angleB;
    });

    int nooftriangle = size-2;

    for(int i = 0;i<nooftriangle;i++){
        addtograph(graphpoints,curs[0]);
        addtograph(graphpoints,curs[i+1]);
        addtograph(graphpoints,curs[i+2]);
    }
}
