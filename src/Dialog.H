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

#ifndef DIALOG_H
#define DIALOG_H

namespace Dialog
{
  void init();
  void about();
  void jpegQuality();
  int jpegQualityValue();
  void javaExport();
  int javaExportOption();
  void pngOptions();
  int pngUsePalette();
  int pngUseAlpha();
  int pngAlphaLevels();
  void newImage();
  void makePalette();
  void editor();
  void message(const char *, const char *);
  bool choice(const char *, const char *);
}

#endif

