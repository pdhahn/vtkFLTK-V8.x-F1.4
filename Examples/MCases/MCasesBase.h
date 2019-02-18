/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MCasesBase.h,v 1.4 2004/06/15 05:15:12 xpxqx Exp $
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
#ifndef MCASES_BASE_H_
#  define MCASES_BASE_H_
// vtkFLTK
#  include "vtkFLTKConfigure.h"
// VTK Common
#  include "vtkObject.h"

class MCasesInternal;
class Fl_Window;
class Fl_VTK_Window;

class VTK_EXPORT MCasesBase : public vtkObject
{
public:
  vtkTypeMacro (MCasesBase, vtkObject);

  /** Get the FLTK window widget. */
  Fl_Window*            GetFltkWindow (void) const;

  /** Get the vtkFLTK window widget. */
  Fl_VTK_Window*        GetVtkWindow (void) const;

  /** Set the case (0 - 255). */
  void                  SetCase (float in, float out, unsigned char n);

  /** Show the FLTK widget. */
  virtual void          Show (int argc, char* argv[]);

  /** Show the FLTK widget. */
  virtual void          Show (void);

  /** Returns whether the FLTK widget window is shown. */
  int                   Shown (void);

  /** Hide the FLTK widget. */
  virtual void          Hide (void);

protected:
  MCasesBase (void);
  virtual ~MCasesBase();

  MCasesInternal*       Internal;
  Fl_Window*            FltkWindow;
  Fl_VTK_Window*        VtkWindow;

  void                  SetFltkWindow (Fl_Window* a);
  void                  SetVtkWindow (Fl_VTK_Window* a);

private:
  MCasesBase (const MCasesBase&); // Not implemented.;
  void operator= (const MCasesBase&); // Not implemented.;
};
#endif /* MCASES_BASE_H_ */
/*
 * End of: $Id: MCasesBase.h,v 1.4 2004/06/15 05:15:12 xpxqx Exp $.
 *
 */
