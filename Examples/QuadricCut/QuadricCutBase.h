/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: QuadricCutBase.h,v 1.4 2004/06/15 05:15:12 xpxqx Exp $
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
#ifndef QUADRIC_CUT_BASE_H_
#  define QUADRIC_CUT_BASE_H_
// vtkFLTK
#  include "vtkFLTKConfigure.h"
// VTK Common
#  include "vtkObject.h"

class QuadricCutInternal;
class Fl_Window;
class Fl_VTK_Window;

class VTK_EXPORT QuadricCutBase : public vtkObject
{
public:
  vtkTypeMacro (QuadricCutBase, vtkObject);

  /** Get the FLTK window widget. */
  Fl_Window*            GetFltkWindow (void) const;

  /** Get the vtkFLTK window widget. */
  Fl_VTK_Window*        GetVtkWindow (void) const;

  void                  SetInIn (int a);
  void                  SetOutIn (int a);
  void                  SetInOut (int a);
  void                  SetOutOut (int a);
  void                  SetOnOn (int a);
  void                  SetOnIn (int a);
  void                  SetOnOut (int a);
  void                  SetInOn (int a);
  void                  SetOutOn (int a);

  /** Show the FLTK widget. */
  virtual void          Show (int argc, char* argv[]);

  /** Show the FLTK widget. */
  virtual void          Show (void);

  /** Returns whether the FLTK widget window is shown. */
  int                   Shown (void);

  /** Hide the FLTK widget. */
  virtual void          Hide (void);

  virtual void          UpdateView (void);

protected:
  QuadricCutBase (void);
  virtual ~QuadricCutBase();

  QuadricCutInternal*   Internal;
  Fl_Window*            FltkWindow;
  Fl_VTK_Window*        VtkWindow;

  void                  SetFltkWindow (Fl_Window* a);
  void                  SetVtkWindow (Fl_VTK_Window* a);

private:
  QuadricCutBase (const QuadricCutBase&); // Not implemented.;
  void operator= (const QuadricCutBase&); // Not implemented.;
};
#endif /* QUADRIC_CUT_BASE_H_ */
/*
 * End of: $Id: QuadricCutBase.h,v 1.4 2004/06/15 05:15:12 xpxqx Exp $.
 *
 */
