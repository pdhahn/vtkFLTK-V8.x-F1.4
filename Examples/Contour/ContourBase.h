/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: ContourBase.h,v 1.5 2004/06/16 01:42:50 xpxqx Exp $
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
#ifndef CONTOUR_BASE_H_
#  define CONTOUR_BASE_H_
// vtkFLTK
#  include "vtkFLTKConfigure.h"
// VTK Common
#  include "vtkObject.h"

typedef double vtkFloatingPointType;

class ContourBaseInternal;
class Fl_Window;
class Fl_VTK_Window;
class vtkCommand;

class VTK_EXPORT ContourBase : public vtkObject
{
public:
  vtkTypeMacro (ContourBase, vtkObject);

  /** Get the FLTK window widget. */
  Fl_Window*            GetFltkWindow (void) const;

  /** Get the vtkFLTK window widget. */
  Fl_VTK_Window*        GetVtkWindow (void) const;

  /** Keep the interface updated. */
  virtual void          Check (void) = 0;

  /** Show the FLTK widget. */
  virtual void          Show (int argc, char* argv[]);

  /** Show the FLTK widget. */
  virtual void          Show (void);

  /** Returns whether the FLTK widget window is shown. */
  int                   Shown (void) const;

  /** Hide the FLTK widget. */
  virtual void          Hide (void);

  /** Update the VTK view. */
  void                  UpdateView (void);

  /*@{*/
  /** Set/Get the isocontour value. */
  void                  SetContourValue (vtkFloatingPointType);
  vtkFloatingPointType  GetContourValue (void);
  /*@}*/

  /*@{*/
  /** Toggle the visibility of the rendered contour. */
  void                  SetContourVisibility (int);
  int                   GetContourVisibility (void);
  /*@}*/

  vtkFloatingPointType  GetMin (void) const;
  vtkFloatingPointType  GetMax (void) const;
  vtkFloatingPointType  GetWindow (void) const;

  void                  SetImageReaderProgressMethod (vtkCommand*);
  void                  SetImageSourceProgressMethod (vtkCommand*);
  void                  SetContourFilterProgressMethod (vtkCommand*);
  void                  SetConnectivityFilterProgressMethod (vtkCommand*);

  /** Write the pixels of the RenderWindow to disk. */
  void                  WriteImage (const char* const& fileName);

protected:
  ContourBase (void);
  virtual ~ContourBase();

  ContourBaseInternal*  Internal;
  Fl_Window*            FltkWindow;
  Fl_VTK_Window*        VtkWindow;

  void                  SetFltkWindow (Fl_Window*);
  void                  SetVtkWindow (Fl_VTK_Window*);

  void                  Initialize (void);
  virtual void          InitIsoInput (void) = 0;

private:
  ContourBase (const ContourBase&); // Not implemented.;
  void operator= (const ContourBase&); // Not implemented.;
};
#endif /* CONTOUR_BASE_H_ */
/*
 * End of: $Id: ContourBase.h,v 1.5 2004/06/16 01:42:50 xpxqx Exp $.
 *
 */
