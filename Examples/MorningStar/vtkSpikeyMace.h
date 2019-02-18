/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkSpikeyMace.h,v 1.2 2004/05/13 18:37:50 xpxqx Exp $
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
#ifndef VTK_SPIKEY_MACE_H_
#  define VTK_SPIKEY_MACE_H_
#  include "vtkAssembly.h"

#  include "vtkFLTKConfigure.h"

class vtkSpikeyMaceInternal;

class VTK_EXPORT vtkSpikeyMace : public vtkAssembly
{
public:
  static vtkSpikeyMace* New (void);
  vtkTypeMacro (vtkSpikeyMace, vtkAssembly);

  void    SetThetaResolution (int const& res);
  void    GetThetaResolution (int      & res);

  void    SetPhiResolution (int const& res);
  void    GetPhiResolution (int      & res);

  void    SetSpikeHeight (double const& height);
  void    GetSpikeHeight (double      & height);

  void    SetSpikeRadius (double const& radius);
  void    GetSpikeRadius (double      & radius);

  void    SetSpikeResolution (int const& res);
  void    GetSpikeResolution (int      & res);

  void    SetScaleFactor (double const& f);
  void    GetScaleFactor (double      & f);

  void    SetSphereColor (double const& r,
                          double const& g,
                          double const& b);
  void    SetSphereColor (const double rgb[3]);
  void    GetSphereColor (double      & r,
                          double      & g,
                          double      & b);
  void    GetSphereColor (double rgb[3]);

  void    SetSphereOpacity (double const& a);
  void    GetSphereOpacity (double      & a);

  void    SetSpikeColor (double const& r,
                         double const& g,
                         double const& b);
  void    SetSpikeColor (const double rgb[3]);
  void    GetSpikeColor (double      & r,
                         double      & g,
                         double      & b);
  void    GetSpikeColor (double rgb[3]);

  void    SetSpikeOpacity (double const& a);
  void    GetSpikeOpacity (double      & a);

protected:
  vtkSpikeyMace (void);
  ~vtkSpikeyMace();

  vtkSpikeyMaceInternal* Internal;

private:
  vtkSpikeyMace (const vtkSpikeyMace&); // Not Implemented.
  vtkSpikeyMace& operator= (const vtkSpikeyMace&); // Not Implemented.
};

#endif /* VTK_SPIKEY_MACE_H_ */
/*
 * End of: $Id: vtkSpikeyMace.h,v 1.2 2004/05/13 18:37:50 xpxqx Exp $.
 *
 */
