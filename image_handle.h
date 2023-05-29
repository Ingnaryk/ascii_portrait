#pragma once

#include <stdexcept>
#include <fstream>
#include <numeric>

constexpr int kernel_size = 3;
constexpr int kernel_size_half = kernel_size / 2;
constexpr int default_channels = 3;
inline uint8_t *image_source{nullptr};
inline int convo_threshold;

void rgb2gray(uint8_t *__restrict__ input_rgb, uint8_t *__restrict__ output_gray, int width, int height)
{
    for (int i = 0; i < width * height; ++i)
    {
        *output_gray = std::accumulate(input_rgb + i * default_channels, input_rgb + (i + 1) * default_channels, 0) / default_channels;
        std::fill_n(input_rgb + i * default_channels, default_channels, *output_gray);
        ++output_gray;
    }
}

void convolution(uint8_t *__restrict__ input_gray, uint8_t *__restrict__ output_gray, int width, int height, int8_t *kernels, int kernel_num)
{
    if (kernel_size % 2 != 1)
        throw std::invalid_argument{"Kernel size must be odd!"};

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int loc = y * width + x;
            int rst = 0;
            for (int8_t *kernel_ptr = kernels; kernel_ptr < kernels + kernel_num * kernel_size * kernel_size; kernel_ptr += kernel_size * kernel_size)
            {
                for (int j = -kernel_size_half; j <= kernel_size_half; ++j)
                {
                    for (int i = -kernel_size_half; i <= kernel_size_half; ++i)
                    {
                        int con_loc = (y + j) * width + (x + i);
                        rst += (con_loc < 0 ? 0 : (con_loc > width * height ? 0 : input_gray[con_loc])) * kernel_ptr[(j + kernel_size_half) * kernel_size + i + kernel_size_half];
                    }
                }
            }
            *output_gray = (rst < convo_threshold ? 0 : (rst > 255 - convo_threshold ? 255 : rst));
            std::fill_n(image_source + loc * default_channels, default_channels, *output_gray);
            ++output_gray;
        }
    }
}

std::vector<int8_t> read_kernels(const char *file)
{
    std::ifstream ifs{file};
    if (!ifs.is_open())
        throw std::runtime_error{"No such file!"};

    int read, kernel_num;
    ifs >> kernel_num;
    std::vector<int8_t> kernels(kernel_size * kernel_size * kernel_num);
    for (int i = 0; i < kernel_size * kernel_size * kernel_num; ++i)
    {
        ifs >> read;
        kernels[i] = read;
    }
    ifs.close();
    return std::move(kernels);
}

void save_portrait(const char *file, char *portrait, int width, int height)
{
    std::ofstream ofs{file};
    if (!ofs.is_open())
        throw std::runtime_error{"Cannot write file!"};

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            ofs << portrait[y * width + x];
        }
        ofs << "\n";
    }
    ofs.close();
}