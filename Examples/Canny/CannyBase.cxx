/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: CannyBase.cxx,v 1.5 2004/05/27 00:14:49 xpxqx Exp $
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
#include "CannyBase.h"
#include "vtkAbstractProgressCommand.h"
// VTK Common
#include <FL/Fl.H>
// VTK Common
#include "vtkStructuredPoints.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCoordinate.h"
// VTK Filtering
#include "vtkImageToStructuredPoints.h"
// VTK Imaging
#include "vtkImageLuminance.h"
#include "vtkImageCast.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageGradient.h"
#include "vtkImageMagnitude.h"
#include "vtkImageNonMaximumSuppression.h"
#include "vtkImageConstantPad.h"
// VTK Graphics
#include "vtkLinkEdgels.h"
#include "vtkThreshold.h"
#include "vtkGeometryFilter.h"
#include "vtkSubPixelPositionEdgels.h"
#include "vtkStripper.h"
// VTK IO
#include "vtkImageReader.h"
#include "vtkImageReader2Factory.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkPNMWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkPolyDataWriter.h"
#include "vtkXMLPolyDataWriter.h"
// VTK Rendering
#include "vtkWindowToImageFilter.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkImageMapper.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRendererSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
// vtkFLTK
#include "Fl_VTK_Window.H"
// C++ forwarding ANSI C
#include <cctype>

// ----------------------------------------------------------------------------
//      C a n n y B a s e I n t e r n a l
// ----------------------------------------------------------------------------
class CannyBaseInternal
{
public:
  bool                                  Initialized;

  vtkImageReader2*                      ImageReader;

  vtkImageLuminance*                    ImageLuminance;
  vtkImageCast*                         ImageCast;
  vtkImageGaussianSmooth*               ImageGaussianSmooth;
  vtkImageGradient*                     ImageGradient;

  vtkImageMagnitude*                    ImageMagnitude;

  vtkImageConstantPad*                  ImageConstantPad;

  vtkImageNonMaximumSuppression*        ImageNonMaximumSuppression;
  vtkImageToStructuredPoints*           EdgelStructuredPointsFilter;
  vtkLinkEdgels*                        ImageLinkEdgels;
  vtkThreshold*                         ImageThresholdEdgels;
  vtkGeometryFilter*                    ImageGeometryFilter;

  vtkImageToStructuredPoints*           GradMapStructuredPointsFilter;

  vtkSubPixelPositionEdgels*            EdgelSubPixelPositioner;
  vtkStripper*                          EdgelStripper;

  vtkPolyDataMapper2D*                  EdgelMapper;
  vtkActor2D*                           EdgelActor;

  vtkImageMapper*                       ImageMapper;
  vtkActor2D*                           ImageActor;
  
  int                                   InputDimensions[3];
  vtkFloatingPointType                  InputRange[2];

  CannyBaseInternal (void)
    : Initialized(false),
      ImageReader(NULL),
      ImageLuminance(NULL),
      ImageCast(NULL),
      ImageGaussianSmooth(NULL),
      ImageGradient(NULL),
      ImageMagnitude(NULL),
      ImageConstantPad(NULL),
      ImageNonMaximumSuppression(NULL),
      EdgelStructuredPointsFilter(NULL),
      ImageLinkEdgels(NULL),
      ImageThresholdEdgels(NULL),
      ImageGeometryFilter(NULL),
      GradMapStructuredPointsFilter(NULL),
      EdgelSubPixelPositioner(NULL),
      EdgelStripper(NULL),
      EdgelMapper(NULL),
      EdgelActor(NULL),
      ImageMapper(NULL),
      ImageActor(NULL)
    {
      this->InputDimensions[0] = 0;
      this->InputDimensions[1] = 1;
      this->InputDimensions[2] = 2;
      this->InputRange[0] = 0;
      this->InputRange[1] = 0;
    }

  ~CannyBaseInternal()
    {
      if (this->ImageReader != NULL)
        this->ImageReader->Delete();
      if (this->EdgelActor != NULL)
        this->EdgelActor->Delete();
      if (this->ImageActor != NULL)
        this->ImageActor->Delete();
    }

  int   InitializeReader (const char* aFileName)
    {
      if (aFileName == NULL)
        return VTK_ERROR;

      if (this->ImageReader != NULL)
        {
        if (strcmp(this->ImageReader->GetFileName(), aFileName) == 0)
          {
          return VTK_OK;
          }
        this->ImageReader->Delete();
        this->ImageReader = NULL;
        }

      if ( ( this->ImageReader =
             vtkImageReader2Factory::CreateImageReader2(aFileName) ) == NULL )
        {
        this->InputDimensions[0] = 0;
        this->InputDimensions[1] = 0;
        this->InputDimensions[2] = 0;
        this->InputRange[0]      = 0;
        this->InputRange[1]      = 0;
        return VTK_ERROR;
        }

      this->ImageReader->SetFileName(aFileName);
      this->ImageReader->Update();
      this->ImageReader->GetOutput()->GetDimensions(this->InputDimensions);
      this->ImageReader->GetOutput()->GetScalarRange(this->InputRange);

      return VTK_OK;
    }

  void  DeletePipeline (vtkRenderer* aRenderer)
    {
      if (this->EdgelActor != NULL)
        {
        aRenderer->RemoveActor(this->EdgelActor);
        this->EdgelActor->Delete();
        this->EdgelActor = NULL;
        }
      if (this->ImageActor != NULL)
        {
        aRenderer->RemoveActor(this->ImageActor);
        this->ImageActor->Delete();
        this->ImageActor = NULL;
        }
    }

  int   InitializeActors (vtkRenderer* aRenderer)
    {
      if (aRenderer == NULL || this->ImageReader == NULL)
        return VTK_ERROR;
 
      int* size = aRenderer->GetSize();

      if (this->ImageActor == NULL)
        {
        this->ImageActor = vtkActor2D::New();
          {
          this->ImageMapper = vtkImageMapper::New();
            {
            this->ImageMapper->SetInputConnection(this->ImageReader->GetOutputPort());
            this->ImageMapper->SetColorWindow(255);
            this->ImageMapper->SetColorLevel(127);
            }
          this->ImageActor->SetMapper(this->ImageMapper);
          // decrement
          this->ImageMapper->Delete();
          }
        }

      aRenderer->AddActor(this->ImageActor);

      this->ImageActor->GetPositionCoordinate()->
        SetCoordinateSystemToViewport();
      this->ImageActor->GetPositionCoordinate()->SetValue(0, 0);

      this->ImageActor->GetPosition2Coordinate()->
        SetCoordinateSystemToViewport();
      this->ImageActor->GetPosition2Coordinate()->SetValue(size[0]-1,size[1]-1);

      if (this->EdgelActor == NULL)
        {
        this->EdgelActor = vtkActor2D::New();
          {
          this->EdgelMapper = vtkPolyDataMapper2D::New();
            {
            this->EdgelStripper = vtkStripper::New();
              {
              // subpixel them
              this->EdgelSubPixelPositioner = vtkSubPixelPositionEdgels::New();
                {
                // gradient the image
                this->ImageGradient = vtkImageGradient::New();
                  {
                  // smooth the image
                  this->ImageGaussianSmooth = vtkImageGaussianSmooth::New();
                    {
                    this->ImageCast = vtkImageCast::New();
                      {
                      if ( this->ImageReader->GetOutput()->
                           GetNumberOfScalarComponents() > 1 )
                        {
                        this->ImageLuminance = vtkImageLuminance::New();
                          {
                          this->ImageLuminance->
                            SetInputConnection(this->ImageReader->GetOutputPort());
                          this->ImageLuminance->ReleaseDataFlagOn();
                          }
                        this->ImageCast->
                          SetInputConnection(this->ImageLuminance->GetOutputPort());
                        // decrement
                        this->ImageLuminance->Delete();
                        }
                      else
                        {
                        this->ImageCast->
                          SetInputConnection(this->ImageReader->GetOutputPort());
                        // Be sure not to leave this pointer dangling.
                        this->ImageLuminance = NULL;
                        }
                      this->ImageCast->SetOutputScalarTypeToFloat();
                      this->ImageCast->ReleaseDataFlagOn();
                      }
                    this->ImageGaussianSmooth->
                      SetInputConnection(this->ImageCast->GetOutputPort());
                    this->ImageGaussianSmooth->SetDimensionality(2);
                    this->ImageGaussianSmooth->SetRadiusFactors(0.5, 0.5, 0);
                    this->ImageGaussianSmooth->ReleaseDataFlagOn();
                    // decrement
                    this->ImageCast->Delete();
                    }
                  this->ImageGradient->
                    SetInputConnection(this->ImageGaussianSmooth->GetOutputPort());
                  this->ImageGradient->SetDimensionality(2);
                  // decrement
                  this->ImageGaussianSmooth->Delete();
                  }
                this->ImageMagnitude = vtkImageMagnitude::New();
                  {
                  this->ImageMagnitude->
                    SetInputConnection(this->ImageGradient->GetOutputPort());
                  }
                this->ImageConstantPad = vtkImageConstantPad::New();
                  {
                  this->ImageConstantPad->
                    SetInputConnection(this->ImageGradient->GetOutputPort());
                  this->ImageConstantPad->
                    SetOutputNumberOfScalarComponents(3);
                  this->ImageConstantPad->SetConstant(0);
                  }
                this->ImageGeometryFilter = vtkGeometryFilter::New();
                  {
                  // threshold links
                  this->ImageThresholdEdgels = vtkThreshold::New();
                    {
                    // link edgles
                    this->ImageLinkEdgels = vtkLinkEdgels::New();
                      {
                      this->EdgelStructuredPointsFilter =
                        vtkImageToStructuredPoints::New();
                        {
                        // non maximum suppression
                        this->ImageNonMaximumSuppression =
                          vtkImageNonMaximumSuppression::New();
                          {
                          this->ImageNonMaximumSuppression->
                            SetVectorInputData(this->ImageGradient->GetOutput());
                          this->ImageNonMaximumSuppression->
                            SetMagnitudeInputData( this->ImageMagnitude->
                                               GetOutput() );
                          this->ImageNonMaximumSuppression->
                            SetDimensionality(2);
                          this->ImageNonMaximumSuppression->ReleaseDataFlagOn();
                          }
                        this->EdgelStructuredPointsFilter->
                          SetInputConnection( this->ImageNonMaximumSuppression->
                                    GetOutputPort() );
                        this->EdgelStructuredPointsFilter->
                          SetVectorInputData( this->ImageConstantPad->
                                          GetOutput() );
                        this->EdgelStructuredPointsFilter->ReleaseDataFlagOn();
                        // decrement
                        this->ImageNonMaximumSuppression->Delete();
                        }
                      this->ImageLinkEdgels->
                        SetInputConnection( this->EdgelStructuredPointsFilter->
                                  GetOutputPort() );
                      this->ImageLinkEdgels->SetGradientThreshold(2);
                      this->ImageLinkEdgels->ReleaseDataFlagOn();
                      // decrement
                      this->EdgelStructuredPointsFilter->Delete();
                      }
                    this->ImageThresholdEdgels->
                      SetInputConnection(this->ImageLinkEdgels->GetOutputPort());
                    this->ImageThresholdEdgels->ThresholdByUpper(10);
                    this->ImageThresholdEdgels->AllScalarsOff();
                    this->ImageThresholdEdgels->ReleaseDataFlagOn();
                    // decrement
                    this->ImageLinkEdgels->Delete();
                    }
                  this->ImageGeometryFilter->
                    SetInputConnection(this->ImageThresholdEdgels->GetOutputPort());
                  this->ImageGeometryFilter->ReleaseDataFlagOn();
                  // decrement
                  this->ImageThresholdEdgels->Delete();
                  }
                this->GradMapStructuredPointsFilter =
                  vtkImageToStructuredPoints::New();
                  {
                  this->GradMapStructuredPointsFilter->
                    SetInputConnection(this->ImageMagnitude->GetOutputPort());
                  this->GradMapStructuredPointsFilter->
                    SetVectorInputData(this->ImageConstantPad->GetOutput());
                  this->GradMapStructuredPointsFilter->ReleaseDataFlagOn();
                  }
                this->EdgelSubPixelPositioner->
                  SetInputConnection(this->ImageGeometryFilter->GetOutputPort());
                this->EdgelSubPixelPositioner->
                  SetGradMapsData(this->GradMapStructuredPointsFilter->GetStructuredPointsOutput());
                // decrement
                this->ImageGradient->Delete();
                this->ImageMagnitude->Delete();
                this->ImageConstantPad->Delete();
                this->ImageGeometryFilter->Delete();
                this->GradMapStructuredPointsFilter->Delete();
                }
              this->EdgelStripper->
                SetInputConnection(this->EdgelSubPixelPositioner->GetOutputPort());
              // decrement
              this->EdgelSubPixelPositioner->Delete();
              }
            this->EdgelMapper->SetInputConnection(this->EdgelStripper->GetOutputPort());
            this->EdgelMapper->ScalarVisibilityOff();
            // decrement
            this->EdgelStripper->Delete();
            }
          this->EdgelActor->SetMapper(this->EdgelMapper);
          // decrement
          this->EdgelMapper->Delete();
          }
        }

      aRenderer->AddActor(this->EdgelActor);

      this->EdgelActor->GetPositionCoordinate()->
        SetCoordinateSystemToViewport();
      this->EdgelActor->GetPositionCoordinate()->SetValue(0, 0);

      this->EdgelActor->GetPosition2Coordinate()->
        SetCoordinateSystemToViewport();
      this->EdgelActor->GetPosition2Coordinate()->SetValue(size[0]-1,size[1]-1);

      return VTK_OK;
    }

private:
  CannyBaseInternal (const CannyBaseInternal&); // Not Implemented
  void operator= (const CannyBaseInternal&); // Not Implemented
};


// ----------------------------------------------------------------------------
//      C o n t o u r B a s e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (CannyBase, "$Revision: 1.5 $");

// ----------------------------------------------------------------------------
CannyBase::CannyBase (void)
  : Internal(new CannyBaseInternal),
    FltkWindow(NULL),
    VtkWindow(NULL),
    ProgressCommand(NULL),
    LineWidth(1.0),
    SmoothingRadiusFactor(0.5),
    EdgelVisibility(1),
    ImageVisibility(1)
{
  this->BackgroundColor[0]=this->BackgroundColor[1]=this->BackgroundColor[2]=0;
  this->ForegroundColor[0]=this->ForegroundColor[1]=this->ForegroundColor[2]=1;
}

CannyBase::~CannyBase()
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    delete this->FltkWindow;
    }

  if (this->ProgressCommand != (vtkAbstractProgressCommand *) 0)
    {
    this->ProgressCommand->UnRegister(this);
    this->ProgressCommand = NULL;
    }

  delete this->Internal;
}

// ----------------------------------------------------------------------------
void
CannyBase::SetFltkWindow (Fl_Window* aWindow)
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
CannyBase::GetFltkWindow (void) const
{
  return this->FltkWindow;
}

// ----------------------------------------------------------------------------
void
CannyBase::SetVtkWindow (Fl_VTK_Window* aWindow)
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
CannyBase::GetVtkWindow (void) const
{
  return this->VtkWindow;
}

// ----------------------------------------------------------------------------
void
CannyBase::Show (int aArgc, char* aArgv[])
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    double bg[3] = {0, 0, 0};
    this->FltkWindow->show(aArgc, aArgv);
    this->SetBackgroundColor(bg);
    this->UpdateView();
    }
}

void
CannyBase::Show (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    double bg[3] = {0, 0, 0};
    this->FltkWindow->show();
    this->SetBackgroundColor(bg);
    this->UpdateView();
    }
}

int
CannyBase::Shown (void) const
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    return this->FltkWindow->shown();
    }

  return 0;
}

void
CannyBase::Hide (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    }
}

void
CannyBase::UpdateView (void)
{
  if (this->VtkWindow != NULL)
    {
    this->VtkWindow->Update();
    this->Check();
    }
}

// ----------------------------------------------------------------------------
double
CannyBase::GetLineWidth (void)
{
  return this->LineWidth;
}

void
CannyBase::SetLineWidth (double aWidth)
{
  if (this->LineWidth != aWidth)
    {
    this->LineWidth = aWidth;
    this->Modified();
    }

  vtkProperty2D* property;

  if ( this->Internal->EdgelActor != NULL &&
       (property = this->Internal->EdgelActor->GetProperty()) != NULL )
    {
    property->SetLineWidth(this->LineWidth);
    }
}

// ----------------------------------------------------------------------------
vtkFloatingPointType
CannyBase::GetSmoothingRadiusFactor (void)
{
  return this->SmoothingRadiusFactor;
}

void
CannyBase::SetSmoothingRadiusFactor (vtkFloatingPointType aFactor)
{
  if (this->SmoothingRadiusFactor != aFactor)
    {
    this->SmoothingRadiusFactor = aFactor;
    this->Modified();
    }

  if (this->Internal->ImageGaussianSmooth != NULL)
    {
    this->Internal->ImageGaussianSmooth->
      SetRadiusFactors( this->SmoothingRadiusFactor,
                        this->SmoothingRadiusFactor,
                        0 );
    }
}

// ----------------------------------------------------------------------------
void
CannyBase::GetBackgroundColor (double aColor[3])
{
  aColor[0] = this->BackgroundColor[0];
  aColor[1] = this->BackgroundColor[1];
  aColor[2] = this->BackgroundColor[2];
}

void
CannyBase::SetBackgroundColor (double aColor[3])
{
  if ( this->BackgroundColor[0] != aColor[0] ||
       this->BackgroundColor[1] != aColor[1] ||
       this->BackgroundColor[2] != aColor[2] )
    {
    this->BackgroundColor[0] = aColor[0];
    this->BackgroundColor[1] = aColor[1];
    this->BackgroundColor[2] = aColor[2];
    this->Modified();
    }

  vtkRenderer* renderer;

  if ( this->VtkWindow != NULL &&
       (renderer = this->VtkWindow->GetDefaultRenderer()) != NULL )
    {
    renderer->SetBackground( this->BackgroundColor[0],
                             this->BackgroundColor[1],
                             this->BackgroundColor[2] );
    }
}

void
CannyBase::GetForegroundColor (double aColor[3])
{
  aColor[0] = this->ForegroundColor[0];
  aColor[1] = this->ForegroundColor[1];
  aColor[2] = this->ForegroundColor[2];
}

void
CannyBase::SetForegroundColor (double aColor[3])
{
  if ( this->ForegroundColor[0] != aColor[0] ||
       this->ForegroundColor[1] != aColor[1] ||
       this->ForegroundColor[2] != aColor[2] )
    {
    this->ForegroundColor[0] = aColor[0];
    this->ForegroundColor[1] = aColor[1];
    this->ForegroundColor[2] = aColor[2];
    this->Modified();
    }

  vtkProperty2D* property;

  if ( this->Internal->EdgelActor != NULL &&
       (property = this->Internal->EdgelActor->GetProperty()) != NULL )
    {
    property->SetColor( this->ForegroundColor[0],
                        this->ForegroundColor[1],
                        this->ForegroundColor[2] );
    }
}

// ----------------------------------------------------------------------------
void
CannyBase::SetEdgelVisibility (int aToggle)
{
  if (this->EdgelVisibility != aToggle)
    {
    this->EdgelVisibility = aToggle;
    this->Modified();
    }

  if (this->Internal->EdgelActor != NULL)
    {
    this->Internal->EdgelActor->SetVisibility(this->EdgelVisibility);
    }
}

void
CannyBase::SetImageVisibility (int aToggle)
{
  if (this->ImageVisibility != aToggle)
    {
    this->ImageVisibility = aToggle;
    this->Modified();
    }

  if (this->Internal->ImageActor != NULL)
    {
    this->Internal->ImageActor->SetVisibility(this->ImageVisibility);
    }
}

/* Interface Layout and Proportions
 *
 *
 *          +--------------292--------------+
 *          |                               | 
 *        +4+                               +4+
 *        | |                               | |
 *        v v                               v v
 *
 *  +-->  +-----------------------------------+  <--+
 *  |     |                                   |     4
 *  |     | +-------------------------------+ |  <--+--+
 *  |     | | *                           * | |        |
 *  |     | |  *                         *  | |        |
 *  |     | |   *                       *   | |        |
 *  |     | |    *                     *    | |        |
 *  |     | |     *                   *     | |        |
 *  |     | |      *                 *      | |        |
 *  |     | |       *               *       | |        |
 *  |     | |        *             *        | |        2
 *  |     | |       R E S I Z A B L E       | |        4
 *  |     | |        *             *        | |        0
 *  |     | |       *               *       | |        |
 *  |     | |      *                 *      | |        |
 *  |     | |     *                   *     | |        |
 *  |     | |    *                     *    | |        |
 *  |     | |   *                       *   | |        |
 *  3     | |  *                         *  | |        |
 *  3     | | *                           * | |        |
 *  0     | +-------------------------------+ |  <--+--+
 *  |     |                                   |     4
 *  |     | +-------------------------------+ |  <--+--+
 *  |     | |                               | |        |
 *  |     | |                               | |        7
 *  |     | |                               | |        8
 *  |     | |                               | |        |
 *  |     | +-------------------------------+ |  <--+--+
 *  |     |                                   |     4
 *  +-->  +-----------------------------------+  <--+
 *
 *        ^                                   ^
 *        |                                   |
 *        +----------------300----------------+
 *
 */

// ----------------------------------------------------------------------------
int
CannyBase::ReadImage (const char* const& aFileName)
{
  if (this->VtkWindow == NULL)
    {
    return VTK_ERROR;
    }

  vtkRenderer* renderer = this->VtkWindow->GetDefaultRenderer();

  if (this->Internal->InitializeReader(aFileName) != VTK_OK)
    {
    this->VtkWindow->size(256, 256);
    return VTK_ERROR;
    }

  const int     addX = 8;
  const int     addY = 90;
  const int     minW = 292;
  const int     maxW = Fl::w() - addX;
  const int     maxH = Fl::h() - addY;

  int   W = this->Internal->InputDimensions[0];
  int   H = this->Internal->InputDimensions[1];

  if (W > maxW)
    {
    H = int(0.5 + H * double(maxW) / W);
    W = maxW;
    }

  if (H > maxH)
    {
    W = int(0.5 + W * double(maxH) / H);
    H = maxH;
    }

  if (W < 1) W = 1;
  if (H < 1) H = 1;

  int   winX = this->FltkWindow->x();
  int   winY = this->FltkWindow->y();

  if ((winX + W) > maxW) winX = maxW - W;
  if ((winY + H) > maxH) winY = maxH - H;

  this->Internal->DeletePipeline(renderer);

  this->VtkWindow->hide();
  this->FltkWindow->resize(winX, winY, (W<minW ? minW : W)+addX, H+addY);

  this->VtkWindow->resize((W<minW ? int(4.5+(minW-W)/2.0) : 4), 4, W, H);

  if (this->Internal->InitializeActors(renderer) != VTK_OK)
    {
    return VTK_ERROR;
    }
  else
    {
    // Copy state ivars.
    this->Internal->ImageGaussianSmooth->
      SetRadiusFactors( this->SmoothingRadiusFactor,
                        this->SmoothingRadiusFactor,
                        0 );

    renderer->SetBackground( this->BackgroundColor[0],
                             this->BackgroundColor[1],
                             this->BackgroundColor[2] );

    this->Internal->EdgelActor->GetProperty()->
      SetLineWidth(this->LineWidth);

    this->Internal->EdgelActor->GetProperty()->
      SetColor( this->ForegroundColor[0],
                this->ForegroundColor[1],
                this->ForegroundColor[2]);

    this->Internal->EdgelActor->SetVisibility(this->EdgelVisibility);
    this->Internal->ImageActor->SetVisibility(this->ImageVisibility);
    }

  this->SetProgressCommand(this->ProgressCommand);

  //  this->Internal->ImageMapper->GetInput()->Update();
  //  this->Internal->EdgelMapper->GetInput()->Update();

  this->VtkWindow->show();

  return VTK_OK;
}

// ----------------------------------------------------------------------------
int
CannyBase::WriteImage (const char* const& aFileName)
{
  if (this->VtkWindow == NULL || aFileName == NULL || aFileName[0] == '\0')
    {
    return VTK_ERROR;
    }

  char* ptr;

  if ((ptr = strrchr((char*)aFileName, '.')) == NULL)
    {
    //vtkWarningMacro("Could not determine output type from suffix.");
    return VTK_ERROR;
    }

  char* suffix = strcpy(new char [strlen(ptr+1) + 1], ptr+1);

  vtkImageWriter* imageWriter = NULL;

  ptr = suffix;
  while (*ptr != '\0') *ptr++ = toupper(*ptr);

  if      (strcmp(suffix, "BMP") == 0)
    {
    imageWriter = vtkBMPWriter::New();
    }
  else if ( strcmp(suffix, "JPG") == 0 ||
            strcmp(suffix, "JPEG") == 0 )
    {
    imageWriter = vtkJPEGWriter::New();
    }
  else if (strcmp(suffix, "PNG") == 0)
    {
    imageWriter = vtkPNGWriter::New();
    }
  else if ( strcmp(suffix, "PGM") == 0 ||
            strcmp(suffix, "PPM") == 0 )
    {
    imageWriter = vtkPNMWriter::New();
    }
  else if ( strcmp(suffix, "PS") == 0 ||
            strcmp(suffix, "EPS") == 0 )
    {
    imageWriter = vtkPostScriptWriter::New();
    }
  else if ( strcmp(suffix, "TIF") == 0 ||
            strcmp(suffix, "TIFF") == 0 )
    {
    imageWriter = vtkTIFFWriter::New();
    }
  else if (strcmp(suffix, "VTK") == 0)
    {
    vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
    writer->SetInputConnection(this->Internal->EdgelStripper->GetOutputPort());
    writer->SetFileName(aFileName);
    writer->Write();
    writer->Delete();
    return VTK_OK;
    }
  else if (strcmp(suffix, "VTP") == 0)
    {
    vtkXMLPolyDataWriter* writer = vtkXMLPolyDataWriter::New();
    writer->SetInputConnection(this->Internal->EdgelStripper->GetOutputPort());
    writer->SetFileName(aFileName);
    writer->Write();
    writer->Delete();
    return VTK_OK;
    }

  if (imageWriter != NULL)
    {
    vtkRendererSource* rendererSource = vtkRendererSource::New();
      {
      rendererSource->SetInput(this->VtkWindow->GetDefaultRenderer());
      rendererSource->WholeWindowOn();
      rendererSource->RenderFlagOn();
      rendererSource->DepthValuesOff();
      rendererSource->DepthValuesInScalarsOff();
      }
    imageWriter->SetInputConnection(rendererSource->GetOutputPort());
    imageWriter->SetFileName(aFileName);
    imageWriter->Write();
    rendererSource->Delete();
    imageWriter->Delete();
    return VTK_OK;
    }    

  return VTK_ERROR;
}

// ----------------------------------------------------------------------------
void
CannyBase::SetProgressCommand (vtkAbstractProgressCommand* aCommand)
{
  if (this->ProgressCommand != NULL && this->Internal->EdgelActor != NULL)
    {
    if (this->Internal->ImageReader != NULL)
      this->Internal->ImageReader->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageLuminance != NULL)
      this->Internal->ImageLuminance->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageCast != NULL)
      this->Internal->ImageCast->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageGaussianSmooth != NULL)
      this->Internal->ImageGaussianSmooth->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageGradient != NULL)
      this->Internal->ImageGradient->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageMagnitude != NULL)
      this->Internal->ImageMagnitude->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageConstantPad != NULL)
      this->Internal->ImageConstantPad->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageNonMaximumSuppression != NULL)
      this->Internal->ImageNonMaximumSuppression->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->EdgelStructuredPointsFilter != NULL)
      this->Internal->EdgelStructuredPointsFilter->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageLinkEdgels != NULL)
      this->Internal->ImageLinkEdgels->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageThresholdEdgels != NULL)
      this->Internal->ImageThresholdEdgels->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageGeometryFilter != NULL)
      this->Internal->ImageGeometryFilter->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->GradMapStructuredPointsFilter != NULL)
      this->Internal->GradMapStructuredPointsFilter->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->EdgelSubPixelPositioner != NULL)
      this->Internal->EdgelSubPixelPositioner->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->EdgelStripper != NULL)
      this->Internal->EdgelStripper->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->EdgelMapper != NULL)
      this->Internal->EdgelMapper->
        RemoveObserver(this->ProgressCommand);
    if (this->Internal->ImageMapper != NULL)
      this->Internal->ImageMapper->
        RemoveObserver(this->ProgressCommand);
    }

  vtkSetObjectBodyMacro(ProgressCommand, vtkAbstractProgressCommand, aCommand);

  if (this->ProgressCommand != NULL && this->Internal->EdgelActor != NULL)
    {
    if (this->Internal->ImageReader != NULL)
      this->Internal->ImageReader->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageLuminance != NULL)
      this->Internal->ImageLuminance->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageCast != NULL)
      this->Internal->ImageCast->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageGaussianSmooth != NULL)
      this->Internal->ImageGaussianSmooth->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageGradient != NULL)
      this->Internal->ImageGradient->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageMagnitude != NULL)
      this->Internal->ImageMagnitude->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageConstantPad != NULL)
      this->Internal->ImageConstantPad->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageNonMaximumSuppression != NULL)
      this->Internal->ImageNonMaximumSuppression->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->EdgelStructuredPointsFilter != NULL)
      this->Internal->EdgelStructuredPointsFilter->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageLinkEdgels != NULL)
      this->Internal->ImageLinkEdgels->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageThresholdEdgels != NULL)
      this->Internal->ImageThresholdEdgels->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageGeometryFilter != NULL)
      this->Internal->ImageGeometryFilter->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->GradMapStructuredPointsFilter != NULL)
      this->Internal->GradMapStructuredPointsFilter->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->EdgelSubPixelPositioner != NULL)
      this->Internal->EdgelSubPixelPositioner->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->EdgelStripper != NULL)
      this->Internal->EdgelStripper->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->EdgelMapper != NULL)
      this->Internal->EdgelMapper->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    if (this->Internal->ImageMapper != NULL)
      this->Internal->ImageMapper->
        AddObserver(vtkCommand::ProgressEvent, this->ProgressCommand);
    }
}

// ----------------------------------------------------------------------------
void
CannyBase::AbortRendering (void)
{
  if (this->VtkWindow != (Fl_VTK_Window *) 0)
    {
    this->VtkWindow->GetRenderWindow()->SetAbortRender(1);
    }

  if (this->Internal->ImageReader != NULL)
    this->Internal->ImageReader->SetAbortExecute(1);
  if (this->Internal->ImageLuminance != NULL)
    this->Internal->ImageLuminance->SetAbortExecute(1);
  if (this->Internal->ImageCast != NULL)
    this->Internal->ImageCast->SetAbortExecute(1);
  if (this->Internal->ImageGaussianSmooth != NULL)
    this->Internal->ImageGaussianSmooth->SetAbortExecute(1);
  if (this->Internal->ImageGradient != NULL)
    this->Internal->ImageGradient->SetAbortExecute(1);
  if (this->Internal->ImageMagnitude != NULL)
    this->Internal->ImageMagnitude->SetAbortExecute(1);
  if (this->Internal->ImageConstantPad != NULL)
    this->Internal->ImageConstantPad->SetAbortExecute(1);
  if (this->Internal->ImageNonMaximumSuppression != NULL)
    this->Internal->ImageNonMaximumSuppression->SetAbortExecute(1);
  if (this->Internal->EdgelStructuredPointsFilter != NULL)
    this->Internal->EdgelStructuredPointsFilter->SetAbortExecute(1);
  if (this->Internal->ImageLinkEdgels != NULL)
    this->Internal->ImageLinkEdgels->SetAbortExecute(1);
  if (this->Internal->ImageThresholdEdgels != NULL)
    this->Internal->ImageThresholdEdgels->SetAbortExecute(1);
  if (this->Internal->ImageGeometryFilter != NULL)
    this->Internal->ImageGeometryFilter->SetAbortExecute(1);
  if (this->Internal->GradMapStructuredPointsFilter != NULL)
    this->Internal->GradMapStructuredPointsFilter->SetAbortExecute(1);
  if (this->Internal->EdgelSubPixelPositioner != NULL)
    this->Internal->EdgelSubPixelPositioner->SetAbortExecute(1);
  if (this->Internal->EdgelStripper != NULL)
    this->Internal->EdgelStripper->SetAbortExecute(1);
  if (this->Internal->EdgelMapper != NULL)
    this->Internal->EdgelMapper->SetAbortExecute(1);
  if (this->Internal->ImageMapper != NULL)
    this->Internal->ImageMapper->SetAbortExecute(1);
}

int
CannyBase::IsRendering (void)
{
  if (this->VtkWindow != (Fl_VTK_Window *) 0)
    {
    return this->VtkWindow->GetRenderWindow()->CheckInRenderStatus();
    }

  return 0;
}

/*
 * End of: $Id: CannyBase.cxx,v 1.5 2004/05/27 00:14:49 xpxqx Exp $.
 *
 */
