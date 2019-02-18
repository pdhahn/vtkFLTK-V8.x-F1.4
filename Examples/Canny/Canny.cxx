/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Canny.cxx,v 1.4 2004/05/27 00:14:49 xpxqx Exp $
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
#include "CannyUI.h"
// FLTK
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_ask.H>
// VTK Common
#include "vtkAbstractProgressCommand.h"
#include "vtkRenderWindow.h"
// vtkFLTK
#include "Fl_VTK_Window.H"

//
// This causes VTK to use the vtkFLTK library bridge code that
// supports Fl_VTK_Window.
//
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkFLTKOpenGLRendering);


// ----------------------------------------------------------------------------
VTK_FLTK_IMPLEMENT(void)
CannyLineWidthCb (Fl_Value_Input* aInput, void* aPtr)
{
  CannyUI* gui;

  if (aInput == NULL)
    return;
  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  gui->SetLineWidth(float(aInput->value()));

  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
CannySmoothingRadiusFactorCb (Fl_Value_Input* aInput, void* aPtr)
{
  CannyUI* gui;

  if (aInput == NULL)
    return;
  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  if (gui->IsRendering())
    {
    aInput->value(gui->GetSmoothingRadiusFactor());
    gui->Check();
    }
  else
    {
    gui->SetSmoothingRadiusFactor(vtkFloatingPointType(aInput->value()));
    gui->UpdateView();
    }
}

VTK_FLTK_IMPLEMENT(void)
CannyBackgroundCb (Fl_Button*, void* aPtr)
{
  CannyUI* gui;

  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  double bg[3];

  gui->GetBackgroundColor(bg);

  if (fl_color_chooser("Background Color", bg[0], bg[1], bg[2]) == 0)
    return; // canceled

  gui->SetBackgroundColor(bg);

  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
CannyForegroundCb (Fl_Button*, void* aPtr)
{
  CannyUI* gui;

  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  double fg[3];

  gui->GetForegroundColor(fg);

  if (fl_color_chooser("Foreground Color", fg[0], fg[1], fg[2]) == 0)
    return; // canceled

  gui->SetForegroundColor(fg);

  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
CannyEdgelVisibilityCb (Fl_Check_Button* aButton, void* aPtr)
{
  CannyUI* gui;

  if (aButton == NULL)
    return;
  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  if (gui->IsRendering())
    {
    aButton->value(!aButton->value());
    gui->Check();
    return;
    }

  gui->SetEdgelVisibility(aButton->value());

  gui->UpdateView();
}

VTK_FLTK_IMPLEMENT(void)
CannyImageVisibilityCb (Fl_Check_Button* aButton, void* aPtr)
{
  CannyUI* gui;

  if (aButton == NULL)
    return;
  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;

  if (gui->IsRendering())
    {
    aButton->value(!aButton->value());
    gui->Check();
    return;
    }

  gui->SetImageVisibility(aButton->value());

  gui->UpdateView();
}

// ----------------------------------------------------------------------------
VTK_FLTK_IMPLEMENT(void)
CannyLoadCb (Fl_Button*, void* aPtr)
{
  CannyUI* gui;

  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;
  if (gui->IsRendering())
    return;

  char* fileName =
    fl_file_chooser( "Read Image",
                     "(*.{bmp,dcm,jpg,jpeg,png,pgm,ppm,slc,tif,tiff})",
                     NULL );

  if (fileName == NULL)
    return;

  while ( gui->GetFltkWindow()->current() != gui->GetFltkWindow() &&
          gui->GetFltkWindow()->current() != gui->GetVtkWindow() ) Fl::wait();

  gui->UpdateView();

  if (gui->ReadImage(fileName) != VTK_OK)
    {
    fl_alert("Failed to read image.");
    }
  else
    {
    const char* ptr = strrchr(fileName, '/');
    gui->GetFltkWindow()->label(ptr==NULL ? fileName : ptr+1);
    }
}

VTK_FLTK_IMPLEMENT(void)
CannySaveCb (Fl_Button*, void* aPtr)
{
  CannyUI* gui;

  if ((gui = reinterpret_cast<CannyUI*>(aPtr)) == NULL)
    return;
  if (gui->IsRendering())
    return;

  char* fileName =
    fl_file_chooser( "Save As",
                     "(*.{bmp,jpg,jpeg,png,pgm,ppm,ps,eps,tif,tiff,vtk,vtp})",
                     NULL );

  if (fileName == NULL)
    return;

  while ( gui->GetFltkWindow()->current() != gui->GetFltkWindow() &&
          gui->GetFltkWindow()->current() != gui->GetVtkWindow() ) Fl::wait();
  gui->UpdateView();

  if (gui->WriteImage(fileName) != VTK_OK)
    {
    fl_alert("Failed to save data.");
    }
}

// ----------------------------------------------------------------------------
class vtkProgressCommand : public vtkAbstractProgressCommand
{
public:
  static vtkProgressCommand* New (void) { return new vtkProgressCommand; }

  void  SetWidget (Fl_Progress* a) { this->Widget = a; }

  void  Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      if (this->Widget != NULL)
        {
        this->Update(aCaller);

        this->Widget->label(this->Buffer);
        this->Widget->value(this->Progress);

        if (this->Widget->window()->shown()) Fl::check();
        }
    }

protected:
  vtkProgressCommand (void) : Widget(NULL) {}

  Fl_Progress*  Widget;

private:
  vtkProgressCommand (const vtkProgressCommand&);
  void operator= (const vtkProgressCommand&);
};

class vtkRendererCommand : public vtkCommand
{
public:
  static vtkRendererCommand* New (void) { return new vtkRendererCommand; }

  void  SetWidget (Fl_Progress* a) { this->Widget = a; }

  void  Execute (vtkObject* aCaller, unsigned long aEID, void* aCallData)
    {
      if (this->Widget != NULL)
        {
        vtkRenderWindow* renderWindow;

        if ((renderWindow = vtkRenderWindow::SafeDownCast(aCaller)) == NULL)
          {
          this->Buffer[0] = '\0';
          return;
          }

        const char* className = renderWindow->GetClassName();
        int         length    = strlen(className) + 14;
        int*        size      = renderWindow->GetSize();

        (void) sprintf( this->Buffer, "%s: %4d x %4d",
                        (length<128 ? className : "RenderWindow"),
                        size[0], size[1] );

        this->Widget->label(this->Buffer);
        this->Widget->value(1);

        if (this->Widget->window()->shown()) Fl::flush();
        }
    }

protected:
  vtkRendererCommand (void) : Widget(NULL) { this->Buffer[0] = '\0'; }

  Fl_Progress*  Widget;
  char Buffer[128];

private:
  vtkRendererCommand (const vtkRendererCommand&);
  void operator= (const vtkRendererCommand&);
};

// ----------------------------------------------------------------------------
int
main (int argc, char* argv[])
{
  // create the interface
  CannyUI*              gui = CannyUI::New();
  vtkProgressCommand*   progressCommand = vtkProgressCommand::New();
  vtkRendererCommand*   rendererCommand = vtkRendererCommand::New();

  progressCommand->SetWidget(gui->Progress);
  rendererCommand->SetWidget(gui->Progress);

  gui->SetProgressCommand(progressCommand);
  gui->GetVtkWindow()->GetRenderWindow()->
    AddObserver(vtkCommand::ModifiedEvent, rendererCommand);

  progressCommand->Delete();
  rendererCommand->Delete();

  // Registers the extra image file formats that are not provided as part 
  // of the core FLTK library for use with the Fl_Shared_Image class.
  fl_register_images();

  gui->Show(argc, argv);

  // this is the standard way of "starting" a fltk application
  int fl_ret = Fl::run();

  gui->Delete();

  return fl_ret;
}

/* 
 * End of: $Id: Canny.cxx,v 1.4 2004/05/27 00:14:49 xpxqx Exp $.
 *  
 */
