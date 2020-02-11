#pragma once

#include <array>
#include <memory>
#include <vector>

#include "geometry.hpp"


struct Object {
    virtual ~Object() = default;
    virtual double intersects_with(const Ray &ray) = 0;
    virtual bool transmit(Eigen::Vector3d &radiation,
            Eigen::Vector3d &color, Ray &ray_new) = 0;
};

template<class OpaqueBase> struct OpaqueObject : Object {
    OpaqueBase base; std::minstd_rand *rng;
    void diffuse_reflect(Ray &ray_new) {
        auto rt = RayTransformer{ray_new.d, base.normal(ray_new)};
        ray_new.d = rt.diffuse_reflect(*rng);
    }
    void specular_reflect(Ray &ray_new) {
        auto rt = RayTransformer{ray_new.d, base.normal(ray_new)};
        ray_new.d = rt.specular_reflect();
    }
    double intersects_with(
            const Ray &ray) final { return base.intersects_with(ray); }
    bool transmit(Eigen::Vector3d &radiation,
            Eigen::Vector3d &color, Ray &ray_new) final {
        using discrete = std::discrete_distribution<>;
        radiation = base.radiation(ray_new.o);
        std::array<double, 3> prop = base.prop(ray_new.o);
        int mode = discrete(prop.begin(), prop.end())(*rng);
        if (mode == 0)
            return false;
        color = base.color(ray_new.o) / (1.-prop[0]);
        if (mode == 1)
            diffuse_reflect(ray_new);
        else
            specular_reflect(ray_new);
        return true;
    }
};

template<class TransparentBase> struct TransparentObject : Object {
    TransparentBase base; std::minstd_rand *rng;
    std::array<double, 2> prop; Eigen::Vector3d color; double refract_index;
    void refract(Ray &ray_new, Eigen::Vector3d &color_) {
        double nr = refract_index;
        if (!base.is_inside(ray_new))
            nr = 1. / nr;
        auto rt = RayTransformer{ray_new.d, base.normal(ray_new)};
        auto result = rt.refract(nr, *rng);
        ray_new.d = result.first; color_ *= result.second;
    }
    double intersects_with(
            const Ray &ray) final { return base.intersects_with(ray); }
    bool transmit(Eigen::Vector3d &radiation,
            Eigen::Vector3d &color_, Ray &ray_new) final {
        using discrete = std::discrete_distribution<>;
        radiation.fill(0.);
        int mode = discrete(prop.begin(), prop.end())(*rng);
        if (mode == 0)
            return false;
        color_ = this->color / (1.-prop[0]);
        refract(ray_new, color_);
        return true;
    }
};

struct SolidColorOpaqueBase {
    std::array<double, 3> _prop;
    Eigen::Vector3d _color, _radiation;
    auto prop(const Eigen::Vector3d &p) { (void) p; return _prop; }
    auto color(const Eigen::Vector3d &p) { (void) p; return _color; }
    auto radiation(const Eigen::Vector3d &p) { (void) p; return _radiation; }
};

struct LEDOpaqueBase : shapes::DefiniteRectangle {
    std::array<double, 3> _prop;
    std::vector<Eigen::Vector3d> data;
    std::int64_t xs, ys; double d;
    void from_file(const char *filename) {
        std::FILE *file = std::fopen(filename, "rb");
        std::fread(&xs, 8, 1, file);
        std::fread(&ys, 8, 1, file);
        data.resize(xs*ys);
        std::fread(data.data(), sizeof(Eigen::Vector3d), xs*ys, file);
        std::fclose(file);
    }
    auto prop(const Eigen::Vector3d &p) { (void) p; return _prop; }
    Eigen::Vector3d color(const Eigen::Vector3d &p) {
        (void) p; return {1., 1., 1.};
    }
    Eigen::Vector3d radiation(const Eigen::Vector3d &p) {
        Eigen::Vector3d rel = p - this->o;
        double wa = rel.dot(a), wb = rel.dot(b);
        std::int64_t x = wa / d, y = ys - wb / d;
        if (x < 0 || x >= xs || y < 0 || y >= ys)
            return {.0, .0, .0};
        return data[y*xs+x];
    }
};

struct Scene : std::vector<std::unique_ptr<Object>> {
    Eigen::Vector3d transmit(const Ray &ray) {
        Object *nearest = nullptr; double t = 0.;
        for (auto &obj : *this) {
            double _t = obj->intersects_with(ray);
            if (_t > 0. && (!nearest || _t < t)) {
                nearest = obj.get();
                t = _t;
            }
        }
        if (!nearest)
            return {0., 0., 0.};
        Eigen::Vector3d radiation, color; Ray ray_new{ray.o+t*ray.d, ray.d};
        if (nearest->transmit(radiation, color, ray_new))
            radiation += (color.array()*transmit(ray_new).array()).matrix();
        return radiation;
    }
};

struct Camera {
    Eigen::Vector3d e, n, a, b; double d;
    std::minstd_rand *rng; Scene *scene;
    Eigen::Vector3d transmit(std::int64_t _x, std::int64_t _y) {
        using uniform = std::uniform_real_distribution<>;
        uniform dist(0., d);
        double x = _x*d + dist(*rng), y = _y*d + dist(*rng);
        Eigen::Vector3d v = n + a*x + b*y;
        return scene->transmit({e, v.normalized()});
    }
};

struct Screen {
    std::vector<Eigen::Vector3d> data;
    std::int64_t xs, ys; std::int64_t count;
    auto &operator()(std::int64_t x, std::int64_t y) {
        return data[y*xs+x];
    }
    void initialize_data(std::int64_t _xs, std::int64_t _ys) {
        xs = _xs; ys = _ys; data.resize(xs*ys);
        for (auto &v : data)
            v.fill(0.);
        count = 0;
    }
    bool from_file(const char *filename) {
        std::FILE *file = std::fopen(filename, "rb");
        if (!file)
            return false;
        std::fread(&count, 8, 1, file);
        std::fread(&xs, 8, 1, file);
        std::fread(&ys, 8, 1, file);
        data.resize(xs*ys);
        std::fread(data.data(), sizeof(Eigen::Vector3d), data.size(), file);
        std::fclose(file);
        return true;
    }
    bool to_file(const char *filename) {
        std::FILE *file = std::fopen(filename, "wb");
        if (!file)
            return false;
        std::fwrite(&count, 8, 1, file);
        std::fwrite(&xs, 8, 1, file);
        std::fwrite(&ys, 8, 1, file);
        std::fwrite(data.data(), sizeof(Eigen::Vector3d), data.size(), file);
        std::fclose(file);
        return true;
    }
    void capture(Camera &camera) {
        std::int64_t xs_half = xs / 2, ys_half = ys / 2;
#pragma omp parallel for schedule(dynamic, 1)
        for (std::int64_t x=0; x<xs; ++x)
            for (std::int64_t y=0; y<ys; ++y)
                (*this)(x, y) += camera.transmit(x-xs_half, ys_half-y);
        ++count;
    }
};

namespace objects {
template<class A, class B> struct Inh2 : A, B {};
using SphereSCO = OpaqueObject<Inh2<shapes::Sphere, SolidColorOpaqueBase>>;
using SphereT = TransparentObject<shapes::Sphere>;
using DefiniteRectangleSCO = OpaqueObject<Inh2<
        shapes::DefiniteRectangle, SolidColorOpaqueBase>>;
using WaterDropSCO = OpaqueObject<Inh2<
        shapes::WaterDrop, SolidColorOpaqueBase>>;
using WaterDropT = TransparentObject<shapes::WaterDrop>;
using LEDSCO = OpaqueObject<LEDOpaqueBase>;
}
