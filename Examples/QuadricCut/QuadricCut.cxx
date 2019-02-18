/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: QuadricCut.cxx,v 1.4 2004/05/13 18:37:50 xpxqx Exp $
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
#include "QuadricCutUI.h"
// FLTK
#include <FL/Fl.H>
// VTK Rendering
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleSwitch.h"
// vtkFLTK
#include "Fl_VTK_Window.H"

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


// ----------------------------------------------------------------------------
VTK_FLTK_IMPLEMENT(void)
InInCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetInIn(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OutInCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOutIn(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
InOutCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetInOut(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OutOutCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOutOut(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OnOnCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOnOn(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OnInCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOnIn(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OnOutCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOnOut(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
InOnCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetInOn(aChoice->value());
}

VTK_FLTK_IMPLEMENT(void)
OutOnCb (Fl_Choice* aChoice, void* aPtr)
{
  QuadricCutUI* gui;

  if (aChoice == NULL)
    return;
  if ((gui = reinterpret_cast<QuadricCutUI*>(aPtr)) == NULL)
    return;

  gui->SetOutOn(aChoice->value());
}

// ----------------------------------------------------------------------------
int
main (int argc, char* argv[])
{
  QuadricCutUI* gui = QuadricCutUI::New();

  gui->Show(argc, argv);

  vtkRenderer*  renderer        = gui->GetView()->GetDefaultRenderer();
  vtkCamera*    camera          = renderer->GetActiveCamera();

  renderer->SetBackground(0.2, 0.4, 0.6);

  /* Position the camera. */
  renderer->ResetCamera();
  camera->Dolly(1.2);
  camera->Azimuth(15);
  camera->Elevation(10);
  renderer->ResetCameraClippingRange();

  gui->UpdateView();

  // this is the standard way of "starting" a fltk application
  int fl_ret = Fl::run();

  gui->Delete();

  return fl_ret;
}

/* 
 * End of: $Id: QuadricCut.cxx,v 1.4 2004/05/13 18:37:50 xpxqx Exp $.
 *  
 */
