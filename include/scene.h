#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "vec3.h"
#include "camera.h"
#include "object.h" 
#include "sphere.h"

struct Texture {
    int width, height;
    std::vector<Vec3> pixels; // Armazena RGB
    
    // Função para pegar a cor nas coordenadas (u, v)
    Vec3 sample(double u, double v) const {
        if (pixels.empty()) return Vec3(1, 0, 1); // Rosa de erro

        // Tratamento de repetição (tiling)
        u = u - floor(u);
        v = v - floor(v);

        // Mapeia 0..1 para 0..width-1
        int i = int(u * width);
        int j = int(v * height);

        // Clamping de segurança
        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;

        return pixels[j * width + i];
    }
};

// Luzes
struct Light {
    Vec3 position;
    Vec3 color;
    double attenuation[3]; 
};

// Pigmentos 
enum PigmentType { SOLID, CHECKER, TEXMAP };

struct Pigment {
    PigmentType type;
    Vec3 color;             
    Vec3 color2;            
    double cube_size;       
    
    std::string tex_file;   
    double tex_params[8];   // Parâmetros P0 e P1 para projeção planar 
    
    Texture* textureData;   // Ponteiro para a imagem carregada

    Pigment() : textureData(nullptr) {}
};

struct Finish {
    double ka, kd, ks, alpha; 
    double kr, kt, ior;       
};

// --- A Cena Completa ---
struct Scene {
    Camera* camera;
    std::vector<Light> lights;
    std::vector<Pigment> pigments;
    std::vector<Finish> finishes;
    std::vector<Object*> objects;
    
    Vec3 ambient_light; 

    Scene() : camera(nullptr) {}
    
    ~Scene() {
        if (camera) delete camera;
        for (auto obj : objects) delete obj;
        for (auto& pig : pigments) {
            if (pig.textureData) delete pig.textureData;
        }
    }
};

#endif