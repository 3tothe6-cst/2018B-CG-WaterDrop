#include <ctime>
#include <limits>

#include "scenes.hpp"


bool should_stop() {
    std::FILE *stop = std::fopen("stop", "rb");
    if (stop) {
        std::fclose(stop);
        std::remove("stop");
    }
    return stop;
}

int main() {
    std::minstd_rand rng{static_cast<uint_fast32_t>(std::time(nullptr))};
    auto scene = scenes::threebody::scene(rng);
    auto camera = scenes::threebody::camera(rng, scene);
    Screen screen;
    if (!screen.from_file("out/scene.dat"))
        screen.initialize_data(1600, 1200);
    for (std::size_t i=0; !(i%16==0&&should_stop()); ++i)
        screen.capture(camera);
    screen.to_file("out/scene.dat");
}
