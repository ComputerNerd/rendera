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

#include "rendera.h"

Dialog::Dialog()
{
  about = new Fl_Double_Window(336, 112, "About");
  about_logo = new Widget(about, 8, 8, 320, 64, "Logo", "data/logo_large.png", 0, 0);
  about_ok = new Fl_Button(336 / 2 - 32, 80, 64, 24, "OK");
  about_ok->callback((Fl_Callback *)hide_about);
  about->set_modal();
  about->end(); 

  new_image = new Fl_Double_Window(200, 128, "New Image");
  new_image_width = new Field(new_image, 64, 8, 72, 24, "Width:");
  new_image_height = new Field(new_image, 64, 40, 72, 24, "Height:");
  new_image_width->maximum_size(8);
  new_image_height->maximum_size(8);
  new_image_width->value("640");
  new_image_height->value("480");
  new Separator(new_image, 8, 84, 200 - 16, 2, "");
  new_image_ok = new Fl_Button(200 - 64 - 8, 96, 64, 24, "OK");
  new_image_ok->callback((Fl_Callback *)hide_new_image);
  new_image_cancel = new Fl_Button(200 - 64 - 8 - 64 - 8, 96, 64, 24, "Cancel");
  new_image_cancel->callback((Fl_Callback *)cancel_new_image);
  new_image->set_modal();
  new_image->end(); 
}

Dialog::~Dialog()
{
}
