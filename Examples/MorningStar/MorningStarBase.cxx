/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MorningStarBase.cxx,v 1.2 2004/05/13 18:37:49 xpxqx Exp $
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
#include "MorningStarBase.h"
#include "vtkSpikeyMace.h"
// VTK Graphics
#include "vtkConeSource.h"
#include "vtkPolyDataNormals.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
// FLTK
#include <FL/Fl_Window.H>
// vtkFLTK
#include "Fl_VTK_Window.H"
// C++ forwarding ANSI C
#include <cassert>


// ----------------------------------------------------------------------------
//      M o r n i n g S t a r B a s e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (MorningStarBase, "$Revision: 1.2 $");

// ----------------------------------------------------------------------------
MorningStarBase::MorningStarBase (void)
  : ConeWindow(NULL),
    ConeView(NULL),
    MaceWindow(NULL),
    MaceView(NULL),
    Cone(NULL),
    Mace(NULL)
{
}

MorningStarBase::~MorningStarBase()
{
  if (this->Cone != NULL)
    {
    this->Mace->UnRegister(this);
    this->Mace = NULL;
    }
  if (this->Cone != NULL)
    {
    this->Cone->UnRegister(this);
    this->Cone = NULL;
    }
  if (this->MaceWindow != NULL)
    {
    this->MaceWindow->hide();
    delete this->MaceWindow;
    this->MaceWindow = NULL;
    }
  if (this->ConeWindow != NULL)
    {
    this->ConeWindow->hide();
    delete this->ConeWindow;
    this->ConeWindow = NULL;
    }
}

// ----------------------------------------------------------------------------
void
MorningStarBase::InitializeCone (void)
{
  if (this->Cone == NULL)
    {
    this->Cone = vtkActor::New();
      {
      vtkPolyDataMapper* coneMapper = vtkPolyDataMapper::New();
        {
        vtkPolyDataNormals* coneNormals = vtkPolyDataNormals::New();
          {
          vtkConeSource* coneSource = vtkConeSource::New();
            {
            coneSource->SetResolution(24);
            }
          coneNormals->SetInputConnection(coneSource->GetOutputPort());
          coneSource->Delete();
          }
        coneMapper->SetInputConnection(coneNormals->GetOutputPort());
        coneNormals->Delete();
        }
      this->Cone->SetMapper(coneMapper);
      this->Cone->GetProperty()->SetColor(0.6f, 1.f, 0.8f);
      coneMapper->Delete();
      }
    this->ConeView->AddProp(this->Cone);
    this->UpdateConeView();
    }
}

void
MorningStarBase::InitializeMace (void)
{
  if (this->Mace == NULL)
    {
    this->Mace = vtkSpikeyMace::New();
      {
      this->Mace->SetSphereColor(0.9, 0.9, 0.3);
      this->Mace->SetSphereOpacity(0.6);
      this->Mace->SetSpikeColor(0.3, 0.6, 0.9);
      this->Mace->SetSpikeOpacity(1.0);
      }
    this->MaceView->AddProp(this->Mace);
    this->ReceiveMaceParam();
    this->UpdateMaceView();
    }
}

// ----------------------------------------------------------------------------
void
MorningStarBase::SetConeWindow (Fl_Window* aWindow)
{
  if      (this->ConeWindow != NULL)
    {
    vtkErrorMacro(<< "The ConeWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == NULL)
    {
    vtkErrorMacro(<< "The ConeWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_Window pointer is NULL.");
    }
  else
    {
    this->ConeWindow = aWindow;
    this->ConeWindow->user_data((void *) this);
    }
}

Fl_Window*
MorningStarBase::GetConeWindow (void) const
{
  return this->ConeWindow;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::SetConeView (Fl_VTK_Window* aWindow)
{
  if      (this->ConeView != NULL)
    {
    vtkErrorMacro(<< "The ConeView is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == NULL)
    {
    vtkErrorMacro(<< "The ConeView is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_VTK_Window pointer is NULL.");
    }
  else
    {
    this->ConeView = aWindow;
    this->ConeView->user_data((void *) this);
    // Let's get pretty ...
    this->ConeView->GetInteractor()->SetDesiredUpdateRate(2.0);
    this->ConeView->GetRenderWindow()->SetDesiredUpdateRate(2.0);
    this->ConeView->GetRenderWindow()->LineSmoothingOn();
    }
}

Fl_VTK_Window*
MorningStarBase::GetConeView (void) const
{
  return this->ConeView;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::SetMaceWindow (Fl_Window* aWindow)
{
  if      (this->MaceWindow != NULL)
    {
    vtkErrorMacro(<< "The MaceWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == NULL)
    {
    vtkErrorMacro(<< "The MaceWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_Window pointer is NULL.");
    }
  else
    {
    this->MaceWindow = aWindow;
    this->MaceWindow->user_data((void *) this);
    }
}

Fl_Window*
MorningStarBase::GetMaceWindow (void) const
{
  return this->MaceWindow;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::SetMaceView (Fl_VTK_Window* aWindow)
{
  if      (this->MaceView != NULL)
    {
    vtkErrorMacro(<< "The MaceView is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == NULL)
    {
    vtkErrorMacro(<< "The MaceView is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_VTK_Window pointer is NULL.");
    }
  else
    {
    this->MaceView = aWindow;
    this->MaceView->user_data((void *) this);
    // Let's get pretty ...
    this->MaceView->GetInteractor()->SetDesiredUpdateRate(2.0);
    this->MaceView->GetRenderWindow()->SetDesiredUpdateRate(2.0);
    this->MaceView->GetRenderWindow()->LineSmoothingOn();
    }
}

Fl_VTK_Window*
MorningStarBase::GetMaceView (void) const
{
  return this->MaceView;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::Show (int aArgc, char* aArgv[])
{
  if (this->ConeWindow != NULL)
    {
    this->ConeWindow->show(aArgc, aArgv);
    this->InitializeCone();
    }
}

void
MorningStarBase::Show (void)
{
  if (this->ConeWindow != NULL)
    {
    this->ConeWindow->show();
    this->InitializeCone();
    }
}

void
MorningStarBase::Hide (void)
{
  if (this->ConeWindow != NULL)
    {
    this->ConeWindow->hide();
    }
}

int
MorningStarBase::Shown (void) const
{
  if (this->ConeWindow != NULL)
    {
    return this->ConeWindow->shown();
    }

  return 0;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::ShowMace (int aArgc, char* aArgv[])
{
  if (this->MaceWindow != NULL)
    {
    this->MaceWindow->show(aArgc, aArgv);
    this->InitializeMace();
    }
}

void
MorningStarBase::ShowMace (void)
{
  if (this->MaceWindow != NULL)
    {
    this->MaceWindow->show();
    this->InitializeMace();
    }
}

void
MorningStarBase::HideMace (void)
{
  if (this->MaceWindow != NULL)
    {
    this->MaceWindow->hide();
    }
}

int
MorningStarBase::ShownMace (void) const
{
  if (this->MaceWindow != NULL)
    {
    return this->MaceWindow->shown();
    }

  return 0;
}

// ----------------------------------------------------------------------------
void
MorningStarBase::UpdateConeView (void)
{
  if (this->ConeView != NULL)
    {
    this->ConeView->Update();
    this->Check();
    }
}

void
MorningStarBase::UpdateMaceView (void)
{
  if (this->MaceView != NULL)
    {
    this->MaceView->Update();
    this->Check();
    }
}

// ----------------------------------------------------------------------------
void
MorningStarBase::SetThetaResolution (int a)
{
  assert(this->Mace != NULL);
  this->Mace->SetThetaResolution(a);
}

void
MorningStarBase::SetPhiResolution (int a)
{
  assert(this->Mace != NULL);
  this->Mace->SetPhiResolution(a);
}

void
MorningStarBase::SetSpikeResolution (int a)
{
  assert(this->Mace != NULL);
  this->Mace->SetSpikeResolution(a);
}

void
MorningStarBase::SetSpikeRadius (double a)
{
  assert(this->Mace != NULL);
  this->Mace->SetSpikeRadius(a);
}

void
MorningStarBase::SetSpikeHeight (double a)
{
  assert(this->Mace != NULL);
  this->Mace->SetSpikeHeight(a);
}

void
MorningStarBase::SetScaleFactor (double a)
{
  assert(this->Mace != NULL);
  this->Mace->SetScaleFactor(a);
}

void
MorningStarBase::SetSphereColor (double r, double g, double b)
{
  assert(this->Mace != NULL);
  this->Mace->SetSphereColor(r,g,b);
}

void
MorningStarBase::SetSphereOpacity (double a)
{
  assert(this->Mace != NULL);
  this->Mace->SetSphereOpacity(a);
}

void
MorningStarBase::SetSpikeColor (double r, double g, double b)
{
  assert(this->Mace != NULL);
  this->Mace->SetSpikeColor(r,g,b);
}

void
MorningStarBase::SetSpikeOpacity (double a)
{
  assert(this->Mace != NULL);
  this->Mace->SetSpikeOpacity(a);
}

// ----------------------------------------------------------------------------
void
MorningStarBase::GetThetaResolution (int& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetThetaResolution(a);
}

void
MorningStarBase::GetPhiResolution (int& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetPhiResolution(a);
}

void
MorningStarBase::GetSpikeResolution (int& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetSpikeResolution(a);
}

void
MorningStarBase::GetSpikeRadius (double& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetSpikeRadius(a);
}

void
MorningStarBase::GetSpikeHeight (double& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetSpikeHeight(a);
}

void
MorningStarBase::GetScaleFactor (double& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetScaleFactor(a);
}

void
MorningStarBase::GetSphereColor (double& r, double& g, double& b)
{
  assert(this->Mace != NULL);
  this->Mace->GetSphereColor(r,g,b);
}

void
MorningStarBase::GetSphereOpacity (double& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetSphereOpacity(a);
}

void
MorningStarBase::GetSpikeColor (double& r, double& g, double& b)
{
  assert(this->Mace != NULL);
  this->Mace->GetSpikeColor(r,g,b);
}

void
MorningStarBase::GetSpikeOpacity (double& a)
{
  assert(this->Mace != NULL);
  this->Mace->GetSpikeOpacity(a);
}

/*
 * End of: $Id: MorningStarBase.cxx,v 1.2 2004/05/13 18:37:49 xpxqx Exp $.
 *
 */
