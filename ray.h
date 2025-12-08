#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray() {}
    
    Ray(const Vec3& origin, const Vec3& direction) 
        : origin(origin), direction(direction) {}

    const Vec3& getOrigin() const { return origin; }
    const Vec3& getDirection() const { return direction; }

    // Calcula o ponto 3D no raio dado um parâmetro t
    // P(t) = Origem + t * Direção
    Vec3 pointAt(double t) const {
        return origin + (direction * t);
    }
};

#endif