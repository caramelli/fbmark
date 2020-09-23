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
  unsigned char *buffer, c;
  unsigned int iters = 1, i, j, n;
  float a, b, tmp, x, y, seconds;
  struct timeval start, stop;

  fd = getenv("FRAMEBUFFER") ? open(getenv("FRAMEBUFFER"), O_RDWR) : open("/dev/fb0", O_RDWR);
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
  gettimeofday(&start, NULL);

  while (iters <= 48) {
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        x = y = c = 0;

        a = 3. * j / width - 2;
        b = 1 - 2. * i / height;

        do {
          tmp = x;
          x = x * x - y * y + a;
          y = 2 * tmp * y + b;
          if (x * x + y * y > 4)
            break;
        } while (++c < iters);

        for (n = 0; n < 3; n++) {
          buffer[(posy + i) * info.xres * info.bits_per_pixel / 8 + (posx + j) * info.bits_per_pixel / 8 + n] = c * 255 / iters;
        }
      }
    }

    iters++;
  }

  gettimeofday(&stop, NULL);
  seconds = (stop.tv_sec - start.tv_sec) + (stop.tv_usec - start.tv_usec) / 1000000.;

  printf("Mandelbrot frame buffer test bench\n");
  printf("Benchmarking 48 iterations: %.2f seconds\n", seconds);

  munmap(buffer, len);
  close(fd);

  return 0;
}
