/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MorningStarBase.h,v 1.3 2004/06/15 05:15:12 xpxqx Exp $
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
#ifndef MORNING_STAR_BASE_H_
#  define MORNING_STAR_BASE_H_
// vtkFLTK
#  include "vtkFLTKConfigure.h"
// VTK Common
#  include "vtkObject.h"

class Fl_Window;
class Fl_VTK_Window;
class vtkActor;
class vtkSpikeyMace;

class VTK_EXPORT MorningStarBase : public vtkObject
{
public:
  vtkTypeMacro (MorningStarBase, vtkObject);

  /** Get the Cone view's parent FLTK window. */
  Fl_Window*            GetConeWindow (void) const;

  /** Get the  Cone view's vtkFLTK window. */
  Fl_VTK_Window*        GetConeView (void) const;

  /** Get the Mace view's parent FLTK window. */
  Fl_Window*            GetMaceWindow (void) const;

  /** Get the  Mace view's vtkFLTK window. */
  Fl_VTK_Window*        GetMaceView (void) const;

  /** Keep the interface updated. */
  virtual void          Check (void) = 0;

  /**@{*/
  /** Show/Hide the Cone view's FLTK window. */
  virtual void          Show (int argc, char* argv[]);
  virtual void          Show (void);
  virtual void          Hide (void);
  /**@}*/

  /** Returns whether the Cone view's FLTK window is shown. */
  int                   Shown (void) const;

  /**@{*/
  /** Show/Hide the Mace view's FLTK window. */
  virtual void          ShowMace (int argc, char* argv[]);
  virtual void          ShowMace (void);
  virtual void          HideMace (void);
  /**@}*/

  /** Returns whether the Mace view's FLTK window is shown. */
  int                   ShownMace (void) const;

  /** Update the Cone's VTK view. */
  void                  UpdateConeView (void);

  /** Update the Mace's VTK view. */
  void                  UpdateMaceView (void);

protected:
  MorningStarBase (void);
  virtual ~MorningStarBase();

  Fl_Window*            ConeWindow;
  Fl_VTK_Window*        ConeView;

  Fl_Window*            MaceWindow;
  Fl_VTK_Window*        MaceView;

  vtkActor*             Cone;
  vtkSpikeyMace*        Mace;

  /**@{*/
  /** \warning MUST be called in the constructor of derived class. */
  void                  SetConeWindow (Fl_Window*);
  void                  SetConeView (Fl_VTK_Window*);
  void                  SetMaceWindow (Fl_Window*);
  void                  SetMaceView (Fl_VTK_Window*);
  /**@}*/

  void                  InitializeCone (void);
  void                  InitializeMace (void);

  virtual void          ReceiveMaceParam (void) = 0;

  void                  SetThetaResolution (int);
  void                  SetPhiResolution (int);
  void                  SetSpikeResolution (int);
  void                  SetSpikeRadius (double);
  void                  SetSpikeHeight (double);
  void                  SetScaleFactor (double);
  void                  SetSphereColor (double, double, double);
  void                  SetSphereOpacity (double);
  void                  SetSpikeColor (double, double, double);
  void                  SetSpikeOpacity (double);

  void                  GetThetaResolution (int&);
  void                  GetPhiResolution (int&);
  void                  GetSpikeResolution (int&);
  void                  GetSpikeRadius (double&);
  void                  GetSpikeHeight (double&);
  void                  GetScaleFactor (double&);
  void                  GetSphereColor (double&, double&, double&);
  void                  GetSphereOpacity (double&);
  void                  GetSpikeColor (double&, double&, double&);
  void                  GetSpikeOpacity (double&);

private:
  MorningStarBase (const MorningStarBase&); // Not implemented.;
  void operator= (const MorningStarBase&); // Not implemented.;
};
#endif /* MORNING_STAR_BASE_H_ */
/*
 * End of: $Id: MorningStarBase.h,v 1.3 2004/06/15 05:15:12 xpxqx Exp $.
 *
 */
