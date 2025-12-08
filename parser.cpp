#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "scene.h"
#include "polyhedron.h"

using namespace std;

// --- Função Auxiliar para ler PPM ---
Texture* loadPPM(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir textura " << filename << endl;
        return nullptr;
    }

    string header;
    file >> header;

    if (header != "P3") {
        cerr << "Erro: Apenas formato PPM P3 (ASCII) eh suportado no momento. Arquivo: " << filename << endl;
        return nullptr;
    }

    Texture* tex = new Texture();
    int max_val;

    // Ignorar comentários (#) no PPM
    auto skip_comments = [&](ifstream& f) {
        char c;
        while (f >> ws && f.peek() == '#') {
            f.ignore(4096, '\n');
        }
    };

    skip_comments(file); file >> tex->width;
    skip_comments(file); file >> tex->height;
    skip_comments(file); file >> max_val;

    tex->pixels.resize(tex->width * tex->height);

    for (int i = 0; i < tex->width * tex->height; ++i) {
        int r, g, b;
        file >> r >> g >> b;
        tex->pixels[i] = Vec3(r / (double)max_val, g / (double)max_val, b / (double)max_val);
    }
    
    cout << "Textura carregada: " << filename << " (" << tex->width << "x" << tex->height << ")" << endl;
    return tex;
}

// --- Função Principal do Parser ---
bool loadScene(const string& filename, Scene& scene) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: Nao foi possivel abrir " << filename << endl;
        return false;
    }

    // 1. Câmera 
    Vec3 eye, at, up;
    double fov;
    file >> eye.x >> eye.y >> eye.z;
    file >> at.x >> at.y >> at.z;
    file >> up.x >> up.y >> up.z;
    file >> fov;

    scene.camera = new Camera(eye, at, up, fov, 1.333);

    // 2. Luzes 
    int num_lights;
    file >> num_lights;
    for (int i = 0; i < num_lights; ++i) {
        Light l;
        file >> l.position.x >> l.position.y >> l.position.z;
        file >> l.color.x >> l.color.y >> l.color.z;
        file >> l.attenuation[0] >> l.attenuation[1] >> l.attenuation[2];
        
        if (i == 0) scene.ambient_light = l.color;
        else scene.lights.push_back(l);
    }

    // 3. Pigmentos 
    int num_pigments;
    file >> num_pigments;
    for (int i = 0; i < num_pigments; ++i) {
        Pigment p;
        string type;
        file >> type; 

        if (type == "solid") {
            p.type = SOLID;
            file >> p.color.x >> p.color.y >> p.color.z;
        } else if (type == "checker") {
            p.type = CHECKER;
            file >> p.color.x >> p.color.y >> p.color.z;
            file >> p.color2.x >> p.color2.y >> p.color2.z;
            file >> p.cube_size;
        } else if (type == "texmap") {
            p.type = TEXMAP;
            file >> p.tex_file;
            for (int k = 0; k < 8; k++) file >> p.tex_params[k];
            
            // Carregamento da textura
            p.textureData = loadPPM(p.tex_file);
        }
        scene.pigments.push_back(p);
    }

    // 4. Acabamentos e 5. Objetos 
    int num_finishes;
    file >> num_finishes;
    for (int i = 0; i < num_finishes; ++i) {
        Finish f;
        file >> f.ka >> f.kd >> f.ks >> f.alpha >> f.kr >> f.kt >> f.ior;
        scene.finishes.push_back(f);
    }

    int num_objects;
    file >> num_objects;
    for (int i = 0; i < num_objects; ++i) {
        int pig_idx, fin_idx;
        string obj_type;
        file >> pig_idx >> fin_idx >> obj_type;

        if (obj_type == "sphere") {
            Vec3 center;
            double radius;
            file >> center.x >> center.y >> center.z >> radius;
            scene.objects.push_back(new Sphere(center, radius, pig_idx, fin_idx));
        } 
        else if (obj_type == "polyhedron") {
            int num_faces;
            file >> num_faces;
            Polyhedron* poly = new Polyhedron(pig_idx, fin_idx);
            for (int k = 0; k < num_faces; k++) {
                double a, b, c, d;
                file >> a >> b >> c >> d;
                poly->add_face(a, b, c, d);
            }
            scene.objects.push_back(poly);
        }
    }

    return true;
}