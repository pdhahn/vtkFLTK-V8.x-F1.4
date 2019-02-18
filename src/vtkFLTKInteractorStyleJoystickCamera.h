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
#ifndef VTK_FLTK_INTERACTOR_STYLE_JOYSTICK_CAMERA_H_
#  define VTK_FLTK_INTERACTOR_STYLE_JOYSTICK_CAMERA_H_
#  include "vtkFLTKConfigure.h"
// VTK generic Interactor Style
#  include "vtkInteractorStyleJoystickCamera.h"

class vtkCommand;

//BTX
class Fl_Window;
class Fl_VTK_Window;
//ETX

/** \class   vtkFLTKInteractorStyleJoystickCamera
 *  \brief   FLTK itneractor style for a vtkFLTRenderWindowInteractor
 * 
 * vtkFLTKInteractorStyleJoystickCamera is a convenience object that
 * overrides the virtual StartState() etc. methods to exclude calls to
 * CreateRepeatingTimer() made in the generic version
 * 
 * \author  Paul Douglas Hahn, CompIntense HPC, LLC
 * \version $Revision: 1.0 $
 * \date    $Date: 2019/02/17 $
 * 
 * \sa
 * vtkFLTKInteractorStyleJoystickCamera vtkRenderWindowInteractor vtkRenderWindow Fl_VTK_Window Fl_Window
 */

class VTK_FLTK_EXPORT vtkFLTKInteractorStyleJoystickCamera
  : public vtkInteractorStyleJoystickCamera
{
public:
  static vtkFLTKInteractorStyleJoystickCamera* New (void);
  vtkTypeMacro(vtkFLTKInteractorStyleJoystickCamera,vtkInteractorStyleJoystickCamera);
  void PrintSelf (ostream&, vtkIndent);

public:
  void StartState(int newstate) override;

  void StopState() override;

protected:
  vtkFLTKInteractorStyleJoystickCamera();
  ~vtkFLTKInteractorStyleJoystickCamera() override;

private:
  vtkFLTKInteractorStyleJoystickCamera (const vtkFLTKInteractorStyleJoystickCamera&);  // Not implemented.
  void operator= (const vtkFLTKInteractorStyleJoystickCamera&);  // Not implemented.
};

#endif /* VTK_FLTK_INTERACTOR_STYLE_JOYSTICK_CAMERA_H_ */
/* 
 * End of: $Id: $.
 * 
 */
