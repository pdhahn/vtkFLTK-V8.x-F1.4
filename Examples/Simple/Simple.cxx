/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Simple.cxx,v 1.4 2004/06/18 22:30:21 xpxqx Exp $
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
#include "vtkProperty.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkFLTKOpenGLRenderWindow.h"
#include "vtkFLTKRenderWindowInteractor.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyleSwitch.h"

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


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
  coneActor->SetPosition(0.0,0.0,0.0);
  //  coneActor->GetProperty()->SetLighting(true);
  coneMapper->Delete();

  // Create the Renderer responsible for drawing the view.
  vtkRenderer*               renderer     = vtkRenderer::New();

  // Add Actor to the renderer and decrement its reference count.
  //  renderer->AddViewProp(coneActor);
  renderer->AddActor(coneActor);
  coneActor->Delete();

  renderer->SetBackground(0.1,0.7,0.0);

  // Create the render window which will show up on the screen.
  vtkRenderWindow*           renderWindow = vtkRenderWindow::New();

  renderWindow->SetMultiSamples(0);

  // Add the renderer into the render window and decrement its reference count.
  renderWindow->AddRenderer(renderer);
  // Set the size to be 300 by 300 pixels.
  renderWindow->SetSize(300, 300);

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

  cerr << "RenderWindow is a: " << renderWindow->GetClassName() << endl;
  cerr << "  Interactor is a: " << interactor->GetClassName() << endl;
  cerr << "       Style is a: " << pStyle->GetCurrentStyle()->GetClassName() << endl;
  cerr << "       Use Timers: " << pStyle->GetUseTimers() << endl;

  //  renderWindow->Render();  // not necessary

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
   
  return 0;
}

/* 
 * End of $Id: Simple.cxx,v 1.4 2004/06/18 22:30:21 xpxqx Exp $.
 * 
 */
