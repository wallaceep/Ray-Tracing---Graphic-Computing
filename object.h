#ifndef OBJECT_H
#define OBJECT_H

#include "ray.h"

// Estrutura para armazenar dados da colisão
struct HitRecord {
    double t;         // Distância da origem
    Vec3 p;           // Ponto de interseção
    Vec3 normal;      // Normal da superfície no ponto p
    int pigmentIndex; // Índice do pigmento (conforme PDF)
    int finishIndex;  // Índice do acabamento (conforme PDF)
};

class Object {
public:
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
};

#endif