#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include <cmath>

class Camera {
public:
    Vec3 origin;          // Posição do olho
    Vec3 lower_left_corner; // Canto inferior esquerdo da tela virtual no espaço 3D
    Vec3 horizontal;      // Vetor que representa a largura total da tela
    Vec3 vertical;        // Vetor que representa a altura total da tela

    // vfov: abertura vertical em graus (field of view)
    // aspect: razão largura/altura da imagem
    Camera(Vec3 lookfrom, Vec3 lookat, Vec3 vup, double vfov, double aspect) {
        origin = lookfrom;

        // 1. Converter FOV de graus para radianos e calcular altura da tela virtual
        double theta = vfov * 3.141592 / 180.0;
        double half_height = tan(theta / 2.0);
        double half_width = aspect * half_height;

        // 2. Construir a base ortonormal (u, v, w) da câmera
        // w aponta para TRÁS (oposto ao alvo)
        Vec3 w = (lookfrom - lookat).normalize(); 
        
        // u aponta para a DIREITA (produto vetorial de up e w)
        Vec3 u = cross(vup, w).normalize();
        
        // v aponta para CIMA (produto vetorial de w e u)
        Vec3 v = cross(w, u); // w e u já são unitários e ortogonais

        // 3. Definir o viewport (tela virtual)
        // O viewport fica em: Origin - half_width*u - half_height*v - w
        // (o "-w" significa que a tela está a 1 unidade de distância na frente do olho)
        
        // Vetores que varrem a tela inteira
        horizontal = 2.0 * half_width * u;
        vertical = 2.0 * half_height * v;

        // Canto inferior esquerdo da tela
        lower_left_corner = origin - (half_width * u) - (half_height * v) - w;
    }

    // Gera um raio para uma coordenada de textura (s, t) onde s,t variam de 0 a 1
    Ray get_ray(double s, double t) {
        // Direção = Ponto no alvo - Origem
        // Ponto no alvo = Canto + (s * largura) + (t * altura)
        Vec3 direction = lower_left_corner + (s * horizontal) + (t * vertical) - origin;
        return Ray(origin, direction.normalize());
    }
};

#endif