/*
Copyright (c) 2014 Joe Davisson.

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

#ifndef GUI_H
#define GUI_H

class Widget;
class Button;
class ToggleButton;
class Field;
class View;

#include "rendera.h"

namespace Gui
{
  void init();
  void setMenuItem(const char *);
  void clearMenuItem(const char *);

  void updateColor(int);
  void checkPalette(Widget *, void *);
  void drawPalette();
  void checkZoomIn(Button *, void *);
  void checkZoomOut(Button *, void *);
  void checkZoomFit(ToggleButton *, void *);
  void checkZoomOne(Button *, void *);
  void checkZoom();
  void checkGrid(ToggleButton *, void *);
  void checkGridX(Field *, void *);
  void checkGridY(Field *, void *);
  void checkPaintSize(Widget *, void *);
  void checkPaintShape(Widget *, void *);
  void checkPaintStroke(Widget *, void *);
  void checkPaintEdge(Widget *, void *);
  void checkPaintSmooth(Widget *, void *);
  void checkTool(Widget *, void *);
  void checkColor(Widget *, void *);
  void checkHue(Widget *, void *);
  void checkSatVal(Widget *, void *);
  void checkTrans(Widget *, void *);
  void checkBlend(Widget *, void *);
  void checkWrap(Widget *, void *);
  void checkClone(Widget *, void *);
  void checkMirror(Widget *, void *);
  void checkOrigin(Widget *, void *);
  void checkConstrain(Widget *, void *);
  void checkCropDo();
  void checkCropValues();

  View *getView();
  int getTool();
}

#endif

