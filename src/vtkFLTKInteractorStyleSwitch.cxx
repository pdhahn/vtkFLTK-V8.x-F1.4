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
#include "vtkFLTKInteractorStyleSwitch.h"
// FLTK
#include <FL/Fl.H>
// VTK Common
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
// vtkFLTK
#include "vtkFLTKOpenGLRenderWindow.h"
#include "vtkFLTKRenderWindowInteractor.h"
#include "vtkFLTKInteractorStyleJoystickCamera.h"
#include "Fl_VTK_Window.H"


vtkStandardNewMacro(vtkFLTKInteractorStyleSwitch);

vtkFLTKInteractorStyleSwitch::vtkFLTKInteractorStyleSwitch()
{
  if ( this->JoystickCamera  &&  !vtkFLTKInteractorStyleJoystickCamera::SafeDownCast( this->JoystickCamera ) )
    {
    this->JoystickCamera->Delete();

    this->JoystickCamera = vtkFLTKInteractorStyleJoystickCamera::New();
    }
}

vtkFLTKInteractorStyleSwitch::~vtkFLTKInteractorStyleSwitch()
{
}

void vtkFLTKInteractorStyleSwitch::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
