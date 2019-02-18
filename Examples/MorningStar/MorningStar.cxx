/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MorningStar.cxx,v 1.7 2004/05/13 18:37:49 xpxqx Exp $
 *
 * Copyright (c) 2002 - 2004 Sean McInerney
 * All rights reserved.
 *
 * See Copyright.txt or http://vtkfltk.sourceforge.net/Copyright.html
 * for details.
 *
 *    This software is distributed WITHOUT ANY WARRANTY; without even 
 *    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 *    PURPOSE.  See the above copyright notice for more information.
 *
 */
#include "MorningStarUI.h"
// FLTK
#include <FL/Fl.H>

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


int
main (int argc, char* argv[])
{
  MorningStarUI* gui = MorningStarUI::New();

  // currently Fl_VTK_Window calls show() on itself AND its parent window when 
  // its associated RenderWindow is created and assigned to the its Interactor
  // (this happens when the above AddActor call is made since the RenderWindow
  //  had not been eith explicitly set or created beforehand). If we do not
  // want to see the Fl_VTK_Window initially, we have to hide it here.
  gui->HideMace();
   
  // show() the main Fl_Window
  gui->Show();

  // this is the standard way of "starting" an FLTK application
  int fl_ret = Fl::run();
   
  gui->Delete();
   
  return fl_ret;
}

/*
 * End of: $Id: MorningStar.cxx,v 1.7 2004/05/13 18:37:49 xpxqx Exp $.
 *
 */
