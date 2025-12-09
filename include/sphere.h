#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "vec3.h"

class Sphere : public Object {
public:
    Vec3 center;
    double radius;
    int pigmentIndex;
    int finishIndex;

    Sphere() {}
    Sphere(Vec3 cen, double r, int pigIdx, int finIdx) 
        : center(cen), radius(r), pigmentIndex(pigIdx), finishIndex(finIdx) {};

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
        Vec3 oc = r.origin - center; // Vetor do Centro da esfera até a Origem do raio
        
        // Coeficientes da equação quadrática
        double a = dot(r.direction, r.direction);
        double b = 2.0 * dot(oc, r.direction);
        double c = dot(oc, oc) - radius * radius;
        
        double discriminant = b*b - 4*a*c;

        if (discriminant > 0) {
            double sqrt_delta = sqrt(discriminant);
            
            // Tentamos a primeira raiz (a mais próxima da câmera: -b - sqrt)
            double temp = (-b - sqrt_delta) / (2.0*a);
            
            // Verificamos se está dentro do intervalo aceitável (na frente da câmera)
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.pointAt(rec.t);
                // A normal de uma esfera é simplesmente (Ponto - Centro) / Raio
                rec.normal = (rec.p - center) / radius; 
                rec.pigmentIndex = pigmentIndex;
                rec.finishIndex = finishIndex;
                return true;
            }
            
            // Se a primeira raiz falhou, tentamos a segunda (+ sqrt)
            temp = (-b + sqrt_delta) / (2.0*a);
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.pointAt(rec.t);
                rec.normal = (rec.p - center) / radius;
                rec.pigmentIndex = pigmentIndex;
                rec.finishIndex = finishIndex;
                return true;
            }
        }
        return false;
    }
};

#endif