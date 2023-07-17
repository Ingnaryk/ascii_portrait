#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#include "argparser.hpp"
#include "image_handle.h"
#include "portrait.h"

constexpr const char *kernel_name = "kernels.txt";
constexpr const char *gray_name = "images/gray.jpg";
constexpr const char *convo_name = "images/convo.jpg";
constexpr const char *portrait_image_name = "images/portrait.jpg";
constexpr const char *portrait_name = "portrait.txt";
constexpr auto stb_deleter = [](uint8_t *ptr)
{ stbi_image_free(ptr); };
enum class Flag
{
    Success,
    ArgumentError,
    FailToLoad,
    UnsupportedFormat,
    OutputSizeTooLarge
};
std::string intput_file, output_size;
int quality;
bool debug;

int main(int argc, const char *argv[])
{
    argparser::Parser &parser = argparser::init("An ascii-portrait learning program");
    parser.flag(&intput_file, "--input", "-i", "Input file name");
    parser.flag(&output_size, "--output", "-o", "Output portrait's size(widthxheight)", "30x15");
    parser.flag(&convo_threshold, "--convo-thres", "-cot", "The threshold of convolution dividing black and white", "55");
    parser.flag(&char_threshold, "--char-thres", "-cht", "The threshold from the image block to a char", "0.04");
    parser.flag(&quality, "--quality", "-q", "The quality of intermediately generated images", "95");
    parser.flag(&debug, "--debug", "-d", "Debug mode for which program will print further runtime information", "0");
    if (!parser.parse(argc, argv))
    {
        parser.print_promt();
        return (int)Flag::ArgumentError;
    }

    int iwidth, iheight, channels;
    std::unique_ptr<uint8_t, decltype(stb_deleter)> source{stbi_load(intput_file.data(), &iwidth, &iheight, &channels, default_channels), stb_deleter};
    if (!source)
        return (int)Flag::FailToLoad;
    image_source = source.get();
    if (channels != default_channels)
        return (int)Flag::UnsupportedFormat;
    int owidth, oheight;
    sscanf(output_size.data(), "%dx%d", &owidth, &oheight);
    if (owidth * oheight * ascii_shape_size * ascii_shape_size > iwidth * iheight)
        return (int)Flag::OutputSizeTooLarge;
    printf("Input image: %dx%d\n", iwidth, iheight);
    printf("Output portrait: %dx%d\n", owidth, oheight);

    std::vector<uint8_t> gray(iwidth * iheight);
    std::vector<uint8_t> convo(iwidth * iheight);
    std::vector<uint8_t> portrait_image(owidth * oheight * ascii_shape_size * ascii_shape_size * default_channels);
    std::vector<char> portrait(owidth * oheight);
    std::vector<int8_t> kernels = read_kernels(kernel_name);

    rgb2gray(source.get(), gray.data(), iwidth, iheight);
    stbi_write_jpg(gray_name, iwidth, iheight, default_channels, image_source, quality);

    convolution(gray.data(), convo.data(), iwidth, iheight, kernels.data(), kernels.size() / (kernel_size * kernel_size));
    stbi_write_jpg(convo_name, iwidth, iheight, default_channels, image_source, quality);

    stbir_resize_uint8(image_source, iwidth, iheight, 0, portrait_image.data(), owidth * ascii_shape_size, oheight * ascii_shape_size, 0, default_channels);
    stbi_write_jpg(portrait_image_name, owidth * ascii_shape_size, oheight * ascii_shape_size, default_channels, portrait_image.data(), quality);

    rgb2gray(portrait_image.data(), convo.data(), owidth * ascii_shape_size, oheight * ascii_shape_size);
    image2portrait(convo.data(), portrait.data(), owidth, oheight);
    save_portrait(portrait_name, portrait.data(), owidth, oheight);

    return (int)Flag::Success;
}
