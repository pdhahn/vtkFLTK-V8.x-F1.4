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
#include "vtkFLTKInteractorStyle.h"
// FLTK
#include <FL/Fl.H>
// VTK Common
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
// vtkFLTK
#include "vtkFLTKOpenGLRenderWindow.h"
#include "vtkFLTKRenderWindowInteractor.h"
#include "Fl_VTK_Window.H"


vtkStandardNewMacro(vtkFLTKInteractorStyle);

vtkFLTKInteractorStyle::vtkFLTKInteractorStyle()
{
}

vtkFLTKInteractorStyle::~vtkFLTKInteractorStyle()
{
}

void vtkFLTKInteractorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
 
//----------------------------------------------------------------------------
// The only thing this does different than the superclass
// is use the old CreateTimer instead of the CreateRepeatingTimer.
// If we make CreateRepeatingTimer virtual, then we can get rid of this
// method.
void vtkFLTKInteractorStyle::StartState(int newstate)
{
  this->State = newstate;
  if (this->AnimState == VTKIS_ANIM_OFF)
  {
    cerr << "HERE in vtkFLTKInteractorStyle!\n" << endl;
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
}
