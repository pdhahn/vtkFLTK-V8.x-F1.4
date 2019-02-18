/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Contour.cxx,v 1.14 2004/05/13 18:37:49 xpxqx Exp $
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
#include "ContourUI.h"
// FLTK
#include <FL/Fl_File_Chooser.H>
// VTK Common
#include "vtkCommand.h"
// VTK Rendering
#include "vtkProp3D.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
// vtkFLTK
#include "Fl_VTK_Window.H"

#include "vtkPNMWriter.h"
#include "vtkRendererSource.h"

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkContourFilter.h"

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


// ----------------------------------------------------------------------------
VTK_FLTK_IMPLEMENT(void)
ContourToggleCb (Fl_Check_Button* aButton, void* aPtr)
{
  ContourUI* gui;

  if (aButton == NULL)
    return;
  if((gui = reinterpret_cast<ContourUI*>(aPtr)) == NULL)
    return;

  gui->SetContourVisibility(aButton->value());
  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
IsoInputCb (Fl_Value_Input* input, void* aPtr)
{
  ContourUI* gui;

  if (input == NULL)
    return;
  if((gui = reinterpret_cast<ContourUI*>(aPtr)) == NULL)
    return;

  gui->SetContourValue(float(input->value()));
  gui->UpdateView();
}

// ----------------------------------------------------------------------------
VTK_FLTK_IMPLEMENT(void)
SaveImageCb (Fl_Button*, void* aPtr)
{
  ContourUI* gui;

  if((gui = reinterpret_cast<ContourUI*>(aPtr)) == NULL)
    return;

  char* fileName =
    fl_file_chooser("Save PNG Image", "PNG files (*.png)", "Volume.png");

  while (gui->GetFltkWindow()->current() != gui->GetFltkWindow()) Fl::wait();
  gui->UpdateView();

  if (fileName != NULL) gui->WriteImage(fileName);
}

VTK_FLTK_IMPLEMENT(void)
SaveSeriesCb (Fl_Button*, void* aPtr)
{
  ContourUI*     gui;
  Fl_VTK_Window* view;

  if((gui = reinterpret_cast<ContourUI*>(aPtr)) == NULL)
    return;
  if((view = gui->GetView()) == NULL)
    return;

  double      window    = gui->GetWindow();
  int         firstStep = int(floor(gui->GetMin() + window*0.025));
  int         lastStep  = int(ceil(gui->GetMax() - window*0.125));
  int         incr      = int(floor(double(lastStep - firstStep) / 256.0));
  const char* baseName = "Contour_";
  char        fileName[1024];

  vtkPNMWriter* pnmWriter = vtkPNMWriter::New();

  vtkRendererSource* rendererSource = vtkRendererSource::New();
    {
    rendererSource->SetInput(view->GetDefaultRenderer());
    rendererSource->WholeWindowOn();
    rendererSource->RenderFlagOn();
    rendererSource->DepthValuesOff();
    rendererSource->DepthValuesInScalarsOff();
    }

  pnmWriter->SetInputConnection(rendererSource->GetOutputPort());

  int digits = int(floor(log(double(lastStep))/log(10.0))) + 1;
  if (digits<4) digits = 4;

  if ((strlen(baseName) + digits + strlen(".ppm")) < (sizeof fileName))
    {
    for (int step=firstStep, i=0; step<=lastStep; step+=incr, i++)
      {
      Fl::check();
      gui->SetContourValue(float(step));
      gui->IsoInput->value(double(step));
      gui->UpdateView();

      (void) sprintf(fileName, "%s%0*d.ppm", baseName, digits, i);
      pnmWriter->SetFileName(fileName);
      pnmWriter->Write();
      }
    }

  rendererSource->Delete();
  pnmWriter->Delete();
}

// ----------------------------------------------------------------------------
class vtkProgressCommand : public vtkCommand
{
public:
  void SetProgressWidget (Fl_Progress* a) { this->ProgressWidget = a; }

protected:
  vtkProgressCommand (void) : ProgressWidget(NULL) { this->Buffer[0] = '\0'; }

  Fl_Progress*  ProgressWidget;
  char          Buffer[128];

private:
  vtkProgressCommand (const vtkProgressCommand&);
  void operator= (const vtkProgressCommand&);
};

class vtkImageSourceProgress : public vtkProgressCommand
{
public:
  static vtkImageSourceProgress* New (void)
    { return new vtkImageSourceProgress; }

  void Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      vtkImageAlgorithm* filter;

      if ((filter = vtkImageAlgorithm::SafeDownCast(aCaller)) == NULL)
        return;
      if (this->ProgressWidget == NULL)
        return;

      float             progress  = filter->GetProgress();
      const char*       className = filter->GetClassName();
      int               length    = strlen(className) + 3 + 3 + 5 + 1;

      (void) sprintf( this->Buffer, "%s: %%%3d done",
                      (length<128 ? className : "filter"), int(progress*100) );

      this->ProgressWidget->label(this->Buffer);
      this->ProgressWidget->value(progress);

      if (this->ProgressWidget->window()->shown()) Fl::check();
    }

protected:
  vtkImageSourceProgress (void) {}

private:
  vtkImageSourceProgress (const vtkImageSourceProgress&);
  void operator= (const vtkImageSourceProgress&);
};

class vtkConnectivityProgress : public vtkProgressCommand
{
public:
  static vtkConnectivityProgress* New (void)
    { return new vtkConnectivityProgress; }

  void Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      vtkPolyDataConnectivityFilter* filter;

      if ((filter = vtkPolyDataConnectivityFilter::SafeDownCast(aCaller))==NULL)
        return;
      if (this->ProgressWidget == NULL)
        return;

      float progress;

      if ((progress = filter->GetProgress()) < 1.f)
        {
        const char*     className = filter->GetClassName();
        int             length    = strlen(className) + 3 + 3 + 5 + 1;

        (void) sprintf( this->Buffer, "%s: %%%3d done",
                        (length<128 ? className : "filter"), int(progress*100));
        }
      else
        {
        (void) sprintf( this->Buffer, "%d connectivity regions",
                        filter->GetNumberOfExtractedRegions() );
        }

      this->ProgressWidget->label(this->Buffer);
      this->ProgressWidget->value(progress);

      if (this->ProgressWidget->window()->shown()) Fl::check();
    }

protected:
  vtkConnectivityProgress (void) {}

private:
  vtkConnectivityProgress (const vtkConnectivityProgress&);
  void operator= (const vtkConnectivityProgress&);
};

class vtkContourProgress : public vtkProgressCommand
{
public:
  static vtkContourProgress* New (void)
    { return new vtkContourProgress; }

  void Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      vtkContourFilter* filter;

      if ((filter = vtkContourFilter::SafeDownCast(aCaller)) == NULL)
        return;
      if (this->ProgressWidget == NULL)
        return;

      float progress;

      if ((progress = filter->GetProgress()) < 1.f)
        {
        const char*     className = filter->GetClassName();
        int             length    = strlen(className) + 3 + 3 + 5 + 1;

        (void) sprintf( this->Buffer, "%s: %%%3d done",
                        (length<128 ? className : "filter"), int(progress*100));
        }
      else
        {
        (void) sprintf( this->Buffer, "contour isovalue: %g",
                        filter->GetValue(0) );
        }

      this->ProgressWidget->label(this->Buffer);
      this->ProgressWidget->value(progress);

      if (this->ProgressWidget->window()->shown()) Fl::check();
    }

protected:
  vtkContourProgress (void) {}

private:
  vtkContourProgress (const vtkContourProgress&);
  void operator= (const vtkContourProgress&);
};


// ----------------------------------------------------------------------------
int
main (int argc, char* argv[])
{
  // create the interface
  ContourUI* gui = ContourUI::New();

  vtkImageSourceProgress*  imageSourceProgress = vtkImageSourceProgress::New();
  vtkConnectivityProgress* connectivityProgress= vtkConnectivityProgress::New();
  vtkContourProgress*      contourProgress     = vtkContourProgress::New();

  imageSourceProgress->SetProgressWidget(gui->Progress);
  connectivityProgress->SetProgressWidget(gui->Progress);
  contourProgress->SetProgressWidget(gui->Progress);

  gui->SetImageReaderProgressMethod(imageSourceProgress);
  gui->SetImageSourceProgressMethod(imageSourceProgress);
  gui->SetContourFilterProgressMethod(contourProgress);
  gui->SetConnectivityFilterProgressMethod(connectivityProgress);

  imageSourceProgress->Delete();
  connectivityProgress->Delete();
  contourProgress->Delete();

  gui->Show(argc, argv);

  // this is the standard way of "starting" a fltk application
  int fl_ret = Fl::run();

  gui->Delete();

  return fl_ret;
}

/* 
 * End of: $Id: Contour.cxx,v 1.14 2004/05/13 18:37:49 xpxqx Exp $.
 *  
 */
