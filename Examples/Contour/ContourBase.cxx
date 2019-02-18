/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: ContourBase.cxx,v 1.5 2004/06/16 01:42:49 xpxqx Exp $
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
#include "ContourBase.h"
#include "vtkOrthogonalImagePlaneSet.h"
// VTK Common
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkStructuredPoints.h"
#include "vtkPolyData.h"
#include "vtkColorTransferFunction.h"
#include "vtkTransform.h"
// VTK Imaging
#include "vtkImageAlgorithm.h"
#include "vtkImageAnisotropicDiffusion3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageCast.h"
#include "vtkImageToStructuredPoints.h"
// VTK Graphics
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkContourFilter.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataNormals.h"
// VTK IO
#include "vtkPNGWriter.h"
#include "vtkImageReader.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkLODActor.h"
#include "vtkCamera.h"
#include "vtkRendererSource.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
// vtkFLTK
#include "Fl_VTK_Window.H"

#define VTK_LARGE_FLOAT  DBL_MAX

// ----------------------------------------------------------------------------
//      C o n t o u r B a s e I n t e r n a l
// ----------------------------------------------------------------------------
class ContourBaseInternal
{
public:
  bool                                  Initialized;

  vtkImageReader*                       ImageReader;
  vtkImageAlgorithm*                    ImageSource1;
  vtkImageAlgorithm*                    ImageSource2;
  vtkPolyDataConnectivityFilter*        ConnectivityFilter;
  vtkContourFilter*                     ContourFilter;

  vtkActor*                             ContourActor;
  vtkActor*                             BoundingActor;

  vtkOrthogonalImagePlaneSet*           ImagePlaneSet;

  float                                 Min;
  float                                 Max;
  float                                 Window;


  ContourBaseInternal (void)
    : Initialized(false),
      ImageReader(vtkImageReader::New()),
      ImageSource1(vtkImageGaussianSmooth::New()),
      ImageSource2(vtkImageAnisotropicDiffusion3D::New()),
      ConnectivityFilter(vtkPolyDataConnectivityFilter::New()),
      ContourFilter(vtkContourFilter::New()),
      ContourActor(vtkLODActor::New()),
      BoundingActor(vtkActor::New()),
      ImagePlaneSet(vtkOrthogonalImagePlaneSet::New()),
      Min(VTK_LARGE_FLOAT),
      Max(-VTK_LARGE_FLOAT),
      Window(1.0)
    {
    }

  ~ContourBaseInternal()
    {
      if (this->ImageReader != NULL)
        this->ImageReader->Delete();
      if (this->ImageSource1 != NULL)
        this->ImageSource1->Delete();
      if (this->ImageSource2 != NULL)
        this->ImageSource2->Delete();
      if (this->ConnectivityFilter != NULL)
        this->ConnectivityFilter->Delete();
      if (this->ContourFilter != NULL)
        this->ContourFilter->Delete();
      if (this->BoundingActor != NULL)
        this->BoundingActor->Delete();
      if (this->ContourActor != NULL)
        this->ContourActor->Delete();
      if (this->ImagePlaneSet != NULL)
        this->ImagePlaneSet->Delete();
    }

  void ReadImageData (void)
    {
      const char* dataRoot;

      if ((dataRoot = getenv("VTK_DATA_ROOT")) == NULL)
        {
        vtkGenericWarningMacro(<< "Please set VTK_DATA_ROOT environment"
                               << " variable.");
        return;
        }

      const char* infix  = "/Data/headsq/quarter";
      char*       prefix =
        strcpy(new char [strlen(dataRoot)+strlen(infix)+1], dataRoot);

      strcat(prefix, infix);

      vtkImageAnisotropicDiffusion3D* anisotropicDiffusion =
        static_cast<vtkImageAnisotropicDiffusion3D*>(this->ImageSource2);
        {
        vtkImageGaussianSmooth* gaussianSmooth =
          static_cast<vtkImageGaussianSmooth*>(this->ImageSource1);
          {
          vtkImageCast* imageCast = vtkImageCast::New();
            {
            double matrix[16] = { 1,  0,  0,  102.4,
                                  0,  0, -1,  209.25,
                                  0,  1,  0,  102.4,
                                  0,  0,  0,  1 };

            vtkTransform* transform = vtkTransform::New();

            transform->SetMatrix(matrix);

            this->ImageReader->SetFilePrefix(prefix);
            this->ImageReader->SetDataByteOrderToLittleEndian();
            this->ImageReader->SetDataExtent(0, 63, 0, 63, 1, 93);
            this->ImageReader->SetDataSpacing(3.2, 3.2, 1.5);
            this->ImageReader->SetDataMask(0x7fff);
            this->ImageReader->SetTransform(transform);
            transform->Delete();

            imageCast->SetInputConnection(this->ImageReader->GetOutputPort());
            imageCast->SetOutputScalarTypeToFloat();
            }
          gaussianSmooth->SetInputConnection(imageCast->GetOutputPort());
          gaussianSmooth->SetDimensionality(3);
          gaussianSmooth->SetRadiusFactors(1.0, 1.0, 1.5/3.2);
          imageCast->Delete();
          }
        anisotropicDiffusion->SetInputConnection(gaussianSmooth->GetOutputPort());
        anisotropicDiffusion->SetNumberOfIterations(4);
        anisotropicDiffusion->SetDiffusionThreshold(40.0);
        anisotropicDiffusion->SetDiffusionFactor(1.0);
        anisotropicDiffusion->GradientMagnitudeThresholdOn();
        }

      delete [] prefix;

      vtkFloatingPointType range[2];

      anisotropicDiffusion->Update();
      anisotropicDiffusion->GetOutput()->GetScalarRange(range);

      this->Min = range[0];
      this->Max = range[1];

      this->Window = this->Max - this->Min;

      return;
    }

  void  AddBoundingActor (vtkRenderer* aRenderer)
    {
      if (aRenderer == NULL)
        return;

        {
        vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
          {
          vtkOutlineCornerFilter* outline = vtkOutlineCornerFilter::New();
            {
            outline->SetInputConnection(this->ImageSource2->GetOutputPort());
            outline->SetCornerFactor(0.15);
            }
          polyDataMapper->SetInputConnection(outline->GetOutputPort());
          polyDataMapper->ScalarVisibilityOff();
          outline->Delete();
          }
        this->BoundingActor->SetMapper(polyDataMapper);
        polyDataMapper->Delete();
        }
      this->BoundingActor->GetProperty()->SetColor(0.9, 0.9, 0.6);
      this->BoundingActor->GetProperty()->SetSpecular(0.5);
      this->BoundingActor->GetProperty()->SetSpecularPower(50);

      aRenderer->AddActor(this->BoundingActor);
    }

  void  AddContourActor (vtkRenderer* aRenderer)
    {
      if (aRenderer == NULL)
        return;

        {
        vtkPolyDataMapper* polyDataMapper = vtkPolyDataMapper::New();
          {
          vtkPolyDataNormals* polyDataNormals = vtkPolyDataNormals::New();
            {
              {
                {
                vtkImageToStructuredPoints* source =
                  vtkImageToStructuredPoints::New();
                  {
                  source->SetInputConnection(this->ImageSource2->GetOutputPort());
                  }
                this->ContourFilter->SetInputConnection(source->GetOutputPort());
                this->ContourFilter->SetValue(0, this->Min + this->Window*0.2);
                // update the gui
                this->ContourFilter->ComputeScalarsOn();
                this->ContourFilter->UseScalarTreeOn();
                source->Delete();
                }
              this->ConnectivityFilter->
                SetInputConnection(this->ContourFilter->GetOutputPort());
              this->ConnectivityFilter->SetExtractionModeToAllRegions();
              this->ConnectivityFilter->ColorRegionsOn();
              }
            polyDataNormals->SetInputConnection(this->ConnectivityFilter->GetOutputPort());
            polyDataNormals->SplittingOff();
            polyDataNormals->ConsistencyOn();
            polyDataNormals->NonManifoldTraversalOff();
            }
          polyDataMapper->SetInputConnection(polyDataNormals->GetOutputPort());
          polyDataMapper->UseLookupTableScalarRangeOff();
          polyDataMapper->SetColorModeToMapScalars();
          polyDataMapper->SetLookupTable(this->ImagePlaneSet->GetColorTF());
//        polyDataMapper->ImmediateModeRenderingOn();
          polyDataMapper->ScalarVisibilityOff();
          polyDataNormals->Delete();
          }
        this->ContourActor->SetMapper(polyDataMapper);
        polyDataMapper->Delete();
        }

      /* "polished gold" */
      this->ContourActor->GetProperty()->SetAmbient(1.0);
      this->ContourActor->GetProperty()->
        SetAmbientColor(0.247250, 0.224500, 0.064500);

      this->ContourActor->GetProperty()->SetDiffuse(1.0);
      this->ContourActor->GetProperty()->
        SetDiffuseColor(0.346150, 0.314300, 0.090300);

      this->ContourActor->GetProperty()->SetSpecular(1.0);
      this->ContourActor->GetProperty()->
        SetSpecularColor(0.797357, 0.723991, 0.208006);

      this->ContourActor->GetProperty()->SetSpecularPower(83.199997);

      aRenderer->AddActor(this->ContourActor);
    }

  void  AddImagePlaneSet (vtkRenderWindowInteractor* aInteractor)
    {
      if (aInteractor == NULL)
        return;

      this->ImagePlaneSet->SetInput(this->ImageSource2->GetOutput());
      this->ImagePlaneSet->SetInteractor(aInteractor);
    }

private:
  ContourBaseInternal (const ContourBaseInternal&); // Not Implemented
  void operator= (const ContourBaseInternal&); // Not Implemented
};


// ----------------------------------------------------------------------------
//      C o n t o u r B a s e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (ContourBase, "$Revision: 1.5 $");

// ----------------------------------------------------------------------------
ContourBase::ContourBase (void)
  : Internal(new ContourBaseInternal),
    FltkWindow(0),
    VtkWindow(0)
{
}

ContourBase::~ContourBase()
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
ContourBase::SetFltkWindow (Fl_Window* aWindow)
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
ContourBase::GetFltkWindow (void) const
{
  return this->FltkWindow;
}

// ----------------------------------------------------------------------------
void
ContourBase::SetVtkWindow (Fl_VTK_Window* aWindow)
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
    // Let's get pretty ...
    this->VtkWindow->GetInteractor()->SetDesiredUpdateRate(2.0);
    this->VtkWindow->GetRenderWindow()->SetDesiredUpdateRate(2.0);
    this->VtkWindow->GetRenderWindow()->LineSmoothingOn();
    }
}

Fl_VTK_Window*
ContourBase::GetVtkWindow (void) const
{
  return this->VtkWindow;
}

// ----------------------------------------------------------------------------
void
ContourBase::Initialize (void)
{
  if (!this->Internal->Initialized && this->VtkWindow != NULL)
    {
    vtkRenderer* renderer = this->VtkWindow->GetDefaultRenderer();

    renderer->SetBackground(0.2, 0.4, 0.6);
    this->Internal->ReadImageData();
    this->Internal->AddBoundingActor(renderer);
    /* Position the camera. */
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Dolly(1.2);
    renderer->GetActiveCamera()->Azimuth(15);
    renderer->GetActiveCamera()->Elevation(10);
    renderer->ResetCameraClippingRange();
    this->UpdateView();
    this->Internal->AddImagePlaneSet(this->VtkWindow->GetInteractor());
    this->UpdateView();
    this->Internal->AddContourActor(renderer);
    this->InitIsoInput();
    this->UpdateView();
    this->Internal->Initialized = true;
    }
}

void
ContourBase::Show (int aArgc, char* aArgv[])
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show(aArgc, aArgv);
    this->Initialize();
    }
}

void
ContourBase::Show (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show();
    this->Initialize();
    }
}

int
ContourBase::Shown (void) const
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    return this->FltkWindow->shown();
    }

  return 0;
}

void
ContourBase::Hide (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    }
}

void
ContourBase::UpdateView (void)
{
  if (this->VtkWindow != NULL)
    {
    this->VtkWindow->Update();
    this->Check();
    }
}

vtkFloatingPointType
ContourBase::GetMin (void) const
{
  return this->Internal->Min;
}

vtkFloatingPointType
ContourBase::GetMax (void) const
{
  return this->Internal->Max;
}

vtkFloatingPointType
ContourBase::GetWindow (void) const
{
  return this->Internal->Window;
}

void
ContourBase::SetContourValue (vtkFloatingPointType a)
{
  this->Internal->ContourFilter->SetValue(0, a);
}

vtkFloatingPointType
ContourBase::GetContourValue (void)
{
  return this->Internal->ContourFilter->GetValue(0);
}

void
ContourBase::SetContourVisibility (int a)
{
  this->Internal->ContourActor->SetVisibility(a);
}

int
ContourBase::GetContourVisibility (void)
{
  return this->Internal->ContourActor->GetVisibility();
}

void
ContourBase::SetImageReaderProgressMethod (vtkCommand* a)
{
  this->Internal->ImageReader->AddObserver(vtkCommand::ProgressEvent, a);
}

void
ContourBase::SetImageSourceProgressMethod (vtkCommand* a)
{
  this->Internal->ImageSource1->AddObserver(vtkCommand::ProgressEvent, a);
  this->Internal->ImageSource2->AddObserver(vtkCommand::ProgressEvent, a);
}

void
ContourBase::SetContourFilterProgressMethod (vtkCommand* a)
{
  this->Internal->ContourFilter->AddObserver(vtkCommand::ProgressEvent, a);
}

void
ContourBase::SetConnectivityFilterProgressMethod (vtkCommand* a)
{
  this->Internal->ConnectivityFilter->AddObserver(vtkCommand::ProgressEvent, a);
}

// ----------------------------------------------------------------------------
void
ContourBase::WriteImage (const char* const& aFileName)
{
  if (this->VtkWindow != NULL)
    {
    vtkPNGWriter* pngWriter = vtkPNGWriter::New();
      {
      vtkRendererSource* rendererSource = vtkRendererSource::New();
        {
        rendererSource->SetInput(this->VtkWindow->GetDefaultRenderer());
        rendererSource->WholeWindowOn();
        rendererSource->RenderFlagOn();
        rendererSource->DepthValuesOff();
        rendererSource->DepthValuesInScalarsOff();
        }
      pngWriter->SetInputConnection(rendererSource->GetOutputPort());
      pngWriter->SetFileName(aFileName);
      pngWriter->Write();
      rendererSource->Delete();
      }
    pngWriter->Delete();
    }
}

/*
 * End of: $Id: ContourBase.cxx,v 1.5 2004/06/16 01:42:49 xpxqx Exp $.
 *
 */
