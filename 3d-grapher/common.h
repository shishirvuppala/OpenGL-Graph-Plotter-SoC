#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>

struct Point {
    float x;
    float y;
    float z;

    Point operator+(const Point& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Point operator-(const Point& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Point operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    Point operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }
};

inline Point operator*(float scalar, const Point& p) {
    return {p.x * scalar, p.y * scalar, p.z * scalar};
}

inline float dot(const Point& a, const Point& b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
};

inline Point cross(const Point& a, const Point& b){
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
};

inline void addtograph(std::vector<float>& vec , Point& p){
    vec.push_back(p.x);
    vec.push_back(p.y);
    vec.push_back(p.z);
}

const int CUBELETSPERSIDE = 64;
const float SCALE = 5.0;
const float DELTA = 2*SCALE /CUBELETSPERSIDE;
const int TOTALCUBELETS = CUBELETSPERSIDE * CUBELETSPERSIDE * CUBELETSPERSIDE;

#endif