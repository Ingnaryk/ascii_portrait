#pragma once

#include <numeric>
#include <algorithm>
#include <cstring>
#include <cstdio>

using uint8_t = unsigned char;

constexpr int ascii_shape_size = 3;
constexpr int ascii_shape[][ascii_shape_size][ascii_shape_size] = {{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
                                                                   {{0, 0, 0}, {1, 1, 1}, {0, 0, 0}},
                                                                   {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}},
                                                                   {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},
                                                                   {{0, 0, 1}, {0, 1, 0}, {1, 0, 0}}};
constexpr char ascii_char[] = {' ', '-', '|', '\\', '/'};
constexpr int char_num = sizeof(ascii_char);
float char_threshold;
extern bool debug;

char shape2char(uint8_t *shape_gray)
{
    float results[char_num], temp_results[ascii_shape_size * ascii_shape_size];
    constexpr auto binary_op = [](uint8_t gray, int val)
    { return std::abs(gray / 255.0f - val * char_threshold); };
#pragma unroll
    for (int i = 0; i < char_num; ++i)
    {
        std::transform(shape_gray, shape_gray + ascii_shape_size * ascii_shape_size, (const short *)(ascii_shape + i), temp_results, binary_op);
        results[i] = std::accumulate(temp_results, temp_results + ascii_shape_size * ascii_shape_size, 0.0f);
        if (debug)
            printf("%f ", results[i]);
    }
    if (debug)
        printf("\n");
    return ascii_char[std::min_element(results, results + char_num) - results];
}

void image2portrait(uint8_t *__restrict__ input_gray, char *__restrict__ output, int width, int height)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            uint8_t shape_gray[ascii_shape_size * ascii_shape_size];
#pragma unroll
            for (int i = 0; i < ascii_shape_size; ++i)
                memcpy(shape_gray + ascii_shape_size * i, input_gray + (y * ascii_shape_size + i) * width * ascii_shape_size + x * ascii_shape_size, sizeof(uint8_t) * ascii_shape_size);
            output[y * width + x] = shape2char(shape_gray);
        }
    }
}