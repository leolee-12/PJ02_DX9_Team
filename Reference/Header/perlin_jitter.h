// perlin_jitter.h
// 외부 헤더입니다요
#pragma once

#include <vector>
#include <random>
#include <numeric>
#include <cmath>

struct Vec2 {
    float x, y;
};

class PerlinJitter {
private:
    std::vector<int> p;
    float amplitude_x = 1.0f, amplitude_y = 1.0f;
    float frequency_x = 0.5f, frequency_y = 0.5f;
    float seed_offset_x = 0.0f, seed_offset_y = 0.0f;

    float fade(float t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float lerp(float t, float a, float b) const {
        return a + t * (b - a);
    }

    float grad(int hash, float x) const {
        return (hash & 1) == 0 ? x : -x;
    }

    float noise1d(float x) const {
        int ix = static_cast<int>(std::floor(x)) & 255;
        float fx = x - std::floor(x);
        float u = fade(fx);
        return lerp(u, grad(p[ix], fx), grad(p[ix + 1], fx - 1.0f)) * 2.0f;
    }

    float random_float(float min, float max) const {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

public:
    PerlinJitter() {
        p.resize(512);
        std::vector<int> p_temp(256);
        std::iota(p_temp.begin(), p_temp.end(), 0);
        std::shuffle(p_temp.begin(), p_temp.end(), std::mt19937{std::random_device{}()});
        for (int i = 0; i < 256; ++i)
            p[i] = p[i + 256] = p_temp[i];
        reset();
    }

    void reset() {
        amplitude_x = random_float(0.8f, 1.2f);
        amplitude_y = random_float(0.8f, 1.2f);
        frequency_x = random_float(0.4f, 0.7f);
        frequency_y = random_float(0.4f, 0.7f);
        seed_offset_x = random_float(0.0f, 100000.0f);
        seed_offset_y = random_float(0.0f, 100000.0f);
    }

    Vec2 get_offset(float t) const {
        float x = noise1d(t * frequency_x + seed_offset_x) * amplitude_x;
        float y = noise1d(t * frequency_y + seed_offset_y) * amplitude_y;
        return {x, y};
    }
};
