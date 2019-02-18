/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Simple2.cxx,v 1.3 2004/06/21 20:54:53 xpxqx Exp $
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
// VTK Common
#include "vtkCommand.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleSwitch.h"
// FLTK
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


// ----------------------------------------------------------------------------
class vtkRendererObserver : public vtkCommand
{
public:
  static vtkRendererObserver* New (void) { return new vtkRendererObserver; }

  void SetOutputWidget (Fl_Output* a) { this->OutputWidget = a; }

  void Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      vtkRenderer* renderer = static_cast<vtkRenderer*>(aCaller);
      int    n = renderer->GetNumberOfPropsRendered();
      double t = renderer->GetLastRenderTimeInSeconds() * 1000.0;
      (void) sprintf(this->Buffer, "%d props in %g ms.", n, t);
      this->OutputWidget->value(this->Buffer);

      if (!this->OutputWidget->window()->shown())
        {
        this->OutputWidget->window()->show();
        }
    }

protected:
  vtkRendererObserver (void) : OutputWidget(0) { this->Buffer[0] = '\0'; }

  Fl_Output*    OutputWidget;
  char          Buffer[256];

private:
  vtkRendererObserver (const vtkRendererObserver&);
  void operator= (const vtkRendererObserver&);
};

//VTK_FLTK_IMPLEMENT(void)
void
ShowCb (Fl_Widget*, void* aPtr)
{
  vtkRenderWindow* renderWindow = reinterpret_cast<vtkRenderWindow*>(aPtr);

  if (!renderWindow->GetMapped()) renderWindow->Render();
}

//VTK_FLTK_IMPLEMENT(void)
void
QuitCb (Fl_Widget*, void*) { exit(0); }


int
main (int argc, char* argv[])
{
  // Set up the VTK rendering pipeline creating an actor and giving it
  // some cone geometry. The references to the VTK pipeline can be
  // deleted along the way as the objects themselves will stick around
  // until the Renderer (created below) is deleted.
  vtkActor*          coneActor  = vtkActor::New();
  vtkPolyDataMapper* coneMapper = vtkPolyDataMapper::New();
  vtkConeSource*     coneSource = vtkConeSource::New();

  // Connect the Source to the Mapper and decrement its reference count.
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  coneSource->Delete();
  // Connect the Mapper to the Actor and decrement its reference count.
  coneActor->SetMapper(coneMapper);
  coneMapper->Delete();

  // Create the Renderer responsible for drawing the view.
  vtkRenderer*               renderer     = vtkRenderer::New();

  // Add Actor to the renderer and decrement its reference count.
  renderer->AddActor(coneActor);
  coneActor->Delete();

  // Create the render window which will show up on the screen.
  vtkRenderWindow*           renderWindow = vtkRenderWindow::New();

  renderWindow->SetMultiSamples(0);

  // Add the renderer into the render window and decrement its reference count.
  renderWindow->AddRenderer(renderer);
  // Set the size to be 300 by 300 pixels.
  renderWindow->SetSize(300, 300);
  renderWindow->SetPosition(30, 30);

  // The vtkRenderWindowInteractor class watches for events (e.g., keypress,
  // mouse) in the vtkRenderWindow. These events are translated into
  // event invocations that VTK understands. Then observers of these VTK
  // events can process them as appropriate.
  vtkRenderWindowInteractor* interactor   = vtkRenderWindowInteractor::New();

  vtkInteractorObserver*  pObserver = interactor->GetInteractorStyle();
  vtkInteractorStyleSwitch*  pStyle = vtkInteractorStyleSwitch::SafeDownCast( pObserver );
  if ( pStyle )
      pStyle->SetCurrentStyleToTrackballCamera();

  // Set the render window for the interactor and decrement its reference count.
  interactor->SetRenderWindow(renderWindow);
  renderWindow->SetWindowName(interactor->GetClassName());

  //
  // Add an FLTK window
  //
  int x = renderWindow->GetPosition()[0] + renderWindow->GetSize()[0] + 10;
  int y = renderWindow->GetPosition()[1];

  // Set up the main window.
  Fl_Window* mainWindow = new Fl_Window(x,y,240,95, "Simple2");
  // An instructional message to the user.
  Fl_Box*    titleBox   = new Fl_Box(5,5,230,25, renderWindow->GetClassName());
  // An instructional message to the user.
  Fl_Output* textOutput = new Fl_Output(5,35,230,25);
  // Button to quit the application
  Fl_Button* showButton = new Fl_Button(55,65,60,25, "show");
  // Button to quit the application
  Fl_Button* quitButton = new Fl_Button(125,65,60,25, "quit");
  // Done populating the mainWindow.
  mainWindow->end();
  // Add the callback to the button.
  showButton->callback(ShowCb, (void *) renderWindow);
  quitButton->callback(QuitCb, NULL);
  mainWindow->callback(QuitCb, NULL);

  // Create and add and observer to update the text output.
  vtkRendererObserver* renObserver = vtkRendererObserver::New();
  renObserver->SetOutputWidget(textOutput);
  renderer->AddObserver(vtkCommand::EndEvent, renObserver);
  renObserver->Delete();

  // Start the event loop.
  // The user can now use the mouse and keyboard to perform the operations
  // on the scene according to the current interaction style. When the user
  // presses the "e" key, by default an ExitEvent is invoked by the
  // vtkRenderWindowInteractor which is caught and drops out of the event
  // loop (triggered by the Start() method that follows).
  interactor->Start();

  // After the event loop returns, Delete the interactor.
  // Once this is done, the rest of the VTK pipeline will also be destroyed.
  interactor->Delete();

  delete mainWindow;

  return 0;
}

/* 
 * End of $Id: Simple2.cxx,v 1.3 2004/06/21 20:54:53 xpxqx Exp $.
 * 
 */
