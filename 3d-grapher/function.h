#ifndef FUNCTION_H
#define FUNCTION_H

#include "common.h" 

class Function {
public:
    float t = 0.0;

    bool operator()(const Point& p, float& val) {
        float r_squared = 12.5 + 12.5 * std::sin(t); 
        
        val = (p.x * p.x) + (p.y * p.y) + (p.z * p.z) - r_squared;
        
        return val < 0.0; 
    }
};

#endif