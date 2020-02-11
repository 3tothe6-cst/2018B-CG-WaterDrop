#pragma once

#include "objects.hpp"


namespace scenes {
namespace threebody {
inline Scene scene(std::minstd_rand &rng) {
    using R = objects::DefiniteRectangleSCO;
    using LED = objects::LEDSCO;
    using ST = objects::SphereT;
    using WO = objects::WaterDropSCO;
    auto bottom = std::make_unique<R>();
    auto top = std::make_unique<R>();
    auto toptop = std::make_unique<R>();
    auto front = std::make_unique<LED>();
    auto spherel = std::make_unique<ST>();
    auto spherem = std::make_unique<ST>();
    auto spherer = std::make_unique<ST>();
    auto waterdropb = std::make_unique<WO>();
    auto waterdropt = std::make_unique<WO>();
    bottom->base.o << 0., 150., 0.; bottom->base.n << 0., 0., 1.;
    bottom->base.a << 0., -1., 0.; bottom->base.b << 1., 0., 0.;
    bottom->base.am = bottom->base.bm = 300.;
    top->base.o << 75., 75., 600.; top->base.n << 0., 0., 1.;
    top->base.a << 0., -1., 0.; top->base.b << 1., 0., 0.;
    top->base.am = top->base.bm = 150.;
    toptop->base.o << 0., 150., 602.; toptop->base.n << 0., 0., 1.;
    toptop->base.a << 0., -1., 0.; toptop->base.b << 1., 0., 0.;
    toptop->base.am = toptop->base.bm = 300.;
    front->base.o << 300., 150., 0.; front->base.n << -1., 0., 0.;
    front->base.a << 0., -1., 0.; front->base.b << 0., 0., 1.;
    front->base.am = 300.; front->base.bm = 225.;
    front->base.from_file("threebody.dat"); front->base.d = 0.15;
    spherel->base.o << 215.1, 84.9, 20.5; spherel->base.r = 20.;
    spherem->base.o << 180., 0., 20.5; spherem->base.r = 20.;
    spherer->base.o << 215.1, -84.9, 20.5; spherer->base.r = 20.;
    waterdropb->base.o << 225., 104., 100.; waterdropb->base.s = -6.;
    waterdropt->base.o << 225., -104., 200.; waterdropt->base.s = 6.;
    bottom->base._prop = {.1, .75, .15}; bottom->base._color << 1., 1., 1.;
    bottom->base._radiation << 0., 0., 0.; bottom->rng = &rng;
    top->base._prop = {.1, .0, .0};
    top->base._radiation << 24., 24., 18.; top->rng = &rng;
    toptop->base._prop = {.1, .9, .0};
    toptop->base._color << 1./16, 1./16, 1./16;
    toptop->base._radiation << 0., 0., 0.; toptop->rng = &rng;
    front->base._prop = {.1, .0, .0}; front->rng = &rng;
    spherel->prop = {.1, .9}; spherel->color << 1., 1., 1.;
    spherel->refract_index = 1.6; spherel->rng = &rng;
    spherem->prop = {.1, .9}; spherem->color << 1., 1., 1.;
    spherem->refract_index = 1.6; spherem->rng = &rng;
    spherer->prop = {.1, .9}; spherer->color << 1., 1., 1.;
    spherer->refract_index = 1.6; spherer->rng = &rng;
    waterdropb->base._prop = {.1, .0, .9};
    waterdropb->base._color << 1., 1., 1;
    waterdropb->base._radiation << 0., 0., 0.; waterdropb->rng = &rng;
    waterdropt->base._prop = {.1, .0, .9};
    waterdropt->base._color << 1., 1., 1;
    waterdropt->base._radiation << 0., 0., 0.; waterdropt->rng = &rng;
    Scene s;
    s.push_back(std::move(bottom));
    s.push_back(std::move(front));
    s.push_back(std::move(top));
    s.push_back(std::move(toptop));
    s.push_back(std::move(spherel));
    s.push_back(std::move(spherem));
    s.push_back(std::move(spherer));
    s.push_back(std::move(waterdropb));
    s.push_back(std::move(waterdropt));
    return s;
}
inline Camera camera(std::minstd_rand &rng, Scene &scene) {
    Camera c; c.e << -325., 0., 625.; c.n << 500., 0., -500.;
    c.a << 0., -1., 0.; c.b << 0.707, 0., 0.707; c.d = 3./16;
    c.rng = &rng; c.scene = &scene;
    return c;
}
}

namespace saturn {
inline Scene scene(std::minstd_rand &rng) {
    using R = objects::DefiniteRectangleSCO;
    using LED = objects::LEDSCO;
    using SO = objects::SphereSCO;
    auto bottom = std::make_unique<R>();
    auto top = std::make_unique<R>();
    auto light = std::make_unique<R>();
    auto front = std::make_unique<LED>();
    auto back = std::make_unique<R>();
    auto left = std::make_unique<R>();
    auto right = std::make_unique<R>();
    auto spherel = std::make_unique<SO>();
    auto spherem = std::make_unique<SO>();
    auto spherer = std::make_unique<SO>();
    bottom->base.o << 0., 150., 0.; bottom->base.n << 0., 0., 1.;
    bottom->base.a << 0., -1., 0.; bottom->base.b << 1., 0., 0.;
    bottom->base.am = bottom->base.bm = 300.;
    top->base.o << 0., 150., 225.; top->base.n << 0., 0., 1.;
    top->base.a << 0., -1., 0.; top->base.b << 1., 0., 0.;
    top->base.am = top->base.bm = 300.;
    light->base.o << 112.5, 37.5, 224.5; light->base.n << 0., 0., 1.;
    light->base.a << 0., -1., 0.; light->base.b << 1., 0., 0.;
    light->base.am = light->base.bm = 75.;
    front->base.o << 300., 150., 0.; front->base.n << -1., 0., 0.;
    front->base.a << 0., -1., 0.; front->base.b << 0., 0., 1.;
    front->base.am = 300.; front->base.bm = 225.;
    front->base.from_file("saturn.dat"); front->base.d = 0.05;
    left->base.o << 0., 150., 0.; left->base.n << 0., -1., 0.;
    left->base.a << 1., 0., 0.; left->base.b << 0., 0., 1.;
    left->base.am = 300.; left->base.bm = 225.;
    right->base.o << 0., -150., 0.; right->base.n << 0., 1., 0.;
    right->base.a << 1., 0., 0.; right->base.b << 0., 0., 1.;
    right->base.am = 300.; right->base.bm = 225.;
    spherel->base.o << 140.1, 84.9, 20.5; spherel->base.r = 20.;
    spherem->base.o << 105., 0., 20.5; spherem->base.r = 20.;
    spherer->base.o << 140.1, -84.9, 20.5; spherer->base.r = 20.;
    bottom->base._prop = {.1, .9, .0}; bottom->base._color << .75, .75, .75;
    bottom->base._radiation << 0., 0., 0.; bottom->rng = &rng;
    top->base._prop = {.1, .9, .0}; top->base._color << .75, .75, .75;
    top->base._radiation << 0., 0., 0.; top->rng = &rng;
    light->base._prop = {.1, .0, .0};
    light->base._radiation << 32., 32., 32.; light->rng = &rng;
    front->base._prop = {.1, .0, .0}; front->rng = &rng;
    left->base._prop = {.1, .9, .0}; left->base._color << .25, .25, .75;
    left->base._radiation << 0., 0., 0.; left->rng = &rng;
    right->base._prop = {.1, .9, .0}; right->base._color << .25, .75, .25;
    right->base._radiation << 0., 0., 0.; right->rng = &rng;
    spherel->base._prop = {.1, .9, .0}; spherel->base._color << 1., 1., 1.;
    spherel->base._radiation << 0., 0., 0.; spherel->rng = &rng;
    spherem->base._prop = {.1, .45, .45}; spherem->base._color << 1., 1., 1.;
    spherem->base._radiation << 0., 0., 0.; spherem->rng = &rng;
    spherer->base._prop = {.1, .0, .9}; spherer->base._color << 1., 1., 1.;
    spherer->base._radiation << 0., 0., 0.; spherer->rng = &rng;
    Scene s;
    s.push_back(std::move(bottom));
    s.push_back(std::move(top));
    s.push_back(std::move(light));
    s.push_back(std::move(front));
    s.push_back(std::move(left));
    s.push_back(std::move(right));
    s.push_back(std::move(spherel));
    s.push_back(std::move(spherem));
    s.push_back(std::move(spherer));
    return s;
}
inline Camera camera(std::minstd_rand &rng, Scene &scene) {
    Camera c; c.e << -600., 0., 112.5; c.n << 600.5, 0., 0.;
    c.a << 0., -1., 0.; c.b << 0., 0., 1.; c.d = 3./16;
    c.rng = &rng; c.scene = &scene;
    return c;
}
}

}
