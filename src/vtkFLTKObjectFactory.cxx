/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkFLTKObjectFactory.cxx,v 1.9 2004/06/16 18:07:53 xpxqx Exp $
 *
 * Copyright (c) 2002 - 2004 Sean McInerney
 * All rights reserved.
 *
 * Copyright (c) 2019 Paul Douglas Hahn, CompIntense HPC, LLC
 * All rights reserved.
 *
 * See LICENSE file for details.
 *
 *    This software is distributed WITHOUT ANY WARRANTY; without even 
 *    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 *    PURPOSE.  See the above copyright notice for more information.
 *
 */

// Object Factory creating properly registered Interactor and RenderWindow 
// for implementing an event-driven interface with FLTK.

// VTK
#include "vtkVersion.h"
#include "vtkFLTKObjectFactory.h"
#include "vtkFLTKOpenGLRenderWindow.h"
#include "vtkFLTKRenderWindowInteractor.h"
#include "vtkFLTKInteractorStyleSwitch.h"
#include "vtkFLTKInteractorStyleJoystickCamera.h"


// ----------------------------------------------------------------------------
//      v t k F L T K O b j e c t F a c t o r y
// ----------------------------------------------------------------------------

// macro creates a function to create a vtkFLTKOpenGLRenderWindow
VTK_CREATE_CREATE_FUNCTION(vtkFLTKOpenGLRenderWindow);
// macro creates a function to create a vtkFLTKRenderWindowInteractor
VTK_CREATE_CREATE_FUNCTION(vtkFLTKRenderWindowInteractor);
// macro creates a function to create a vtkFLTKInteractorStyleSwitch
VTK_CREATE_CREATE_FUNCTION(vtkFLTKInteractorStyleSwitch);
// macro creates a function to create a vtkFLTKInteractorStyleJoystickCamera
VTK_CREATE_CREATE_FUNCTION(vtkFLTKInteractorStyleJoystickCamera);

//------------------------------------------------------------------------------
//vtkCxxRevisionMacro(vtkFLTKObjectFactory, "$Revision: 1.9 $");
// The first time CreateInstance is called, all dll's or shared libraries in the
// environment variable VTK_AUTOLOAD_PATH are loaded into the current process.
vtkStandardNewMacro(vtkFLTKObjectFactory);

// register the one override in the constructor of the factory
vtkFLTKObjectFactory::vtkFLTKObjectFactory(void)
{
  this->RegisterOverride("vtkRenderWindow",
                         "vtkFLTKOpenGLRenderWindow",
                         "concrete implementation of a rendering window"
                         " utilizing the OpenGL rendering library and a"
			 " FLTK interface to the windowing system",
                         1, // Enabled
                         vtkObjectFactoryCreatevtkFLTKOpenGLRenderWindow);
  this->RegisterOverride("vtkRenderWindowInteractor",
                         "vtkFLTKRenderWindowInteractor",
                         "concrete implementation of a rendering window"
			 " interactor for a Fl_VTK_Window \"peer\" class"
			 " implementing a FLTK event-driven interface",
                         1, // Enabled
                         vtkObjectFactoryCreatevtkFLTKRenderWindowInteractor);
#if 0
  // these do not seem to be effective for some reason (?)
  this->RegisterOverride("vtkInteractorStyleSwitch",
                         "vtkFLTKInteractorStyleSwitch",
                         "concrete implementation of an interactor"
			 " style switch",
                         1, // Enabled
                         vtkObjectFactoryCreatevtkFLTKInteractorStyleSwitch);
  this->RegisterOverride("vtkInteractorStyleJoystickCamera",
                         "vtkFLTKInteractorStyleJoystickCamera",
                         "concrete implementation of an interactor"
			 " style joystick + camera",
                         1, // Enabled
                         vtkObjectFactoryCreatevtkFLTKInteractorStyleJoystickCamera);
#endif
} // Constructor

// return the version of VTK that the factory was built with
const char*
vtkFLTKObjectFactory::GetVTKSourceVersion(void)
{
  return VTK_SOURCE_VERSION;
}

// return a text description of the factory
const char*
vtkFLTKObjectFactory::GetDescription(void)
{
  return "ObjectFactory for concrete Rendering classes implemented via FLTK";
}

// Macro to create the interface "C" functions used in
// a dll or shared library that contains a VTK object factory.
VTK_FACTORY_INTERFACE_IMPLEMENT(vtkFLTKObjectFactory);

// Registration of object factories.
static unsigned int vtkFLTKObjectFactoryCount;

void vtkFLTKOpenGLRendering_AutoInit_Construct()
{
  if(++vtkFLTKObjectFactoryCount == 1)
    {

    vtkFLTKObjectFactory* factory = vtkFLTKObjectFactory::New();
    if (factory)
      {
      // vtkObjectFactory keeps a reference to the "factory",
      vtkObjectFactory::RegisterFactory(factory);
      factory->Delete();
      }
    }
}

void vtkFLTKOpenGLRendering_AutoInit_Destruct()
{
  if(--vtkFLTKObjectFactoryCount == 0)
    {
    // Do not call vtkObjectFactory::UnRegisterFactory because
    // vtkObjectFactory.cxx statically unregisters all factories.
    }
}

/* 
 * End of: $Id: vtkFLTKObjectFactory.cxx,v 1.9 2004/06/16 18:07:53 xpxqx Exp $.
 * 
 */
