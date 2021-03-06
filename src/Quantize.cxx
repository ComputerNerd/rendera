/*
Copyright (c) 2015 Joe Davisson.

This file is part of Rendera.

Rendera is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Rendera is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rendera; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <algorithm>
#include <vector>

#include "Bitmap.H"
#include "Dialog.H"
#include "Gui.H"
#include "Inline.H"
#include "Octree.H"
#include "Palette.H"
#include "Project.H"
#include "Quantize.H"
#include "Widget.H"

namespace
{
  // color struct, stores RGB values as floats for increased
  // accuracy, also stores the frequency of the color in the image
  struct color_type
  {
    float r, g, b;
    float freq;
    bool active;
  };

  // create a color_type structure
  inline void makeColor(color_type *c,
                        const float &r, const float &g, const float &b,
                        const float &freq)
  {
    c->r = r;
    c->g = g;
    c->b = b;
    c->freq = freq;
    c->active = true;
  }

  // compute quantization error
  inline float error(color_type *c1, color_type *c2)
  {
    const float r = c1->r - c2->r;
    const float g = c1->g - c2->g;
    const float b = c1->b - c2->b;

    return ((c1->freq * c2->freq) / (c1->freq + c2->freq)) *
            (r * r + g * g + b * b);
  }

  // merge two colors
  inline void merge(color_type *c1, color_type *c2)
  {
    const float mul = 1.0f / (c1->freq + c2->freq);
    c1->r = (c1->freq * c1->r + c2->freq * c2->r) * mul;
    c1->g = (c1->freq * c1->g + c2->freq * c2->g) * mul;
    c1->b = (c1->freq * c1->b + c2->freq * c2->b) * mul;

    c1->freq += c2->freq;  
  }

  // reduces color count by averaging sections of the color cube
  int limitColors(Octree *histogram, color_type *colors, int max_colors)
  {
    int count = 0;
    int step = 16;

    if(max_colors == 512)
      step = 32;

    for(int b = 0; b <= 256 - step; b += step)
    {
      for(int g = 0; g <= 256 - step; g += step)
      {
        for(int r = 0; r <= 256 - step; r += step)
        {
          float rr = 0;
          float gg = 0;
          float bb = 0;
          float div = 0;

          for(int k = 0; k < step; k++)
          {
            for(int j = 0; j < step; j++)
            {
              for(int i = 0; i < step; i++)
              {
                const float d = histogram->read(r + i, g + j, b + k);

                if(d > 0)
                  histogram->write(r + i, g + j, b + k, 0);

                rr += d * (r + i);
                gg += d * (g + j);
                bb += d * (b + k);
                div += d;
              }
            }
          }

          if(div > 0)
          {
            rr /= div;
            gg /= div;
            bb /= div;
            makeColor(&colors[count], rr, gg, bb, div);
            count++;
          }
        }
      }
    }

    return count;
  }

  // stretch a palette to obtain the exact number of colors desired
  void stretchPalette(int *data, int current, int target)
  {
    std::vector<int> temp(target);

    const float ax = (float)(current - 1) / (float)(target - 1);
    int *c[2];

    c[0] = c[1] = &data[0];

    for(int x = 0; x < target; x++)
    {
      float uu = (x * ax);
      int u1 = uu;

      if(u1 > current - 1)
        u1 = current - 1;

      int u2 = (u1 < (current - 1) ? u1 + 1 : u1);
      float u = uu - u1;

      c[0] += u1;
      c[1] += u2;

      float f[2];

      f[0] = (1.0 - u);
      f[1] = u;

      float r = 0, g = 0, b = 0;

      for(int i = 0; i < 2; i++)
      {
        r += (float)getr(*c[i]) * f[i];
        g += (float)getg(*c[i]) * f[i];
        b += (float)getb(*c[i]) * f[i];
      }

      temp[x] = makeRgb((int)r, (int)g, (int)b);

      c[0] -= u1;
      c[1] -= u2;
    }

    for(int x = 0; x < target; x++)
      data[x] = temp[x];
  }
}

// Pairwise clustering quantization, adapted from the algorithm described here:
//
// http://www.visgraf.impa.br/Projects/quantization/quant.html
// http://www.visgraf.impa.br/sibgrapi97/anais/pdf/art61.pdf
//
// This version saves computation time by reducing the color table size
// (and reducing it further if an image is very colorful, in which case color
// accuracy is not as important).
void Quantize::pca(Bitmap *src, Palette *pal, int size)
{
  // popularity histogram
  Octree histogram;

  int max;
  int rep = size;

  // build histogram, inc is the weight of 1 pixel in the image
  float inc = 1.0 / (src->cw * src->ch);
  int count = 0;

  // measure of how colorful an image is
  // more colorful images fill more spaces in the table
  std::vector<int> color_metric(512, 0);

  // preserve lightest/darkest colors
  int lightest = makeRgb(0, 0, 0);
  int darkest = makeRgb(255, 255, 255);

  for(int j = src->ct; j <= src->cb; j++)
  {
    int *p = src->row[j] + src->cl;

    for(int i = src->cl; i <= src->cr; i++)
    {
      if(getl(*p) > getl(lightest))
        lightest = *p;

      if(getl(*p) < getl(darkest))
        darkest = *p;

      rgba_type rgba = getRgba(*p++);
      float freq = histogram.read(rgba.r, rgba.g, rgba.b);

      if(freq < inc)
        count++;

      histogram.write(rgba.r, rgba.g, rgba.b, freq + inc);

      color_metric[((int)rgba.r >> 5) << 0 |
                   ((int)rgba.g >> 5) << 3 |
                   ((int)rgba.b >> 5) << 6] = 1;
    }
  }

  // assign maximum frequency to lightest/darkest colors
  rgba_type rgba;
  rgba = getRgba(lightest);
  histogram.write(rgba.r, rgba.g, rgba.b, 1.0f);
  rgba = getRgba(darkest);
  histogram.write(rgba.r, rgba.g, rgba.b, 1.0f);

  // if image uses more than 1/2 of the color cube then
  // reduce table sizes to save time
  int color_metric_count = 0;

  for(int i = 0; i < 512; i++)
    if(color_metric[i])
      color_metric_count++;

  int max_colors = 4096;

  if(color_metric_count > 256)
    max_colors = 512;

  // color list
  std::vector<color_type> colors(max_colors);

  for(int i = 0; i < max_colors; i++)
    colors[i].active = false;

  // quantization error matrix
  std::vector<float> err_data(((max_colors + 1) * max_colors) / 2);

  // skip if already enough colors
  if(count <= rep)
  {
    count = 0;

    for(int i = 0; i < 16777216; i++)
    {
      rgba_type rgba = getRgba(i);
      const float freq = histogram.read(rgba.r, rgba.g, rgba.b);

      if(freq > 0)
      {
        makeColor(&colors[count], rgba.r, rgba.g, rgba.b, freq);
        count++;
      }
    }
  }
  else
  {
    // limit color count to 4096
    count = limitColors(&histogram, &colors[0], max_colors);
  }

  // set max
  max = count;
  if(max < rep)
    rep = max;

  // init error matrix
  for(int j = 0; j < max; j++)
  {
    for(int i = 0; i < j; i++)
      err_data[i + (j + 1) * j / 2] = error(&colors[i], &colors[j]);
  }

  Gui::showProgress(count - rep);

  // measure offset between array elements
  const int step = &(colors[1].active) - &(colors[0].active);

  while(count > rep)
  {
    int ii = 0, jj = 0;
    float least_err = 999999;
    bool *a = &(colors[0].active);

    // find lowest value in error matrix
    for(int j = 0; j < max; j++, a += step)
    {
      if(*a)
      {
        float *e = &err_data[(j + 1) * j / 2];
        bool *b = &(colors[0].active);

        for(int i = 0; i < j; i++, e++, b += step)
        {
          if(*b && (*e < least_err))
          {
            least_err = *e;
            ii = i;
            jj = j;
          }
        }
      }
    }

    // compute quantization level and place in i, delete j
    merge(&colors[ii], &colors[jj]);
    colors[jj].active = false;
    count--;

    // recompute error matrix for new row
    for(int j = ii; j < max; j++)
    {
      if(colors[j].active)
        err_data[ii + (j + 1) * j / 2] = error(&colors[ii], &colors[j]);
    }

    // user cancelled operation
    if(Fl::get_key(FL_Escape))
      return;

    Gui::updateProgress(count);
  }

  Gui::hideProgress();

  // build palette
  int index = 0;

  for(int i = 0; i < max; i++)
  {
    if(colors[i].active)
    {
      pal->data[index] =
        makeRgb((int)colors[i].r, (int)colors[i].g, (int)colors[i].b);

      index++;
    }
  }

  pal->max = index;

  // stretch palette
  if(pal->max != size)
  {
    stretchPalette(pal->data, pal->max, size);
    pal->max = size;
  }

  // redraw palette widget
  Gui::drawPalette();
  Project::palette->fillTable();
}

// this only makes 256-color palettes
// used by restore filter
void Quantize::fast(Bitmap *src, Palette *pal, int /* */)
{
  Bitmap temp(16, 16);
  src->scale(&temp);
  int index = 0;

  for(int y = 0; y < 16; y++)
    for(int x = 0; x < 16; x++)
      pal->data[index++] = temp.getpixel(x, y);

  pal->max = 256;
  pal->sort();

  for(int i = 0; i < pal->max - 1; i++)
  {
    if(diff24(pal->data[i], pal->data[i + 1]) < 512)
      pal->deleteColor(i);
  }
}

