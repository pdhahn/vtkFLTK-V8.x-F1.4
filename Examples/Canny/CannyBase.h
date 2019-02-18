/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: CannyBase.h,v 1.4 2004/06/15 05:15:11 xpxqx Exp $
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
#ifndef CANNY_BASE_H_
#  define CANNY_BASE_H_
// vtkFLTK
#  include "vtkFLTKConfigure.h"
// VTK Common
#  include "vtkObject.h"

typedef double vtkFloatingPointType;

class CannyBaseInternal;
class Fl_Window;
class Fl_VTK_Window;
class vtkAbstractProgressCommand;

class VTK_EXPORT CannyBase : public vtkObject
{
public:
  vtkTypeMacro (CannyBase, vtkObject);

  /** Get the FLTK window widget. */
  Fl_Window*            GetFltkWindow (void) const;

  /** Get the vtkFLTK window widget. */
  Fl_VTK_Window*        GetVtkWindow (void) const;

  /*@{*/
  /** Keep the interface updated. */
  virtual void          Check (void) = 0;
  virtual void          Flush (void) = 0;
  /*@}*/

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
  /** Get/Set the line width. */
  double                GetLineWidth (void);
  void                  SetLineWidth (double);
  /*@}*/

  /*@{*/
  /** Get/Set the gaussian smoothing radius factor. */
  vtkFloatingPointType  GetSmoothingRadiusFactor (void);
  void                  SetSmoothingRadiusFactor (vtkFloatingPointType);
  /*@}*/

  /*@{*/
  /** Get/Set the background color. */
  void                  GetBackgroundColor (double [3]);
  void                  SetBackgroundColor (double [3]);
  /*@}*/

  /*@{*/
  /** Get/Set the foreground color. */
  void                  GetForegroundColor (double [3]);
  void                  SetForegroundColor (double [3]);
  /*@}*/

  /** Toggle edgel actor visibility. */
  void                  SetEdgelVisibility (int);

  /** Toggle image actor visibility. */
  void                  SetImageVisibility (int);

  /** Read an image file. */
  int                   ReadImage (const char* const& fileName);

  /** Write the pixels of the RenderWindow to disk. */
  int                   WriteImage (const char* const& fileName);

  /*@{*/
  /** Set the progress command. */
  vtkGetObjectMacro(ProgressCommand, vtkAbstractProgressCommand);
  void                  SetProgressCommand (vtkAbstractProgressCommand*);
  /*@}*/

  /** Interrupt a rendering that is in progress. */
  void                  AbortRendering (void);

  /** Are we in the middle of a render? */
  int                   IsRendering (void);

protected:
  CannyBase (void);
  virtual ~CannyBase();

  CannyBaseInternal*    Internal;
  Fl_Window*            FltkWindow;
  Fl_VTK_Window*        VtkWindow;

  vtkAbstractProgressCommand*   ProgressCommand;

  double                LineWidth;
  double                SmoothingRadiusFactor;

  double                BackgroundColor[3];
  double                ForegroundColor[3];

  int                   EdgelVisibility;
  int                   ImageVisibility;

  void                  SetFltkWindow (Fl_Window*);
  void                  SetVtkWindow (Fl_VTK_Window*);

private:
  CannyBase (const CannyBase&); // Not implemented.;
  void operator= (const CannyBase&); // Not implemented.;
};
#endif /* CANNY_BASE_H_ */
/*
 * End of: $Id: CannyBase.h,v 1.4 2004/06/15 05:15:11 xpxqx Exp $.
 *
 */
