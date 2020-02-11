#pragma once

#include <cmath>
#include <random>
#include <utility>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <boost/math/constants/constants.hpp>
#include <unsupported/Eigen/Polynomials>


constexpr double EPS = 1e-8;

inline Eigen::Vector3d proj(const Eigen::Vector3d &v,
        const Eigen::Vector3d &n) { return v.dot(n)/n.dot(n) * n; }
inline Eigen::Vector3d vert(const Eigen::Vector3d &v) {
    if (std::abs(v.y()) <= std::abs(v.x()))
        return {v.z(), 0., -v.x()};
    return {0., v.z(), -v.y()};
}

struct Ray {
    Eigen::Vector3d o, d;
};

struct RayTransformer {
    const Eigen::Vector3d &d, &n;
    Eigen::Vector3d diffuse_reflect(std::minstd_rand &rng) {
        using uniform = std::uniform_real_distribution<>;
        using boost::math::constants::pi;
        Eigen::Vector3d x = vert(n).normalized(), y = n.cross(x);
        double theta_cosine = uniform(0., 1.)(rng);
        double theta_sine = std::sqrt(1. - std::pow(theta_cosine, 2));
        double phi = uniform(0., 2*pi<double>())(rng);
        return (x*std::cos(phi)*theta_sine
            + y*std::sin(phi)*theta_sine + n*theta_cosine);
    }
    Eigen::Vector3d specular_reflect() { return d - 2 * proj(d, n); }
    std::pair<Eigen::Vector3d, double> refract(
            double nr, std::minstd_rand &rng) {
        using discrete = std::discrete_distribution<>;
        double i_cos2 = std::pow(d.dot(n), 2) / (d.dot(d)*n.dot(n));
        double r_cos2 = 1. - (1.-i_cos2)*nr*nr;
        if (r_cos2 <= 0.)
            return {specular_reflect(), 1.};
        double R0 = std::pow((nr-1.)/(nr+1.), 2);
        double Ki = R0 + (1.-R0)*std::pow(1.-std::sqrt(i_cos2), 5);
        double Kr = 1. - (R0 + (1.-R0)*std::pow(1.-std::sqrt(r_cos2), 5));
        Eigen::Vector3d d_new;
        if (discrete({Ki, Kr})(rng) == 0)
            d_new = specular_reflect();
        else
            d_new = (d + (d - proj(d, n)) * (
                std::sqrt((1./r_cos2-1.)/(1./i_cos2-1.))-1.)).normalized();
        return {d_new, Ki+Kr};
    }
};

namespace shapes {
struct Sphere {
    Eigen::Vector3d o; double r;
    double intersects_with(const Ray &ray) const {
        Eigen::Vector3d rel = ray.o - this->o;
        double qea = ray.d.dot(ray.d), qebh = ray.d.dot(rel), 
               qec = rel.dot(rel) - r*r, det = qebh*qebh - qea*qec;
        if (det <= 0.)
            return 0.;
        det = std::sqrt(det); double t;
        return (t=-qebh-det) > EPS || (t=-qebh+det) > EPS ? t : 0.;
    }
    bool __is_inside(const Ray &ray,
            const Eigen::Vector3d &z) const { return ray.d.dot(z) > 0.; }
    bool is_inside(
            const Ray &ray) const { return __is_inside(ray, ray.o-this->o); }
    Eigen::Vector3d normal(const Ray &ray) const {
        Eigen::Vector3d z = ray.o - this->o;
        if (__is_inside(ray, z))
            z *= -1.;
        return z / r;
    }
};

struct DefiniteRectangle {
    Eigen::Vector3d o, n, a, b; double am, bm;
    double intersects_with(const Ray &ray) const {
        Eigen::Vector3d rel = ray.o - this->o;
        double p = ray.d.dot(this->n), q = -rel.dot(this->n), t;
        if (std::abs(p) <= EPS || (t=q/p) <= EPS)
            return 0.;
        Eigen::Vector3d where = rel + t*ray.d;
        double wa = where.dot(a), wb = where.dot(b);
        return 0. < wa && wa < am && 0. < wb && wb < bm ? t : 0.;
    }
    Eigen::Vector3d normal(const Ray &ray) const {
        return ray.d.dot(this->n) > 0. ? -this->n : this->n;
    }
};

struct WaterDrop {
    Eigen::Vector3d o; double s;
    double __with_parallel_ray(const Ray &ray) const {
        Eigen::Vector3d rel = ray.o - this->o;
        if (!((rel.y() <= 0.) ^ (rel.y() <= 6.*s)))
            return 0.;
        double u = std::sqrt(rel.y()/(6.*s));
        double r2 = 9.*s*s*u*u*(1.-u)*(1.-u)*(2.+u)*(2.+u);
        Eigen::Vector2d d2d{ray.d.x(), ray.d.z()}, rel2d{rel.x(), rel.z()};
        double qea = d2d.dot(d2d), qebh = d2d.dot(rel2d),
               qec = rel2d.dot(rel2d) - r2, det = qebh*qebh - qea*qec;
        if (det <= 0.)
            return 0.;
        det = std::sqrt(det); double t;
        return (t=-qebh-det) > EPS || (t=-qebh+det) > EPS ? t : 0.;
    }
    double intersects_with(const Ray &ray) const {
        if (std::abs(ray.d.y()) <= EPS)
            return __with_parallel_ray(ray);
        Eigen::Vector3d rel = ray.o - this->o;
        if (ray.d.y() > 0. && rel.y() >= 0. && rel.y() >= 6.*s)
            return 0.;
        if (ray.d.y() < 0. && rel.y() <= 0. && rel.y() <= 6.*s)
            return 0.;
        double A = 6.*s*ray.d.x()/ray.d.y();
        double B = -rel.y()*ray.d.x()/ray.d.y()+rel.x();
        double C = 6.*s*ray.d.z()/ray.d.y();
        double D = -rel.y()*ray.d.z()/ray.d.y()+rel.z();
        double X = std::pow(A*D-B*C, 2)/(A*A+C*C);
        double Y = std::pow((14*std::sqrt(7)-20)*s/9, 2);
        if (X >= Y)
            return 0.;
        Eigen::Matrix<double, 7, 1> poly;
        poly << -B*B-D*D, 0., 36.*s*s-2*A*B-2*C*D, -36.*s*s,
                -27.*s*s-A*A-C*C, 18.*s*s, 9.*s*s;
        Eigen::PolynomialSolver<double, 6> ps; ps.compute(poly);
        std::vector<double> u_roots; ps.realRoots(u_roots);
        double t = 0.; bool t_found = false;
        for (auto u : u_roots) {
            if (u <= 0. || u >= 1.)
                continue;
            double _t = (6.*s*u*u-rel.y()) / ray.d.y();
            if (_t > EPS && (!t_found || _t < t)) {
                t_found = true;
                t = _t;
            }
        }
        return t;
    }
    Eigen::Vector3d __z(const Ray &ray) const {
        Eigen::Vector3d rel = ray.o - this->o;
        double u = std::sqrt(rel.y()/(6.*s));
        Eigen::Vector3d x_d_{rel.x(), 0., rel.z()}; x_d_.normalize();
        if (s < 0.)
            x_d_ *= -1;
        double dx = 12.*u*s, dy = (9.*u*u+6.*u-6.)*s;
        return dx*x_d_ + dy*Eigen::Vector3d{0., 1., 0.};
    }
    bool __is_inside(const Ray &ray,
            const Eigen::Vector3d &z) const { return ray.d.dot(z) > 0.; }
    bool is_inside(
            const Ray &ray) const { return __is_inside(ray, __z(ray)); }
    Eigen::Vector3d normal(const Ray &ray) const {
        Eigen::Vector3d z = __z(ray);
        if (__is_inside(ray, z))
            z *= -1.;
        return z.normalized();
    }
};

}
