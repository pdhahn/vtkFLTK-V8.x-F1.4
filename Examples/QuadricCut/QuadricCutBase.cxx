/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: QuadricCutBase.cxx,v 1.5 2004/05/13 18:37:50 xpxqx Exp $
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
#include "QuadricCutBase.h"
// VTK Common
#include "vtkPolyData.h"
#include "vtkQuadric.h"
// VTK Imaging
#include "vtkBooleanTexture.h"
// VTK Graphics
#include "vtkSphereSource.h"
#include "vtkImplicitTextureCoords.h"
// VTK Rendering
#include "vtkDataSetMapper.h"
#include "vtkTexture.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
// FLTK
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
// vtkFLTK
#include "Fl_VTK_Window.H"

// ----------------------------------------------------------------------------
//      Q u a d r i c C u t B a s e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (QuadricCutBase, "$Revision: 1.5 $");

// ----------------------------------------------------------------------------
float lamp_black[] = { 0.180f, 0.280f, 0.230f };
float banana[]     = { 0.890f, 0.810f, 0.340f };
float khaki[]      = { 0.941f, 0.902f, 0.549f };
float tomato[]     = { 1.000f, 0.388f, 0.278f };

class QuadricCutInternal
{
public:
  vtkRenderer*          Renderer;

  unsigned char         White[2];
  unsigned char         Clear[2];
  unsigned char         Black[2];

  vtkBooleanTexture*    BooleanTexture;

  vtkActor*             SphereActor;

  QuadricCutInternal (void)
    : Renderer(NULL),
      BooleanTexture(vtkBooleanTexture::New()),
      SphereActor(NULL)
    {
      this->White[0] = 0xFF;
      this->White[1] = 0xFF;
      this->Clear[0] = 0xFF;
      this->Clear[1] = 0x00;
      this->Black[0] = 0x00;
      this->Black[1] = 0xFF;

      this->BooleanTexture->SetXSize(256);
      this->BooleanTexture->SetYSize(256);
      this->BooleanTexture->SetThickness(1);
      this->BooleanTexture->SetInIn(this->White);
      this->BooleanTexture->SetOutIn(this->White);
      this->BooleanTexture->SetInOut(this->White);
      this->BooleanTexture->SetOutOut(this->White);
      this->BooleanTexture->SetOnOn(this->White);
      this->BooleanTexture->SetOnIn(this->White);
      this->BooleanTexture->SetOnOut(this->White);
      this->BooleanTexture->SetInOn(this->White);
      this->BooleanTexture->SetOutOn(this->White);
    }

  ~QuadricCutInternal()
    {
      this->BooleanTexture->Delete();

      if (this->SphereActor != NULL)
        this->SphereActor->Delete();
    }

  void  AddActors (vtkRenderer* aRenderer)
    {
      if ((this->Renderer = aRenderer) == NULL)
        return;

      this->SphereActor = vtkActor::New();
        {
        vtkDataSetMapper* dataSetMapper = vtkDataSetMapper::New();
          {
          // create texture coordianates for all
          vtkImplicitTextureCoords* tcoords = vtkImplicitTextureCoords::New();
            {
            // create a sphere for all to use
            vtkSphereSource* sphereSource = vtkSphereSource::New();
              {
              sphereSource->SetPhiResolution(50);
              sphereSource->SetThetaResolution(50);
              }
            // define two elliptical cylinders
            vtkQuadric* quadric1 = vtkQuadric::New();
              {
              quadric1->SetCoefficients(1, 3, 0, 0, 0, 0, 0, 0, 0, -0.15);
              }
            vtkQuadric* quadric2 = vtkQuadric::New();
              {
              quadric2->SetCoefficients(3, 1, 0, 0, 0, 0, 0, 0, 0, -0.075);
              }
            tcoords->SetInputConnection(sphereSource->GetOutputPort());
            tcoords->SetRFunction(quadric1);
            tcoords->SetSFunction(quadric2);
            sphereSource->Delete();
            quadric1->Delete();
            quadric2->Delete();
            }
          dataSetMapper->SetInputConnection(tcoords->GetOutputPort());
          tcoords->Delete();
          }
        vtkTexture* texture = vtkTexture::New();
          {
          texture->SetInputConnection(this->BooleanTexture->GetOutputPort());
          texture->InterpolateOff();
          texture->RepeatOff();
          }
        this->SphereActor->SetMapper(dataSetMapper);
        this->SphereActor->SetTexture(texture);
        this->SphereActor->GetProperty()->SetDiffuse(1.0);
        this->SphereActor->GetProperty()->SetAmbient(1.0);
        this->SphereActor->GetProperty()->SetSpecular(1.0);
        this->SphereActor->GetProperty()->SetAmbientColor(0.247, 0.225, 0.065);
        this->SphereActor->GetProperty()->SetDiffuseColor(0.346, 0.314, 0.090);
        this->SphereActor->GetProperty()->SetSpecularColor(0.797, 0.724, 0.208);
        this->SphereActor->GetProperty()->SetSpecularPower(83.20);
        dataSetMapper->Delete();
        texture->Delete();
        }

      aRenderer->AddActor(this->SphereActor);
    }

  void  SetInIn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetInIn(this->Clear); break;
        case 2:          this->BooleanTexture->SetInIn(this->Black); break;
        case 0: default: this->BooleanTexture->SetInIn(this->White); break;
        }
    }

  void  SetOutIn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOutIn(this->Clear); break;
        case 2:          this->BooleanTexture->SetOutIn(this->Black); break;
        case 0: default: this->BooleanTexture->SetOutIn(this->White); break;
        }
    }

  void  SetInOut (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetInOut(this->Clear); break;
        case 2:          this->BooleanTexture->SetInOut(this->Black); break;
        case 0: default: this->BooleanTexture->SetInOut(this->White); break;
        }
    }

  void  SetOutOut (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOutOut(this->Clear); break;
        case 2:          this->BooleanTexture->SetOutOut(this->Black); break;
        case 0: default: this->BooleanTexture->SetOutOut(this->White); break;
        }
    }

  void  SetOnOn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOnOn(this->Clear); break;
        case 2:          this->BooleanTexture->SetOnOn(this->Black); break;
        case 0: default: this->BooleanTexture->SetOnOn(this->White); break;
        }
    }

  void  SetOnIn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOnIn(this->Clear); break;
        case 2:          this->BooleanTexture->SetOnIn(this->Black); break;
        case 0: default: this->BooleanTexture->SetOnIn(this->White); break;
        }
    }

  void  SetOnOut (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOnOut(this->Clear); break;
        case 2:          this->BooleanTexture->SetOnOut(this->Black); break;
        case 0: default: this->BooleanTexture->SetOnOut(this->White); break;
        }
    }

  void  SetInOn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetInOn(this->Clear); break;
        case 2:          this->BooleanTexture->SetInOn(this->Black); break;
        case 0: default: this->BooleanTexture->SetInOn(this->White); break;
        }
    }

  void  SetOutOn (int a)
    {
      switch (a)
        {
        case 1:          this->BooleanTexture->SetOutOn(this->Clear); break;
        case 2:          this->BooleanTexture->SetOutOn(this->Black); break;
        case 0: default: this->BooleanTexture->SetOutOn(this->White); break;
        }
    }

private:
  QuadricCutInternal (const QuadricCutInternal&); // Not Implemented.
  QuadricCutInternal& operator= (const QuadricCutInternal&); // Not Implemented.
};

// ----------------------------------------------------------------------------
QuadricCutBase::QuadricCutBase (void)
  : Internal(new QuadricCutInternal),
    FltkWindow(0),
    VtkWindow(0)
{
}

QuadricCutBase::~QuadricCutBase()
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    delete this->FltkWindow;
    }

  delete this->Internal;
}

// ----------------------------------------------------------------------------
void
QuadricCutBase::SetFltkWindow (Fl_Window* aWindow)
{
  if      (this->FltkWindow != (Fl_Window *) 0)
    {
    vtkErrorMacro(<< "The FltkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == (Fl_Window *) 0)
    {
    vtkErrorMacro(<< "The FltkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_Window pointer is NULL.");
    }
  else
    {
    this->FltkWindow = aWindow;
    this->FltkWindow->user_data((void *) this);
    }
}

Fl_Window*
QuadricCutBase::GetFltkWindow (void) const
{
  return this->FltkWindow;
}

// ----------------------------------------------------------------------------
void
QuadricCutBase::SetVtkWindow (Fl_VTK_Window* aWindow)
{
  if      (this->VtkWindow != (Fl_VTK_Window *) 0)
    {
    vtkErrorMacro(<< "The VtkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == (Fl_VTK_Window *) 0)
    {
    vtkErrorMacro(<< "The VtkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_VTK_Window pointer is NULL.");
    }
  else
    {
    this->VtkWindow = aWindow;
    this->VtkWindow->user_data((void *) this);
    }
}

Fl_VTK_Window*
QuadricCutBase::GetVtkWindow (void) const
{
  return this->VtkWindow;
}

// ----------------------------------------------------------------------------
void
QuadricCutBase::SetInIn (int a)
{
  this->Internal->SetInIn(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOutIn (int a)
{
  this->Internal->SetOutIn(a);
  this->UpdateView();
}

void
QuadricCutBase::SetInOut (int a)
{
  this->Internal->SetInOut(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOutOut (int a)
{
  this->Internal->SetOutOut(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOnOn (int a)
{
  this->Internal->SetOnOn(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOnIn (int a)
{
  this->Internal->SetOnIn(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOnOut (int a)
{
  this->Internal->SetOnOut(a);
  this->UpdateView();
}

void
QuadricCutBase::SetInOn (int a)
{
  this->Internal->SetInOn(a);
  this->UpdateView();
}

void
QuadricCutBase::SetOutOn (int a)
{
  this->Internal->SetOutOn(a);
  this->UpdateView();
}

// ----------------------------------------------------------------------------
void
QuadricCutBase::Show (int aArgc, char* aArgv[])
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show(aArgc, aArgv);

    if (this->Internal->Renderer == NULL && this->VtkWindow != NULL)
      {
      this->Internal->AddActors(this->VtkWindow->GetDefaultRenderer());
      this->VtkWindow->GetRenderWindow()->Render();
      }
    }
}

void
QuadricCutBase::Show (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show();

    if (this->Internal->Renderer == NULL && this->VtkWindow != NULL)
      {
      this->Internal->AddActors(this->VtkWindow->GetDefaultRenderer());
      this->VtkWindow->GetRenderWindow()->Render();
      }
    }
}

int
QuadricCutBase::Shown (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    return this->FltkWindow->shown();
    }

  return 0;
}

void
QuadricCutBase::Hide (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    }
}

void
QuadricCutBase::UpdateView (void)
{
  if (this->Internal->Renderer != NULL && this->VtkWindow != NULL)
    {
    this->VtkWindow->Update();
    }
}

/*
 * End of: $Id: QuadricCutBase.cxx,v 1.5 2004/05/13 18:37:50 xpxqx Exp $.
 *
 */
