#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class Vec3 {
public:
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    // Operações Básicas de Vetor
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(double t) const { return Vec3(x * t, y * t, z * t); }
    Vec3 operator/(double t) const { return Vec3(x / t, y / t, z / t); }
    
    // Produto de componentes 
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }

    // Comprimento do vetor 
    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return x*x + y*y + z*z;
    }

    // Normalização 
    Vec3 normalize() const {
        double len = length();
        if (len > 0) return *this / len;
        return *this; // Evita divisão por zero
    }
};

// Produto Escalar (Dot Product) - para iluminação e textura 
inline double dot(const Vec3& u, const Vec3& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// Produto Vetorial (Cross Product) - para o sistema de coordenadas da câmera 
inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return Vec3(u.y * v.z - u.z * v.y,
                u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

// Sobrecarga para permitir "double * Vec3" 
inline Vec3 operator*(double t, const Vec3& v) {
    return Vec3(t * v.x, t * v.y, t * v.z);
}

#endif