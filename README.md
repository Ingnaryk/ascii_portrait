# An ascii-portrait learning program

One-command compilation: `g++ ./main.cpp -o main`

```plaintext
Flags:
    -i, --input
                       Input file name

    -o, --output
                       Output portrait's size(widthxheight) | default: 30x15

  -cot, --convo-thres
                       The threshold of convolution dividing black and white | default: 55

  -cht, --char-thres
                       The threshold from the image block to a char | default: 0.04

    -q, --quality
                       The quality of intermediately generated images | default: 95

    -d, --debug
                       Debug mode for which program will print further information during running | default: 0
```

Reference of third-party repos:

- <http://github.com/nothings/stb>
- <https://github.com/YanB25/argparser>
