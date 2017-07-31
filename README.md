# OrangePi_FBCP

Inspired from KameraSui FBCP and from rpi-fbcp to copy a framebuffer to another. Doesn't use hw accell and doesn't exploid fd mapping.
Read entire source fd, work in memory exploiting caching and locality and writing entire frame to destination

Develop By Roberto Tacconelli 2017

Feel Free To Use & Distribution

### Usage

PiFBCP   fb_from   fb_to

Example
./PiFBCP 0 8 (copy from fd0 to fd8)

### Limits

1. source and destination must have same resolution. I advise to modify script.bin and obtain same resolution
  Example:
  disp_mode = 0
  fb0_format = 5
  fb0_width = 480
  fb0_height = 320
I've developed a version to copy fb to a different resolution but It's not efficient due to scaling and interpolation
2. Convert from 32bpp/24bpp to 16bpp using an internal inline function

### Compile

gcc -o  PiFBCP PiFBCP.c

### Tested

Tested on OrangePi Zero with Waveshare 3.5 (A)


------

Inspired By

KameraSui/OrangePi_FBCP

tasanakorn/rpi-fbcp

https://github.com/tasanakorn/rpi-fbcp

linux__frameBuffer__操作2--写入和截屏

http://blog.csdn.net/sno_guo/article/details/8439000
