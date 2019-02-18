/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkOrthogonalImagePlaneSet.h,v 1.2 2004/05/13 18:37:49 xpxqx Exp $
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
#ifndef VTK_ORTHOGONAL_IMAGE_PLANE_SET_H_
#  define VTK_ORTHOGONAL_IMAGE_PLANE_SET_H_
// VTK Common
#  include "vtkObject.h"

class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCellPicker;
class vtkTextProperty;
class vtkImagePlaneWidget;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;

/** \class   vtkOrthogonalImagePlaneSet
 *  \brief   A set of three orthogonal \c vtkImagePlaneWidgets.
 * 
 * \author  Sean McInerney
 * \version $Revision: 1.2 $
 * \date    $Date: 2004/05/13 18:37:49 $
 * 
 * \sa
 * vtkObject vtkImagePlaneWidget vtkRenderWindowInteractor
 */

class VTK_EXPORT vtkOrthogonalImagePlaneSet : public vtkObject
{
public:
  static vtkOrthogonalImagePlaneSet* New (void);
  vtkTypeMacro (vtkOrthogonalImagePlaneSet, vtkObject);
  void PrintSelf (ostream&, vtkIndent);

  void  On (void);
  void  Off (void);

  /** Set the Interactor. */
  void                          SetInteractor (vtkRenderWindowInteractor*);

  /** Set the Input. */
  void                          SetInput (vtkImageData*);

  /** Get the shared internal picker. */
  vtkCellPicker*                GetPicker (void);

  /** Get the shared property for the display text. */
  vtkTextProperty*              GetTextProperty (void);

  /** Get the image plane widget (0=X, 1=Y, and 2=Z). */
  vtkImagePlaneWidget*          GetImagePlaneWidget (int plane);

  /** Get the color transfer function. */
  vtkColorTransferFunction*     GetColorTF (void);

  /** Get the opacity transfer function. */
  vtkPiecewiseFunction*         GetOpacityTF (void);

protected:
  vtkOrthogonalImagePlaneSet (void);
  ~vtkOrthogonalImagePlaneSet();

  vtkCellPicker*                CellPicker;
  vtkTextProperty*              TextProperty;

  vtkImagePlaneWidget*          XPlane;
  vtkImagePlaneWidget*          YPlane;
  vtkImagePlaneWidget*          ZPlane;

  vtkColorTransferFunction*     ColorTF;
  vtkPiecewiseFunction*         OpacityTF;

private:
  vtkOrthogonalImagePlaneSet (const vtkOrthogonalImagePlaneSet&);
  void operator= (const vtkOrthogonalImagePlaneSet&);
};

#endif /* VTK_ORTHOGONAL_IMAGE_PLANE_SET_H_ */
/* 
 * End of: $Id: vtkOrthogonalImagePlaneSet.h,v 1.2 2004/05/13 18:37:49 xpxqx Exp $.
 * 
 */
