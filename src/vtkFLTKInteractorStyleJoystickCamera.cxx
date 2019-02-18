/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: $
 *
 * Copyright (c) 2019 Paul Douglas Hahn, CompIntense HPC, LLC
 * All rights reserved.
 *
 * See LICENSE file for details.
 *
 *    This software is distributed WITHOUT ANY WARRANTY; without even 
 *    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 *    PURPOSE.  See the above LICENSE notice for more information.
 *
 */
#include "vtkFLTKInteractorStyleJoystickCamera.h"
// FLTK
#include <FL/Fl.H>
// VTK Common
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
// vtkFLTK
#include "vtkFLTKOpenGLRenderWindow.h"
#include "vtkFLTKRenderWindowInteractor.h"
#include "Fl_VTK_Window.H"


vtkStandardNewMacro(vtkFLTKInteractorStyleJoystickCamera);

vtkFLTKInteractorStyleJoystickCamera::vtkFLTKInteractorStyleJoystickCamera()
{
}

vtkFLTKInteractorStyleJoystickCamera::~vtkFLTKInteractorStyleJoystickCamera()
{
}

void vtkFLTKInteractorStyleJoystickCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
 
//----------------------------------------------------------------------------
// The only thing this does different than the superclass
// is use the old CreateTimer instead of the CreateRepeatingTimer.
// If we make CreateRepeatingTimer virtual, then we can get rid of this
// method.
void vtkFLTKInteractorStyleJoystickCamera::StartState(int newstate)
{
  //    cout << "HERE in vtkFLTKInteractorStyleJoystickCamera::StartState()!\n" << endl;
#if 1
  this->State = newstate;
  if (this->AnimState == VTKIS_ANIM_OFF)
  {
    vtkFLTKRenderWindowInteractor *rwi = (vtkFLTKRenderWindowInteractor*)this->Interactor;
    rwi->GetRenderWindow()->SetDesiredUpdateRate(rwi->GetDesiredUpdateRate());
    this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
    if ( this->UseTimers &&
         !(this->TimerId=rwi->CreateRepeatingTimer(this->TimerDuration)) )
    {
      // vtkTestingInteractor cannot create timers
      if (std::string(rwi->GetClassName()) != "vtkTestingInteractor")
      {
        vtkErrorMacro(<< "Timer start failed");
      }
      this->State = VTKIS_NONE;
    }
  }
#else
  this->State = newstate;
  if (this->AnimState == VTKIS_ANIM_OFF)
  {
    vtkRenderWindowInteractor *rwi = this->Interactor;
    rwi->GetRenderWindow()->SetDesiredUpdateRate(rwi->GetDesiredUpdateRate());
    this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
    rwi->SetTimerEventDuration(this->TimerDuration);
    if ( this->UseTimers && !(this->TimerId=rwi->CreateTimer(VTKI_TIMER_FIRST)) )
    {
      vtkErrorMacro(<< "Timer start failed");
      this->State = VTKIS_NONE;
    }
  }
#endif
}

void vtkFLTKInteractorStyleJoystickCamera::StopState()
{
  //    cout << "HERE in vtkFLTKInteractorStyleJoystickCamera::StopState()!\n" << endl;
  this->State = VTKIS_NONE;
  if (this->AnimState == VTKIS_ANIM_OFF)
  {
    vtkFLTKRenderWindowInteractor *rwi = (vtkFLTKRenderWindowInteractor*)this->Interactor;
    vtkRenderWindow *renwin = rwi->GetRenderWindow();
    renwin->SetDesiredUpdateRate(rwi->GetStillUpdateRate());
    if (this->UseTimers &&
        // vtkTestingInteractor cannot create timers
        std::string(rwi->GetClassName()) != "vtkTestingInteractor" &&
        !rwi->DestroyTimer(this->TimerId))
    {
      vtkErrorMacro(<< "Timer stop failed");
    }
    this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
    rwi->Render();
  }
}

