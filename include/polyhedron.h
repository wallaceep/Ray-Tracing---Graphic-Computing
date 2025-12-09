#ifndef POLYHEDRON_H
#define POLYHEDRON_H

#include "object.h"
#include <vector>
#include <cmath>

struct Face {
    // Equação do plano: ax + by + cz + d = 0
    double a, b, c, d;
    Vec3 normal;

    Face(double a, double b, double c, double d) : a(a), b(b), c(c), d(d) {
        normal = Vec3(a, b, c).normalize();
    }
};

class Polyhedron : public Object {
public:
    std::vector<Face> faces;
    int pigmentIndex;
    int finishIndex;

    Polyhedron(int pigIdx, int finIdx) : pigmentIndex(pigIdx), finishIndex(finIdx) {}

    void add_face(double a, double b, double c, double d) {
        faces.push_back(Face(a, b, c, d));
    }

    // Algoritmo de interseção para Poliedros Convexos
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
        double t_enter = -1e9; // Começa no -infinito
        double t_exit = 1e9;   // Começa no +infinito
        const Face* enter_face = nullptr;

        for (const auto& face : faces) {
            // O denominador é o produto escalar da direção do raio com a normal do plano
            // Isso diz o quão alinhado o raio está com o plano)
            double denom = dot(face.normal, r.direction);
            
            // Numerador da equação de interseção t = -(P0 . N + d) / (D . N)
            double dist = -(dot(face.normal, r.origin) + face.d);

            // Raio paralelo ao plano
            if (std::abs(denom) < 1e-6) {
                // Se o raio é paralelo e a origem está "fora" do plano, ele erra o objeto todo
                if (dist < 0) return false; 
            } else {
                double t = dist / denom;

                if (denom < 0) {
                    // Entrando no semi-espaço
                    if (t > t_enter) {
                        t_enter = t;
                        enter_face = &face;
                    }
                } else {
                    // Saindo do semi-espaço
                    if (t < t_exit) {
                        t_exit = t;
                    }
                }
            }
        }

        // Verifica se a interseção é válida
        if (t_enter < t_exit && t_exit > t_min) {
            double t = t_enter;
            // Se a entrada está atrás da câmera, verificamos a saída (estamos dentro do objeto)
            if (t < t_min) {
                t = t_exit;
                 // Nota: Se estamos saindo, a normal deveria ser invertida, 
                 // mas para convexos opacos simples, focamos na entrada.
            }

            if (t > t_min && t < t_max) {
                rec.t = t;
                rec.p = r.pointAt(t);
                if (enter_face) rec.normal = enter_face->normal;
                else rec.normal = faces[0].normal; // Fallback
                
                rec.pigmentIndex = pigmentIndex;
                rec.finishIndex = finishIndex;
                return true;
            }
        }

        return false;
    }
};

#endif