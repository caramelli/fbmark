/*
  fbmark                   Linux Framebuffer Benchmark
  Copyright (C) 2014-2017  Nicolas Caramelli

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
  int fd, width, height, posx, posy;
  struct fb_var_screeninfo info;
  size_t len;
  unsigned char *buffer, *data, r, g ,b;
  unsigned int w, h, i, j, x, y, start, end, count;
  struct timeval tv;

  fd = getenv("FRAMEBUFFER") ? open(getenv("FRAMEBUFFER"), O_RDWR) : open("/dev/fb0", O_RDWR);
  if (fd == -1) {
    printf("Failed to open Framebuffer device: %m\n");
    return 1;
  }

  ioctl(fd, FBIOGET_VSCREENINFO, &info);

  if (getenv("WIDTH")) width = atoi(getenv("WIDTH"));
  else width = info.xres;
  if (getenv("HEIGHT")) height = atoi(getenv("HEIGHT"));
  else height = info.yres;
  if (getenv("POSX")) posx = atoi(getenv("POSX"));
  else posx = 0;
  if (getenv("POSY")) posy = atoi(getenv("POSY"));
  else posy = 0;

  len = info.xres * info.yres * info.bits_per_pixel / 8;
  buffer = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
  w = width >> 2;
  h = height >> 2;
  count = 0;
  gettimeofday(&tv, NULL);
  start = tv.tv_sec * 1000 + tv.tv_usec / 1000;

  do {
    r = rand() % 256;
    g = rand() % 256;
    b = rand() % 256;
    x = rand() % (width - w);
    y = rand() % (height - h);
    data = buffer + (posy + y) * info.xres * info.bits_per_pixel / 8 + (posx + x) * info.bits_per_pixel / 8;

    for (i = 0; i < h; i++) {
      for (j = 0; j < w; j++) {
        data[info.bits_per_pixel / 8 * j + 2] = r;
        data[info.bits_per_pixel / 8 * j + 1] = g;
        data[info.bits_per_pixel / 8 * j    ] = b;
      }
      data += info.xres * info.bits_per_pixel / 8;
    }

    count++;
    gettimeofday(&tv, NULL);
    end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
  } while (end < (start + 5000));

  printf("Rectangle frame buffer test bench\n");
  printf("Benchmarking %dx%d size: %.2f MPixels/second\n", w, h, count * w * h / ((end - start) * 1000.));

  munmap(buffer, len);
  close(fd);

  return 0;
}
