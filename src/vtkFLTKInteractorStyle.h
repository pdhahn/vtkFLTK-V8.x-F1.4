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
#ifndef VTK_FLTK_INTERACTOR_STYLE_H_
#  define VTK_FLTK_INTERACTOR_STYLE_H_
#  include "vtkFLTKConfigure.h"
// VTK generic Interactor Style
#  include "vtkInteractorStyle.h"

class vtkCommand;

//BTX
class Fl_Window;
class Fl_VTK_Window;
//ETX

/** \class   vtkFLTKInteractorStyle
 *  \brief   FLTK itneractor style for a vtkFLTRenderWindowInteractor
 * 
 * vtkFLTKInteractorStyle is a convenience object that overrides
 * the virtual StartState() etc. methods to exclude calls to
 * CreateRepeatingTimer() in the generic version
 * 
 * \author  Paul Douglas Hahn, CompIntense HPC, LLC
 * \version $Revision: 1.0 $
 * \date    $Date: 2019/02/17 $
 * 
 * \sa
 * vtkFLTKInteractorStyle vtkRenderWindowInteractor vtkRenderWindow Fl_VTK_Window Fl_Window
 */

class VTK_FLTK_EXPORT vtkFLTKInteractorStyle
  : public vtkInteractorStyle
{
public:
  static vtkFLTKInteractorStyle* New (void);
  vtkTypeMacro(vtkFLTKInteractorStyle,vtkInteractorStyle);
  void PrintSelf (ostream&, vtkIndent);

public:
  void StartState(int newstate) override;

protected:
  vtkFLTKInteractorStyle();
  ~vtkFLTKInteractorStyle() override;

private:
  vtkFLTKInteractorStyle (const vtkFLTKInteractorStyle&);  // Not implemented.
  void operator= (const vtkFLTKInteractorStyle&);  // Not implemented.
};

#endif /* VTK_FLTK_INTERACTOR_STYLE_H_ */
/* 
 * End of: $Id: $.
 * 
 */
