/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkSpikeyMace.cxx,v 1.3 2004/05/13 18:37:49 xpxqx Exp $
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
// VTK pipeline
#include "vtkSpikeyMace.h"
#include "vtkPolyData.h"
#include "vtkSphereSource.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkLODActor.h"
#include "vtkGlyph3D.h"
#include "vtkObjectFactory.h"

class vtkSpikeyMaceInternal
{
public:
  vtkSpikeyMaceInternal (void)
    : SphereSource(vtkSphereSource::New()),
      SphereMapper(vtkPolyDataMapper::New()),
      SphereActor(vtkLODActor::New()),
      SpikeSource(vtkConeSource::New()),
      SpikeGlyph(vtkGlyph3D::New()),
      SpikeMapper(vtkPolyDataMapper::New()),
      SpikeActor(vtkLODActor::New())
    {
      // SphereSource
      this->SphereSource->SetThetaResolution(8);
      this->SphereSource->SetPhiResolution(8);
      // SphereMapper
      this->SphereMapper->SetInputConnection(SphereSource->GetOutputPort());
      // SphereActor - DO NOT Delete() refererence herein!
      this->SphereActor->SetMapper(SphereMapper);
      this->SphereActor->GetProperty()->SetColor(1.0, 0.0, 1.0);
      this->SphereActor->GetProperty()->SetOpacity(1.0);
      // SpikeSource
      this->SpikeSource->SetResolution(8);
      this->SpikeSource->SetHeight(1.0);
      this->SpikeSource->SetRadius(0.5);
      // SpikeGlyph
      this->SpikeGlyph->SetInputConnection(SphereSource->GetOutputPort());
      this->SpikeGlyph->SetSourceConnection(SpikeSource->GetOutputPort());
      this->SpikeGlyph->SetVectorModeToUseNormal();
      this->SpikeGlyph->SetScaleModeToScaleByVector();
      this->SpikeGlyph->SetScaleFactor(0.25);
      // SpikeMapper
      this->SpikeMapper->SetInputConnection(SpikeGlyph->GetOutputPort());
      // SpikeActor - DO NOT Delete() refererence herein!
      this->SpikeActor->SetMapper(SpikeMapper);
      this->SpikeActor->GetProperty()->SetColor(1.0, 1.0, 0.0);
      this->SpikeActor->GetProperty()->SetOpacity(1.0);
    }

  ~vtkSpikeyMaceInternal()
    {
      if (this->SphereSource != NULL)
        {
        this->SphereSource->Delete();
        }
      if (this->SphereMapper != NULL)
        {
        this->SphereMapper->Delete();
        }
      if (this->SpikeSource != NULL)
        {
        this->SpikeSource->Delete();
        }
      if (this->SpikeGlyph != NULL)
        {
        this->SpikeGlyph->Delete();
        }
      if (this->SpikeMapper != NULL)
        {
        this->SpikeMapper->Delete();
        }
      if (this->SphereActor != NULL)
        {
        this->SphereActor->Delete();
        }
      if (this->SpikeActor != NULL)
        {
        this->SpikeActor->Delete();
        }
    }

  vtkSphereSource*   SphereSource;
  vtkPolyDataMapper* SphereMapper;
  vtkLODActor*       SphereActor;
  vtkConeSource*     SpikeSource;
  vtkGlyph3D*        SpikeGlyph;
  vtkPolyDataMapper* SpikeMapper;
  vtkLODActor*       SpikeActor;

private:
  vtkSpikeyMaceInternal (const vtkSpikeyMaceInternal&);
  void operator= (const vtkSpikeyMaceInternal&);
};

// ----------------------------------------------------------------------------
//      v t k S p i k e y M a c e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (vtkSpikeyMace, "$Revision: 1.3 $");
vtkStandardNewMacro (vtkSpikeyMace);

// ----------------------------------------------------------------------------
vtkSpikeyMace::vtkSpikeyMace (void) : Internal(new vtkSpikeyMaceInternal)
{
  this->AddPart(this->Internal->SphereActor);
  this->AddPart(this->Internal->SpikeActor);
}

vtkSpikeyMace::~vtkSpikeyMace()
{
  delete this->Internal;
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetThetaResolution (int const& a)
{
  this->Internal->SphereSource->SetThetaResolution(a);
}

void
vtkSpikeyMace::GetThetaResolution (int & a)
{
  a = this->Internal->SphereSource->GetThetaResolution();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetPhiResolution (int const& a)
{
  this->Internal->SphereSource->SetPhiResolution(a);
}

void
vtkSpikeyMace::GetPhiResolution (int & a)
{
  a = this->Internal->SphereSource->GetPhiResolution();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetSpikeHeight (double const& a)
{
  this->Internal->SpikeSource->SetHeight(a);
}

void
vtkSpikeyMace::GetSpikeHeight (double & a)
{
  a = this->Internal->SpikeSource->GetHeight();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetSpikeRadius (double const& a)
{
  this->Internal->SpikeSource->SetRadius(a);
}

void
vtkSpikeyMace::GetSpikeRadius (double & a)
{
  a = this->Internal->SpikeSource->GetRadius();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetSpikeResolution (int const& a)
{
  this->Internal->SpikeSource->SetResolution(a);
}

void
vtkSpikeyMace::GetSpikeResolution (int & a)
{
  a = this->Internal->SpikeSource->GetResolution();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetScaleFactor (double const& a)
{
  this->Internal->SpikeGlyph->SetScaleFactor(a);
}

void
vtkSpikeyMace::GetScaleFactor (double & a)
{
  a = this->Internal->SpikeGlyph->GetScaleFactor();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetSphereColor (double const& aR,
                               double const& aG,
                               double const& aB)
{
  this->Internal->SphereActor->GetProperty()->SetColor(aR, aG, aB);
}

void
vtkSpikeyMace::SetSphereColor (const double a[3])
{
  this->Internal->SphereActor->GetProperty()->SetColor(a[0], a[1], a[2]);
}

void
vtkSpikeyMace::GetSphereColor (double      & aR,
                               double      & aG,
                               double      & aB)
{
  aR = (this->Internal->SphereActor->GetProperty()->GetColor())[0];
  aG = (this->Internal->SphereActor->GetProperty()->GetColor())[1];
  aB = (this->Internal->SphereActor->GetProperty()->GetColor())[2];
}

void
vtkSpikeyMace::GetSphereColor (double a[3])
{
  a[0] = (this->Internal->SphereActor->GetProperty()->GetColor())[0];
  a[1] = (this->Internal->SphereActor->GetProperty()->GetColor())[1];
  a[2] = (this->Internal->SphereActor->GetProperty()->GetColor())[2];
}

void
vtkSpikeyMace::SetSphereOpacity (double const& a)
{
  this->Internal->SphereActor->GetProperty()->SetOpacity(a);
}

void
vtkSpikeyMace::GetSphereOpacity (double & a)
{
  a = this->Internal->SphereActor->GetProperty()->GetOpacity();
}

// ----------------------------------------------------------------------------
void
vtkSpikeyMace::SetSpikeColor (double const& aR,
                              double const& aG,
                              double const& aB)
{
  this->Internal->SpikeActor->GetProperty()->SetColor(aR, aG, aB);
}

void
vtkSpikeyMace::SetSpikeColor (const double a[3])
{
  this->Internal->SpikeActor->GetProperty()->SetColor(a[0], a[1], a[2]);
}

void
vtkSpikeyMace::GetSpikeColor (double      & aR,
                              double      & aG,
                              double      & aB)
{
  aR = (this->Internal->SpikeActor->GetProperty()->GetColor())[0];
  aG = (this->Internal->SpikeActor->GetProperty()->GetColor())[1];
  aB = (this->Internal->SpikeActor->GetProperty()->GetColor())[2];
}

void
vtkSpikeyMace::GetSpikeColor (double a[3])
{
  a[0] = (this->Internal->SpikeActor->GetProperty()->GetColor())[0];
  a[1] = (this->Internal->SpikeActor->GetProperty()->GetColor())[1];
  a[2] = (this->Internal->SpikeActor->GetProperty()->GetColor())[2];
}

void
vtkSpikeyMace::SetSpikeOpacity (double const& a)
{
  this->Internal->SpikeActor->GetProperty()->SetOpacity(a);
}

void
vtkSpikeyMace::GetSpikeOpacity (double & a)
{
  a = this->Internal->SpikeActor->GetProperty()->GetOpacity();
}

/*
 * End of: $Id: vtkSpikeyMace.cxx,v 1.3 2004/05/13 18:37:49 xpxqx Exp $.
 *
 */
