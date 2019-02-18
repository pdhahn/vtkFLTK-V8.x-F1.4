/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Cone4.cxx,v 1.19 2004/06/18 22:32:09 xpxqx Exp $
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
// VTK Graphics
#include "vtkConeSource.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
// FLTK
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
// vtkFLTK
#include "Fl_VTK_Window.H"

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


// callbacks for the animation
VTK_FLTK_IMPLEMENT(void)
TimerCb (void* a)
{
  Fl_VTK_Window* window = reinterpret_cast<Fl_VTK_Window*>(a);

  vtkCamera* camera = window->GetDefaultCamera();

  camera->Roll(0.2);
  window->Update();

  // Repeat timeout at 1/30th of a second from when the system call
  // elapsed that caused this timeout.
  Fl::repeat_timeout(1.0 / 30.0, TimerCb, a);
}

// callbacks for the buttons
VTK_FLTK_IMPLEMENT(void)
ShowCb (Fl_Widget* aButton, void* aPtr)
{
  if ((static_cast<Fl_Button*>(aButton))->value())
    {
    aButton->label("hide");
    reinterpret_cast<Fl_Window*>(aPtr)->show();
    }
  else
    {
    aButton->label("show");
    reinterpret_cast<Fl_Window*>(aPtr)->hide();
    }
}

VTK_FLTK_IMPLEMENT(void)
AnimateCb (Fl_Widget* aButton, void* aPtr)
{
  if ((static_cast<Fl_Button*>(aButton))->value())
    {
    aButton->label("stop");
    Fl::add_timeout(0, TimerCb, aPtr);
    }
  else
    {
    aButton->label("animate");
    Fl::remove_timeout(TimerCb, aPtr);
    }
}

VTK_FLTK_IMPLEMENT(void)
PrintCb (Fl_Widget*, void* aPtr)
{
  Fl_VTK_Window* w;

  if ((w = reinterpret_cast<Fl_VTK_Window*>(aPtr)) != 0)
    {
    w->GetInteractor()->Print(cerr);
    w->GetRenderWindow()->Print(cerr);
    }
}

VTK_FLTK_IMPLEMENT(void)
ViewCb (Fl_Widget* aWindow, void* aPtr)
{
  reinterpret_cast<Fl_Button*>(aPtr)->label("show");
  reinterpret_cast<Fl_Button*>(aPtr)->value(0);
  static_cast<Fl_Window*>(aWindow)->hide();
}

VTK_FLTK_IMPLEMENT(void)
QuitCb (Fl_Widget*, void*) { exit(0); }


int
main (int argc, char* argv[])
{
  const char* instructions =
    "<key>: <action>\n"
    "    3: toggle stereo mode\n"
    "  j|t: toggle  joystick | trackball modes\n"
    "  w|s: toggle wireframe | surface modes\n"
    "  c|a: toggle    camera | actor modes\n"
    "    p: pick\n"
    "    r: reset";

  // Set up the main window.
  Fl_Window*    mainWindow      = new Fl_Window(300, 140, "Cone4: main");
  // An instructional message to the user.
  Fl_Box*       box             = new Fl_Box(5, 5, 290, 100, instructions);
    {
    box->labelsize(12);
    box->labelfont(FL_COURIER);
    box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
  // Buttons to show/hide the VTK window as well as quit the application
  Fl_Button*    showButton      = new Fl_Button(  5, 110, 70, 25, "show");
  Fl_Button*    animateButton   = new Fl_Button( 80, 110, 70, 25, "animate");
  Fl_Button*    printButton     = new Fl_Button(155, 110, 70, 25, "print");
  Fl_Button*    quitButton      = new Fl_Button(235, 110, 60, 25, "quit");
  // Done populating the mainWindow.
  mainWindow->end();

  showButton->type(FL_TOGGLE_BUTTON);
  animateButton->type(FL_TOGGLE_BUTTON);

  // Set up a separate top level window and its Fl_VTK_Window child.
  Fl_Window* coneWindow = new Fl_Window(400,100, 216,216, "Cone4: cone");
    {
    Fl_VTK_Window* coneView = new Fl_VTK_Window(12,12, 192,192);
      {
      // If 'coneWindow' gets resized, 'coneView' should resize with it.
      coneWindow->resizable(coneView);
      // Done populating the mainWindow.
      coneWindow->end();
      // Setup the VTK rendering pipeline creating an actor and giving it some 
      // cone geometry. The references to the VTK pipeline can be deleted along 
      // the way as the objects themselves will stick around until the
      // Interactor associated with the Fl_VTK_Window is dereferenced. (The
      // Interactor holds a reference to the required RenderWindow which
      // references the Renderer which ultimately references the Actor.)
      vtkActor* coneActor = vtkActor::New();
        {
        vtkPolyDataMapper* coneMapper = vtkPolyDataMapper::New();
          {
          vtkConeSource* coneSource = vtkConeSource::New();
            {
            coneSource->SetResolution(12);
            }
          // Connect the Source to the Mapper
          coneMapper->SetInputConnection(coneSource->GetOutputPort());
          coneSource->Delete();
          }
        // Connect the Mapper to the Actor
        coneActor->SetMapper(coneMapper);
        coneMapper->Delete();
        }
      // Add the Actor to the window's renderer.
      coneView->AddProp(coneActor);
      // Add callbacks to the buttons.
      animateButton->callback(AnimateCb, (void *) coneView);
      printButton->callback(PrintCb, (void *) coneView);
      coneWindow->callback(ViewCb, (void *) showButton);
      coneActor->Delete();
      }
    // Add callbacks to the buttons.
    showButton->callback(ShowCb, (void *) coneWindow);
    quitButton->callback(QuitCb, NULL);
    mainWindow->callback(QuitCb, NULL);
    coneWindow->hide();
    }

  // show() the main Fl_Window
  mainWindow->show(argc, argv);

  // Start the FLTK event loop.
  int fl_ret = Fl::run();
   
  // After the FLTK event loop returns, delete the interface windows.
  delete mainWindow;
  // N.B.: When we delete 'coneWindow', its Fl_VTK_Window child is also 
  //       destroyed. When the Fl_VTK_Window is destroyed, it invokes
  //       Delete() on its associated vtkFlRenderWindowInteractor. Once this 
  //       is done, the rest of the VTK pipeline will also be destroyed.
  delete coneWindow;
   
  return fl_ret;
}

/* 
 * End of $Id: Cone4.cxx,v 1.19 2004/06/18 22:32:09 xpxqx Exp $.
 * 
 */
