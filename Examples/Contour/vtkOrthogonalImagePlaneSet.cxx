/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkOrthogonalImagePlaneSet.cxx,v 1.5 2004/06/16 01:42:50 xpxqx Exp $
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
#include "vtkOrthogonalImagePlaneSet.h"
#include "vtkFLTKConfigure.h"
// VTK Common
#include "vtkImageData.h"
#include "vtkCellPicker.h"
#include "vtkLookupTable.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
// VTK Rendering
#include "vtkProperty.h"
#include "vtkTextProperty.h"
#include "vtkRenderWindowInteractor.h"
// VTK Hybrid
#include "vtkImagePlaneWidget.h"
// VTK Common
#include "vtkObjectFactory.h"

typedef double vtkFloatingPointType;

// ----------------------------------------------------------------------------
//      v t k O r t h o g o n a l I m a g e P l a n e S e t
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (vtkOrthogonalImagePlaneSet, "$Revision: 1.5 $");
vtkStandardNewMacro (vtkOrthogonalImagePlaneSet);

// ----------------------------------------------------------------------------
vtkOrthogonalImagePlaneSet::vtkOrthogonalImagePlaneSet (void)
  : CellPicker(vtkCellPicker::New()),
    TextProperty(vtkTextProperty::New()),
    XPlane(vtkImagePlaneWidget::New()),
    YPlane(vtkImagePlaneWidget::New()),
    ZPlane(vtkImagePlaneWidget::New()),
    ColorTF(vtkColorTransferFunction::New()),
    OpacityTF(vtkPiecewiseFunction::New())
{
  this->CellPicker->SetTolerance(0.005);

  this->TextProperty->SetColor(1,1,0.9);
  this->TextProperty->ShadowOn();
  //  this->TextProperty->AntiAliasingOn();

  this->XPlane->SetKeyPressActivationValue('x');
  this->XPlane->SetPicker(this->CellPicker);
  this->XPlane->GetPlaneProperty()->SetColor(1,0,0);
  this->XPlane->SetCursorProperty(this->XPlane->GetPlaneProperty());
  this->XPlane->DisplayTextOn();
  this->XPlane->SetTextProperty(this->TextProperty);
  this->XPlane->TextureInterpolateOn();

  this->YPlane->SetKeyPressActivationValue('y');
  this->YPlane->SetPicker(this->CellPicker);
  this->YPlane->GetPlaneProperty()->SetColor(1,1,0);
  this->YPlane->SetCursorProperty(this->YPlane->GetPlaneProperty());
  this->YPlane->DisplayTextOn();
  this->YPlane->SetTextProperty(this->TextProperty);
  this->YPlane->TextureInterpolateOn();
  this->YPlane->SetLookupTable(this->XPlane->GetLookupTable());

  this->ZPlane->SetKeyPressActivationValue('z');
  this->ZPlane->SetPicker(this->CellPicker);
  this->ZPlane->GetPlaneProperty()->SetColor(0,0,1);
  this->ZPlane->SetCursorProperty(this->ZPlane->GetPlaneProperty());
  this->ZPlane->DisplayTextOn();
  this->ZPlane->SetTextProperty(this->TextProperty);
  this->ZPlane->TextureInterpolateOn();
  this->ZPlane->SetLookupTable(this->XPlane->GetLookupTable());
}

vtkOrthogonalImagePlaneSet::~vtkOrthogonalImagePlaneSet()
{
  this->XPlane->Delete();
  this->YPlane->Delete();
  this->ZPlane->Delete();
  this->CellPicker->Delete();
  this->TextProperty->Delete();
  this->ColorTF->Delete();
  this->OpacityTF->Delete();
}

// ----------------------------------------------------------------------------

void
vtkOrthogonalImagePlaneSet::On (void)
{
  if (this->XPlane->GetInteractor() != NULL) this->XPlane->On();
  if (this->YPlane->GetInteractor() != NULL) this->YPlane->On();
  if (this->ZPlane->GetInteractor() != NULL) this->ZPlane->On();
}

void
vtkOrthogonalImagePlaneSet::Off (void)
{
  if (this->XPlane->GetInteractor() != NULL) this->XPlane->Off();
  if (this->YPlane->GetInteractor() != NULL) this->YPlane->Off();
  if (this->ZPlane->GetInteractor() != NULL) this->ZPlane->Off();
}

// ----------------------------------------------------------------------------
void
vtkOrthogonalImagePlaneSet::SetInteractor (vtkRenderWindowInteractor* aRWI)
{
  if (aRWI == NULL)
    return;

  this->XPlane->SetInteractor(aRWI);
  this->YPlane->SetInteractor(aRWI);
  this->ZPlane->SetInteractor(aRWI);

  this->On();

  this->Modified();
}

void
vtkOrthogonalImagePlaneSet::SetInput (vtkImageData* aImageData)
{
  if (aImageData == NULL)
    return;

  int dimensions[3];

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType range[2], win;
  vtkFloatingPointType origin[3];

  //  aImageData->Update();
  aImageData->GetDimensions(dimensions);
  aImageData->GetSpacing(spacing);
  aImageData->GetOrigin(origin);
  aImageData->GetScalarRange(range);
  win = range[1] - range[0];

  // VTK_NEAREST_RESLICE | VTK_LINEAR_RESLICE | VTK_CUBIC_RESLICE
  int yzInterpolation = VTK_LINEAR_RESLICE;
  if (spacing[1]!=spacing[0] || spacing[2]!=spacing[1])
    {
    yzInterpolation = VTK_CUBIC_RESLICE;
    }

  this->XPlane->SetInputData(aImageData);
  this->YPlane->SetInputData(aImageData);
  this->ZPlane->SetInputData(aImageData);

  this->XPlane->SetPlaneOrientationToXAxes();
  this->YPlane->SetPlaneOrientationToYAxes();
  this->ZPlane->SetPlaneOrientationToZAxes();

  this->XPlane->SetResliceInterpolate(VTK_LINEAR_RESLICE);
  this->YPlane->SetResliceInterpolate(yzInterpolation);
  this->ZPlane->SetResliceInterpolate(yzInterpolation);

  this->XPlane->SetSlicePosition(origin[0] + dimensions[0] * spacing[0] * 0.2);
  this->YPlane->SetSlicePosition(origin[1] + dimensions[1] * spacing[1] * 0.2);
  this->ZPlane->SetSlicePosition(origin[2] + dimensions[2] * spacing[2] * 0.2);

  // Set the internal lut to that of the first plane. In this way, 
  // the set of three orthogonal planes can share the same lut so that 
  // window-levelling is performed uniformly among planes.
  this->ColorTF->RemoveAllPoints();
  this->ColorTF->AddRGBPoint( range[0] + win * 0.300, 1.0,  0.0,  0.0 );
  this->ColorTF->AddRGBPoint( range[0] + win * 0.400, 1.0,  1.0,  0.0 );
  this->ColorTF->AddRGBPoint( range[0] + win * 0.500, 0.0,  1.0,  0.0 );
  this->ColorTF->AddRGBPoint( range[0] + win * 0.600, 0.0,  1.0,  1.0 );
  this->ColorTF->AddRGBPoint( range[0] + win * 0.700, 0.0,  0.0,  1.0 );
  this->ColorTF->AddRGBPoint( range[0] + win * 0.800, 1.0,  0.0,  1.0 );

  this->OpacityTF->RemoveAllPoints();
  this->OpacityTF->AddSegment( range[0] + win * 0.000, 0.000,
                               range[0] + win * 0.159, 0.250 );
  this->OpacityTF->AddSegment( range[0] + win * 0.251, 0.500,
                               range[0] + win * 0.398, 0.750 );
  this->OpacityTF->AddSegment( range[0] + win * 0.630, 1.000,
                               range[0] + win * 1.000, 1.000 );

  vtkLookupTable*   lut     = this->XPlane->GetLookupTable();
  vtkIdType         m       = lut->GetNumberOfTableValues();

  vtkFloatingPointType value   = range[0];
  vtkFloatingPointType incr    = win / vtkFloatingPointType(m);
  vtkFloatingPointType rgba[4];

  for (vtkIdType n=0; n<m; n++)
    {
    this->ColorTF->GetColor(value, rgba);
    rgba[3] = this->OpacityTF->GetValue(value);
    lut->SetTableValue(n, rgba);
    value += incr;
    }

  this->Modified();
}

vtkCellPicker*
vtkOrthogonalImagePlaneSet::GetPicker (void)
{
  return this->CellPicker;
}

vtkTextProperty*
vtkOrthogonalImagePlaneSet::GetTextProperty (void)
{
  return this->TextProperty;
}

vtkImagePlaneWidget*
vtkOrthogonalImagePlaneSet::GetImagePlaneWidget (int aPlane)
{
  switch (aPlane)
    {
    case 0:
      return this->XPlane;
    case 1:
      return this->YPlane;
    case 2:
      return this->ZPlane;
    default:
      vtkWarningMacro(<< "GetImagePlaneWidget(): plane # '" << aPlane
                      << "' out of range.");
      return NULL;
    }
}

vtkColorTransferFunction*
vtkOrthogonalImagePlaneSet::GetColorTF (void)
{
  return this->ColorTF;
}

vtkPiecewiseFunction*
vtkOrthogonalImagePlaneSet::GetOpacityTF (void)
{
  return this->OpacityTF;
}

void
vtkOrthogonalImagePlaneSet::PrintSelf (ostream& aTarget, vtkIndent aIndent)
{
  this->Superclass::PrintSelf(aTarget, aIndent);

  aTarget << aIndent << "CellPicker:   " << this->CellPicker << endl;
  if (this->CellPicker != NULL)
    this->CellPicker->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "TextProperty: " << this->TextProperty << endl;
  if (this->TextProperty != NULL)
    this->TextProperty->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "XPlane:       " << this->XPlane << endl;
  if (this->XPlane != NULL)
    this->XPlane->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "YPlane:       " << this->YPlane << endl;
  if (this->YPlane != NULL)
    this->YPlane->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "ZPlane:       " << this->ZPlane << endl;
  if (this->ZPlane != NULL)
    this->ZPlane->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "ColorTF:      " << this->ColorTF << endl;
  if (this->ColorTF != NULL)
    this->ColorTF->PrintSelf(aTarget, aIndent.GetNextIndent());

  aTarget << aIndent << "OpacityTF:    " << this->OpacityTF << endl;
  if (this->OpacityTF != NULL)
    this->OpacityTF->PrintSelf(aTarget, aIndent.GetNextIndent());
}

/*
 * End of: $Id: vtkOrthogonalImagePlaneSet.cxx,v 1.5 2004/06/16 01:42:50 xpxqx Exp $.
 *
 */
