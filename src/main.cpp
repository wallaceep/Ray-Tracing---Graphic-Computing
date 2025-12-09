#include <iostream>
#include <fstream>
#include <algorithm> 
#include <cmath>
#include "scene.h"

// Protótipo da função parser 
bool loadScene(const std::string& filename, Scene& scene);

// Auxiliar para limitar valores entre 0 e 1 (clamp)
double clamp(double x) { return x < 0 ? 0 : (x > 1 ? 1 : x); }
Vec3 clamp_color(Vec3 c) { return Vec3(clamp(c.x), clamp(c.y), clamp(c.z)); }

// Refletir um vetor em torno de uma normal
Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * dot(v, n) * n;
}

// Refratar um vetor (Lei de Snell)
bool refract(const Vec3& v, const Vec3& n, double ni_over_nt, Vec3& refracted) {
    Vec3 uv = v.normalize();
    double dt = dot(uv, n);
    double discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    return false; // Reflexão interna total
}

Vec3 get_pigment_color(const Pigment& pig, const Vec3& p) {
    if (pig.type == SOLID) {
        return pig.color;
    } 
    else if (pig.type == CHECKER) {
        double s = pig.cube_size;
        // Adiciona um pequeno epsilon para estabilidade numérica nas bordas
        int cx = (int)floor((p.x + 0.0001) / s);
        int cy = (int)floor((p.y + 0.0001) / s);
        int cz = (int)floor((p.z + 0.0001) / s);

        if ((cx + cy + cz) % 2 == 0) return pig.color;
        else return pig.color2;
    } 
    else if (pig.type == TEXMAP) {
        // Se a textura não foi carregada corretamente, retorna rosa erro
        if (!pig.textureData) return Vec3(1, 0, 1);

        // P0 e P1 são os vetores de 4 elementos lidos do arquivo
        
        // s = P0 . PC (onde PC é x, y, z, 1)
        double s = pig.tex_params[0] * p.x + 
                   pig.tex_params[1] * p.y + 
                   pig.tex_params[2] * p.z + 
                   pig.tex_params[3]; 

        // r = P1 . PC
        double r = pig.tex_params[4] * p.x + 
                   pig.tex_params[5] * p.y + 
                   pig.tex_params[6] * p.z + 
                   pig.tex_params[7]; 

        return pig.textureData->sample(s, r);
    }
    return Vec3(0, 0, 0);
}

Vec3 cast_ray(const Ray& r, Scene& scene, int depth) {
    // Limite de recursão para evitar loop infinito e estouro de pilha
    if (depth > 5) return Vec3(0,0,0); 

    HitRecord rec;
    bool hit_anything = false;
    double closest_so_far = 999999.0;
    HitRecord temp_rec;

    // 1. Interseção com a cena (encontrar o objeto mais próximo)
    for (Object* obj : scene.objects) {
        if (obj->hit(r, 0.001, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    if (!hit_anything) return Vec3(0.0, 0.0, 0.0); // Fundo preto

    const Pigment& pig = scene.pigments[rec.pigmentIndex];
    const Finish& fin = scene.finishes[rec.finishIndex];
    
    Vec3 P = rec.p;
    Vec3 N = rec.normal;
    Vec3 V = -r.direction.normalize(); 
    
    Vec3 obj_color = get_pigment_color(pig, P); 

    Vec3 local_color = fin.ka * scene.ambient_light * obj_color; // Ambiente

    for (const auto& light : scene.lights) {
        Vec3 L_vec = light.position - P;
        double dist = L_vec.length();
        Vec3 L = L_vec.normalize();

        // Sombra
        Ray shadow_ray(P, L);
        bool in_shadow = false;
        HitRecord srec;
        for (Object* obj : scene.objects) {
            if (obj->hit(shadow_ray, 0.001, dist, srec)) {
                in_shadow = true;
                break;
            }
        }

        if (!in_shadow) {
            double n_dot_l = std::max(0.0, dot(N, L));
            Vec3 diffuse = fin.kd * n_dot_l * (light.color * obj_color);
            
            Vec3 R = reflect(-L, N);
            double r_dot_v = std::max(0.0, dot(R, V));
            Vec3 specular = fin.ks * pow(r_dot_v, fin.alpha) * light.color;

            double att = 1.0 / (light.attenuation[0] + light.attenuation[1]*dist + light.attenuation[2]*dist*dist);
            local_color = local_color + (diffuse + specular) * att;
        }
    }

    Vec3 final_color = local_color;

    // --- Componente Global: Reflexão (kr) --- 
    if (fin.kr > 0) {
        Vec3 reflected_dir = reflect(r.direction.normalize(), N);
        Ray reflected_ray(P, reflected_dir);
        Vec3 reflection_color = cast_ray(reflected_ray, scene, depth + 1);
        final_color = final_color + fin.kr * reflection_color;
    }

    // --- Componente Global: Transmissão/Refração (kt) --- 
    if (fin.kt > 0) {
        Vec3 outward_normal;
        double ni_over_nt;
        Vec3 refracted_dir;
        
        // Verifica se o raio está entrando ou saindo do objeto
        if (dot(r.direction, N) > 0) {
            outward_normal = -N;
            ni_over_nt = fin.ior; // Saindo: n2 / n1 
        } else {
            outward_normal = N;
            ni_over_nt = 1.0 / fin.ior; // Entrando: n1 / n2
        }

        if (refract(r.direction, outward_normal, ni_over_nt, refracted_dir)) {
            Ray refracted_ray(P, refracted_dir);
            Vec3 refraction_color = cast_ray(refracted_ray, scene, depth + 1);
            final_color = final_color + fin.kt * refraction_color;
        } 
    }

    return clamp_color(final_color);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_cena> [output.ppm]" << std::endl;
        return 1;
    }

    Scene scene;
    if (loadScene(argv[1], scene)) {
        // Dimensões da imagem (padrão 800x600 ou lidas da câmera se implementado)
        int nx = 800;
        int ny = 600;

        std::string output_file = (argc >= 3) ? argv[2] : "output.ppm";
        std::ofstream out_file(output_file);
        
        // Cabeçalho PPM
        out_file << "P3\n" << nx << " " << ny << "\n255\n";

        std::cout << "Renderizando " << nx << "x" << ny << " para " << output_file << "..." << std::endl;

        // Loop de Renderização
        for (int j = ny - 1; j >= 0; j--) {
            if (j % 50 == 0) std::cout << "Linhas restantes: " << j << std::endl;

            for (int i = 0; i < nx; i++) {
                double u = double(i) / double(nx);
                double v = double(j) / double(ny);

                Ray r = scene.camera->get_ray(u, v);
                Vec3 col = cast_ray(r, scene, 0);

                int ir = int(255.99 * col.x);
                int ig = int(255.99 * col.y);
                int ib = int(255.99 * col.z);

                out_file << ir << " " << ig << " " << ib << "\n";
            }
        }
        out_file.close();
        std::cout << "Concluido!" << std::endl;
    } else {
        std::cerr << "Falha ao carregar a cena." << std::endl;
        return 1;
    }

    return 0;
}