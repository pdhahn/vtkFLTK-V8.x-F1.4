/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MCases.cxx,v 1.5 2004/05/13 18:37:49 xpxqx Exp $
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
#include "MCasesUI.h"
// FLTK
#include <FL/Fl.H>
// VTK Rendering
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
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
CaseInputCb (Fl_Value_Input* aInput, void* aPtr)
{
  MCasesUI* gui;

  if (aInput == NULL)
    return;
  if ((gui = reinterpret_cast<MCasesUI*>(aPtr)) == NULL)
    return;

  gui->mCaseRoller->value(aInput->value());

  Fl::check();

  gui->SetCase(1, 0, (unsigned char)(aInput->value()));
  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
CaseRollerCb (Fl_Roller* aInput, void* aPtr)
{
  MCasesUI* gui;

  if (aInput == NULL)
    return;
  if ((gui = reinterpret_cast<MCasesUI*>(aPtr)) == NULL)
    return;

  gui->mCaseInput->value(aInput->value());

  Fl::check();

  gui->SetCase(1, 0, (unsigned char)(aInput->value()));
  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
AntiAliasToggleCb (Fl_Light_Button* aButton, void* aPtr)
{
  MCasesUI* gui;

  if (aButton == NULL)
    return;
  if ((gui = reinterpret_cast<MCasesUI*>(aPtr)) == NULL)
    return;

  if (aButton->value())
    gui->GetView()->GetRenderWindow()->SetAAFrames(7);
  else
    gui->GetView()->GetRenderWindow()->SetAAFrames(0);

  gui->UpdateView();
}

// ----------------------------------------------------------------------------
int
main (int argc, char* argv[])
{
  MCasesUI* gui = MCasesUI::New();

  gui->Show(argc, argv);

  vtkRenderer*  renderer        = gui->GetView()->GetDefaultRenderer();
  vtkCamera*    camera          = renderer->GetActiveCamera();

  renderer->SetBackground(0.2, 0.4, 0.6);

  /* Position the camera. */
  renderer->ResetCamera();
  camera->Dolly(1.2);
  camera->Azimuth(30);
  camera->Elevation(20);
  renderer->ResetCameraClippingRange();

  gui->UpdateView();

  // this is the standard way of "starting" a fltk application
  int fl_ret = Fl::run();

  gui->Delete();

  return fl_ret;
}

/* 
 * End of: $Id: MCases.cxx,v 1.5 2004/05/13 18:37:49 xpxqx Exp $.
 *  
 */
