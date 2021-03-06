/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkFLTKOpenGLRenderWindow.cxx,v 1.42 2004/06/16 01:42:51 xpxqx Exp $
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
#include "vtkFLTKOpenGLRenderWindow.h"

#ifndef VTK_IMPLEMENT_MESA_CXX
#  include "vtkOpenGLRenderer.h"
#  include "vtkOpenGLProperty.h"
#  include "vtkOpenGLTexture.h"
#  include "vtkOpenGLCamera.h"
#  include "vtkOpenGLLight.h"
#  include "vtkOpenGLActor.h"
#  include "vtkOpenGLPolyDataMapper.h"
#else
#  include "MangleMesaInclude/osmesa.h"
#endif /* VTK_IMPLEMENT_MESA_CXX */

#include "vtkToolkits.h"
#ifndef VTK_IMPLEMENT_MESA_CXX
#  ifdef VTK_OPENGL_HAS_OSMESA
#    include <GL/osmesa.h>
#  endif /* VTK_OPENGL_HAS_OSMESA */
#endif /* VTK_IMPLEMENT_MESA_CXX */

// FLTK
#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/gl.h>

// Warning: whatever GLContextType is defined to must take
// exactly the same space in a structure as a void* !!!
#if defined(WIN32)
#  if defined(_MSC_VER) || defined (__BORLANDC__)
#    include <GL/glaux.h>
#  endif
typedef HGLRC           GLContextType;
typedef HWND            WindowIdType;
#elif defined(APPLE)
#  include <AGL/agl.h>
typedef AGLContext      GLContextType;
typedef WindowRef       WindowIdType;
#elif defined(UNIX)
#  include "GL/glx.h"
typedef GLXContext      GLContextType;
typedef Window          WindowIdType;
#endif

// VTK Common
#include "vtkCommand.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"

// vtkFLTK
#include "Fl_VTK_Window.H"

// Forwarded Standard C Library header.
#include <cstring>
// Standard C++ Library header.
#include <sstream>


class vtkFLTKOpenGLRenderWindow;
class vtkRenderWindow;


class vtkFLTKOpenGLRenderWindowInternal
{
  friend class vtkFLTKOpenGLRenderWindow;
private:
  vtkFLTKOpenGLRenderWindowInternal (vtkRenderWindow*);

#ifdef VTK_OPENGL_HAS_OSMESA
  // OffScreen stuff
  OSMesaContext OffScreenContextId;
  void*         OffScreenWindow;
  int           ScreenMapped;
  // Looks like this just stores DoubleBuffer.
  int           ScreenDoubleBuffer;
#endif /* VTK_OPENGL_HAS_OSMESA */
};

vtkFLTKOpenGLRenderWindowInternal::vtkFLTKOpenGLRenderWindowInternal (
#ifdef VTK_OPENGL_HAS_OSMESA
  vtkRenderWindow* a
#else
  vtkRenderWindow* vtkNotUsed(a)
#endif /* VTK_OPENGL_HAS_OSMESA */
  )
{
  // OpenGL specific
#ifdef VTK_OPENGL_HAS_OSMESA
  this->OffScreenContextId = 0;
  this->OffScreenWindow    = 0;
  this->ScreenMapped       = a->GetMapped();
  this->ScreenDoubleBuffer = a->GetDoubleBuffer();
#endif /* VTK_OPENGL_HAS_OSMESA */
}

// ----------------------------------------------------------------------------
//      v t k F L T K O p e n G L R e n d e r W i n d o w
// ----------------------------------------------------------------------------

#ifndef VTK_IMPLEMENT_MESA_CXX
//vtkCxxRevisionMacro (vtkFLTKOpenGLRenderWindow, "$Revision: 1.42 $");
vtkStandardNewMacro (vtkFLTKOpenGLRenderWindow);
#endif /* !VTK_IMPLEMENT_MESA_CXX */


#define MAX_LIGHTS 8


#ifdef VTK_OPENGL_HAS_OSMESA
// a couple of routines for offscreen rendering
void
vtkOSMesaDestroyWindow (void* aWindow) 
{
  free(aWindow);
}

void*
vtkOSMesaCreateWindow (int aWidth, int aHeight) 
{
  return malloc(aWidth * aHeight * 4);
}
#endif /* VTK_OPENGL_HAS_OSMESA */

// ----------------------------------------------------------------------------
vtkFLTKOpenGLRenderWindow::vtkFLTKOpenGLRenderWindow (void)
  : Internal(new vtkFLTKOpenGLRenderWindowInternal(this)),
    FlParent(NULL),
    FlWindow(NULL),
    Mode(-1),
    OwnFlWindow(0),
    CursorHidden(0),
    ForceMakeCurrent(0),
    UsingHardware(0),
    Capabilities(NULL)
{
  this->ScreenSize[0] = this->ScreenSize[1] = 0;
}

vtkFLTKOpenGLRenderWindow::~vtkFLTKOpenGLRenderWindow()
{
  // close-down all system-specific drawing resources
  this->Finalize();

  delete this->Internal;
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::Frame (void)
{
  vtkDebugMacro(<< "Frame() (subclass method)");

  this->MakeCurrent();

  if (!this->AbortRender && this->DoubleBuffer && this->SwapBuffers)
    {
    if (this->FlWindow != NULL)
      {
      this->FlWindow->swap_buffers();
      vtkDebugMacro(<< "Frame(): Fl_Gl_Window::swap_buffers()");
      }
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetBorders (int a)
{
  vtkDebugMacro(<< " setting Borders to " << a << ".");

  if (this->Borders == a)
    return;

  if (this->FlWindow != NULL)
    {
    this->FlWindow->border(a);

    if ((this->FlWindow->border() && a) || (!this->FlWindow->border() && !a))
      {
      this->Borders = a;
      this->Modified();
      }
    else
      {
      vtkWarningMacro(<< "Border change request failed. This must usually"
                      <<" be performed before a window has been realized.");
      }
    }
  else
    {
    this->Borders = a;
    this->Modified();
    }
}

void
vtkFLTKOpenGLRenderWindow::SetStereoCapableWindow (int a)
{
  vtkDebugMacro(<< " setting StereoCapableWindow to " << a << ".");

  if (this->StereoCapableWindow == a)
    return;

  if (!this->GetMapped())
    {
    this->StereoCapableWindow = a;
    this->Modified();
    }
  else
    {
    vtkWarningMacro(<< "Requesting StereoCapableWindow must be performed "
                    << "before a window is realized, i.e. before a render.");
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::WindowInitialize (void)
{
  int x = 5, y = 5, w = 300, h = 300;
  
  vtkDebugMacro(<< "WindowInitialize()");

  if (this->FlWindow != NULL)
    {
    x = this->FlWindow->x();
    y = this->FlWindow->y();
    w = this->FlWindow->w();
    h = this->FlWindow->h();
    }
  else
    {
    if (this->Position[0] >= 0) x = this->Position[0];
    if (this->Position[1] >= 0) y = this->Position[1];
    if (this->Size[0] > 0)      w = this->Size[0];
    if (this->Size[1] > 0)      h = this->Size[1];
    }
  
  if ( this->FlWindow != NULL  &&  !this->FlWindow->context() )
    {
    this->ReleaseGraphicsResources( this );
    }

  if (!this->OffScreenRendering)
    {
    // Ensure connection to the default display.
    this->CheckDisplayConnection();

    if (this->FlWindow == NULL) // Create our own window.
      {
      // NOTE: Fl_Gl_Window makes sure that a modifying call to resize()
      //       is made at the end of its constructor.
      this->FlWindow = new Fl_VTK_Window(this, x, y, w, h, this->WindowName);
      this->OwnFlWindow = 1;

      if (this->FlParent != NULL)
        {
	this->FlParent->add(static_cast<Fl_Widget*>(this->FlWindow));
        }
      }
    else                        // Use a supplied window.
      {
      if (this->FlParent != NULL && this->FlParent != this->FlWindow->parent())
        {
        this->FlParent->add(static_cast<Fl_Widget*>(this->FlWindow));
        }

      if (this->FlWindow->GetRenderWindow() != this)
        {
	this->FlWindow->SetRenderWindow(this);
        }

      this->FlWindow->resize_(x, y, w, h);
      }

    // Get the parent if one has not been set.
    if (this->FlParent == NULL)
      {
      this->SetFlParent(this->FlWindow->parent());
      }

    // Sets the OpenGL capabilites of the window as a side effect.
    (void) this->GetDesiredVisualMode();

    this->FlWindow->border(this->Borders);

    // Realize the widget.
    if (this->FlParent != NULL)
      this->FlParent->show();
    else
      this->FlWindow->show();
    // Bring its GL context up to date.
    this->MakeCurrent();
    // Flushes the output buffer and wait until all requests
    // have been received and processed by the server.
    Fl::check();

    this->Mapped      = this->FlWindow->shown();
    this->Position[0] = this->FlWindow->x();
    this->Position[1] = this->FlWindow->y();
    this->Size[0]     = this->FlWindow->w();
    this->Size[1]     = this->FlWindow->h();
    }
  else
    {
    this->DoubleBuffer = 0;
#ifdef VTK_OPENGL_HAS_OSMESA
    if (this->Internal->OffScreenWindow == NULL)
      {
      this->Internal->OffScreenWindow = vtkOSMesaCreateWindow(w,h);
      this->Size[0]     = w;
      this->Size[1]     = h;      
      this->OwnFlWindow = 1;
      }    
    this->Internal->OffScreenContextId = OSMesaCreateContext(GL_RGBA, 0);
#endif /* VTK_OPENGL_HAS_OSMESA */
    this->MakeCurrent();
    this->Mapped = 0;
    }
    
  //
  // Tell our renderers about us.
  //
  vtkRenderer* renderer;

  for ( this->Renderers->InitTraversal(); 
        (renderer = this->Renderers->GetNextItem()) != 0; )
    {
    // Each vtkProp in the renderer calls ReleaseGraphicsResources().
    renderer->SetRenderWindow(NULL);
    renderer->SetRenderWindow(this);
    }

  //
  // Sets up OpenGL with MODELVIEW matrix mode, enabled LEQUAL depth test,
  // a texture environment with texture function GL_MODULATE, enabled blending
  // with GL_SRC_ALPHA and GL_ONE_MINUS_SRC_ALPHA. Point, line, and polygon
  // smoothing are then toggled according to their respective ivars.
  // GL_NORMALIZE is enable and the light model set to GL_LIGHT_MODEL_TWO_SIDE.
  //
  this->OpenGLInit();

  glAlphaFunc(GL_GREATER, 0);
}

void
vtkFLTKOpenGLRenderWindow::Initialize (void)
{
  vtkDebugMacro(<< "Initialize()");

  // Check to see if already been initialized.
  if ( (this->FlWindow != NULL && this->FlWindow->context() != NULL)
#ifdef VTK_OPENGL_HAS_OSMESA
       || this->Internal->OffScreenContextId != NULL
#endif /* VTK_OPENGL_HAS_OSMESA */
    )
    {
    return;
    }

  // Now initialize the window.
  this->WindowInitialize();
}

void
vtkFLTKOpenGLRenderWindow::Finalize (void)
{
  vtkDebugMacro(<< "Finalize()");

  // make sure we have been initialized 
  if ( (this->FlWindow != NULL && this->FlWindow->context() != NULL)
#ifdef VTK_OPENGL_HAS_OSMESA
       || this->Internal->OffScreenContextId != NULL
#endif /* VTK_OPENGL_HAS_OSMESA */
    )
    {
    this->MakeCurrent();

    // tell each of the renderers that this render window/graphics context
    // is being removed (the RendererCollection is removed by 
    // vtkRenderWindow's destructor)
    vtkRenderer* ren;

    for ( this->Renderers->InitTraversal();
          (ren = this->Renderers->GetNextItem()) != NULL; )
      {
      // Each vtkProp in the renderer calls ReleaseGraphicsResources().
      ren->SetRenderWindow(NULL);
      }

    // First, delete all the old lights.
    for (short lighti=GL_LIGHT0; lighti<GL_LIGHT0+MAX_LIGHTS; lighti++)
      {
      glDisable(GLenum(lighti));
      }

    // Now delete all textures.
    GLuint id;

    glDisable(GL_TEXTURE_2D);
    std::map<const vtkTextureObject*, int>::iterator  mit = this->TextureResourceIds.begin();
    for ( ; mit != this->TextureResourceIds.end(); ++mit )
      {
      id = GLuint(mit->second);
#ifdef GL_VERSION_1_1
      if (glIsTexture(id))
        {
        glDeleteTextures(1, &id);
        }
#else
      if (glIsList(id))
        {
        glDeleteLists(id, 1);
        }
#endif /* GL_VERSION_1_1 */
      }

    glFinish();

#ifdef VTK_OPENGL_HAS_OSMESA
    if (this->OffScreenRendering && this->Internal->OffScreenContextId != NULL)
      {
      OSMesaDestroyContext(this->Internal->OffScreenContextId);
      this->Internal->OffScreenContextId = NULL;
      vtkOSMesaDestroyWindow(this->Internal->OffScreenWindow);
      this->Internal->OffScreenWindow    = NULL;
      }
    else
#endif /* VTK_OPENGL_HAS_OSMESA */
      {
      this->FlWindow->context(NULL, true/*destroy*/);

      // then close the old window 
      if ( this->OwnFlWindow &&
#if !defined(APPLE)
           fl_display != NULL &&
#endif /* APPLE */
           this->FlWindow->shown() )
        {
        this->FlWindow->hide();
        }
      }
    }

#if !defined(APPLE)
  if (fl_display != NULL)
#endif /* APPLE */
    {
    Fl::check();
    }

  if (this->Capabilities != NULL)
    {
    delete [] this->Capabilities;
    this->Capabilities = NULL;
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetFullScreen (int a)
{
  vtkDebugMacro(<< "SetFullScreen(" << a << ") (subclass method)");

  if (this->OffScreenRendering || (this->FullScreen == a))
    {
    return;
    }
  
  if (!this->GetMapped())
    {
    this->PrefFullScreen();
    return;
    }

  // set the mode 
  if ((this->FullScreen = a) > 0)
    {
    // if window already up, get its values .
    if (this->FlWindow != NULL)
      {
      this->OldScreen[0] = this->FlWindow->x();
      this->OldScreen[1] = this->FlWindow->y();
      this->OldScreen[2] = this->FlWindow->w();
      this->OldScreen[3] = this->FlWindow->h();
      this->OldScreen[4] = this->FlWindow->border();
      this->PrefFullScreen();
      }
    }
  else
    {
    this->Position[0] = this->OldScreen[0];
    this->Position[1] = this->OldScreen[1];
    this->Size[0]     = this->OldScreen[2]; 
    this->Size[1]     = this->OldScreen[3];
    this->Borders     = this->OldScreen[4];
    }
  
  // remap the window 
  this->WindowRemap();

  if (this->FlWindow != NULL)
    {
    // if full screen then grab the keyboard 
    if (this->FullScreen)
      {
      if (!this->FlWindow->take_focus())
        {
        vtkWarningMacro(<< "SetFullScreen(): "
                        << "Failed to take focus before going to FullScreen!");
        }
      // Makes the window completely fill the screen, without any window
      // manager border visible. You must use fullscreen_off() to undo this.
      // This may not work with all window managers.
      this->FlWindow->fullscreen();
      }
    else
      {
      // Turns off any side effects of fullscreen() and does resize(x,y,w,h).
      this->FlWindow->fullscreen_off( this->OldScreen[0], this->OldScreen[1],
                                      this->OldScreen[2], this->OldScreen[3] );
      this->FlWindow->border(this->OldScreen[4]);
      }
    }

  this->Modified();
}

void
vtkFLTKOpenGLRenderWindow::PrefFullScreen (void)
{
  vtkDebugMacro(<< "PrefFullScreen()");

  // use full screen 
  this->Position[0] = 0;
  this->Position[1] = 0;
  this->Size[0]     = (this->OffScreenRendering ? 1280 : Fl::w());
  this->Size[1]     = (this->OffScreenRendering ? 1024 : Fl::h());
  
  // don't show borders 
  this->Borders = 0;
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::WindowRemap (void)
{
  vtkDebugMacro(<< "WindowRemap() (subclass method)");

  // Shut everything down.
  this->Finalize();

  // Set everything up again.
  this->Initialize();
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::Start (void)
{
  vtkDebugMacro(<< "Start() (subclass method)");

  // If the renderer has not been initialized, do so now.
  if ( (this->FlWindow == NULL || this->FlWindow->context() == NULL)
#ifdef VTK_OPENGL_HAS_OSMESA
       && !this->Internal->OffScreenContextId
#endif /* VTK_OPENGL_HAS_OSMESA */
    )
    {
    this->Initialize();
    }

  // Set the current window and bring it up to date.
  this->MakeCurrent();
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetSize (int aWidth, int aHeight)
{
  vtkDebugMacro(<< "SetSize(" << aWidth << "," << aHeight
                << ") (subclass method)");

  // If not mapped or offscreen, then just the ivars are set.
  if (this->Size[0] != aWidth || this->Size[1] != aHeight)
    {
    this->Size[0] = aWidth;
    this->Size[1] = aHeight;
    this->Modified();
    }

  if (this->OffScreenRendering 
#ifdef VTK_OPENGL_HAS_OSMESA
      && this->Internal->OffScreenWindow != NULL
#endif /* VTK_OPENGL_HAS_OSMESA */
    )
    {
    vtkRenderer* ren;

    // Disconnect renderers from this render window.
    vtkRendererCollection* renderers = this->Renderers;

    renderers->Register(this);
    this->Renderers->Delete();
    this->Renderers = vtkRendererCollection::New();

    for ( renderers->InitTraversal();
          (ren = renderers->GetNextItem()) != NULL; )
      {
      ren->SetRenderWindow(NULL);
      }
    
#ifdef VTK_OPENGL_HAS_OSMESA
    OSMesaDestroyContext(this->Internal->OffScreenContextId);
    this->Internal->OffScreenContextId = NULL;
    vtkOSMesaDestroyWindow(this->Internal->OffScreenWindow);
    this->Internal->OffScreenWindow    = NULL;
#endif /* VTK_OPENGL_HAS_OSMESA */

    this->WindowInitialize();
    
    // Add the renders back into the render window.
    for ( renderers->InitTraversal();
          (ren = renderers->GetNextItem()) != NULL; )
      {
      this->AddRenderer(ren);
      }
    renderers->Delete();
    }
  else
    {
    if (this->FlWindow != NULL)
      {
      if ( this->Size[0] != this->FlWindow->w() ||
           this->Size[1] != this->FlWindow->h() )
        {
        this->FlWindow->resize_( this->FlWindow->x(), this->FlWindow->y(),
                                 this->Size[0], this->Size[1] );

        if (this->FlWindow->shown()) Fl::flush();

        this->Position[0] = this->FlWindow->x();
        this->Position[1] = this->FlWindow->y();
        }
      }
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetMapped (int a)
{
  vtkDebugMacro(<< "SetMapped(" << a << ") (subclass method)");

  if (a != this->Mapped)
    {
    this->Mapped = a;
    this->Modified();
    }

  if (this->FlWindow == NULL)
    {
    if (this->Mapped)
      {
      this->WindowRemap();
      }
    }
  else
    {
    if (this->Mapped)
      {
      if (!this->FlWindow->shown())
        {
        this->WindowRemap();
        }
      }
    else
      {
      if (this->FlWindow->shown())
        {
        this->FlWindow->hide();
        }
      }
    }
}

int
vtkFLTKOpenGLRenderWindow::GetMapped (void)
{
  if (this->FlWindow == NULL)
    {
    if (this->Mapped != 0)
      {
      this->Mapped = 0;
      this->Modified();
      }
    }
  else
    {
    if (this->Mapped != this->FlWindow->shown())
      {
      this->Mapped = this->FlWindow->shown();
      this->Modified();
      }
    }

  vtkDebugMacro(<< "GetMapped() returning " << this->Mapped << ".");

  return this->Mapped;
}

// ----------------------------------------------------------------------------
int
vtkFLTKOpenGLRenderWindow::GetDesiredVisualMode (void)
{
  int mode = Fl_VTK_Window::desired_mode( this->DoubleBuffer,
                                          this->StereoCapableWindow,
                                          this->MultiSamples );

  if (mode != 0)
    {
    if (!Fl::gl_visual(mode) && this->Mode != 0)
      {
      mode = ( Fl::gl_visual(this->Mode)
               ? this->Mode
               : FL_RGB|FL_DOUBLE|FL_DEPTH );
      }
    }

  if (this->FlWindow != NULL && mode != 0)
    {
    this->FlWindow->mode(mode);
    mode = this->FlWindow->mode();
    }

  this->Mode = mode;

  vtkDebugMacro(<< "GetDesiredVisualMode(): returning ( "
                << ( (this->Mode & FL_INDEX) ? "FL_INDEX" :
                     ( (this->Mode & FL_RGB8) ? "FL_RGB8" : "FL_RGB" ) )
                << ((this->Mode & FL_DOUBLE) ? " | FL_DOUBLE" : " | FL_SINGLE")
                << ((this->Mode & FL_ACCUM) ? " | FL_ACCUM" : "")
                << ((this->Mode & FL_ALPHA) ? " | FL_ALPHA" : "")
                << ((this->Mode & FL_DEPTH) ? " | FL_DEPTH" : "")
                << ((this->Mode & FL_STENCIL) ? " | FL_STENCIL" : "")
                << ((this->Mode & FL_MULTISAMPLE) ? " | FL_MULTISAMPLE" : "")
                << ((this->Mode & FL_STEREO) ? " | FL_STEREO" : "")
                << " )");

  return this->Mode;
}

// ----------------------------------------------------------------------------
// the following can be useful for debugging XErrors
// When uncommented (along with the lines in MakeCurrent) 
// it will cause a segfault upon an XError instead of 
// the normal XError handler
#ifdef VTK_FLTK_OPENGL_RENDER_WINDOW_XERROR_DEBUG
extern "C"
{
int vtkXError (Display* vtkNotUsed(aDisplay), XErrorEvent* vtkNotUsed(aErr))
{
  // cause a segfault
  *(float *)(0x01) = 1.0;
  return 1;
}
}
#endif /* VTK_FLTK_OPENGL_RENDER_WINDOW_XERROR_DEBUG */

void
vtkFLTKOpenGLRenderWindow::MakeCurrent (void)
{
  // When debugging XErrors uncomment the following lines:
#if defined(VTK_FLTK_OPENGL_RENDERWINDOW_XERROR_DEBUG)&&!defined(APPLE)
  if (fl_display != NULL)
    {
    XSynchronize(fl_display,1);
    }
  XSetErrorHandler(vtkXError);
#endif /* VTK_FLTK_OPENGL_RENDERWINDOW_XERROR_DEBUG */

#ifdef VTK_OPENGL_HAS_OSMESA
  // set the current window 
  if (this->OffScreenRendering)
    {
    if (this->Internal->OffScreenContextId)
      {
      if (OSMesaMakeCurrent(this->Internal->OffScreenContextId, 
                            this->Internal->OffScreenWindow, GL_UNSIGNED_BYTE, 
                            this->Size[0], this->Size[1]) != GL_TRUE) 
        {
        vtkWarningMacro("MakeCurrent(): Failed call to OSMesaMakeCurrent.");
        }
      }
    }
  else
#endif /* VTK_OPENGL_HAS_OSMESA */
    {
    if (this->GetMapped())
      {
      if ( this->ForceMakeCurrent ||
           this->FlWindow->context() == NULL ||
           this->FlWindow != Fl_Window::current() )
        {
        vtkDebugMacro(<< "MakeCurrent() (subclass method, "
                      << (this->ForceMakeCurrent ? "is" : "not") << " forced, "
                      << (this->FlWindow != Fl_Window::current() ?
                          "not" : "is") << " current, context: "
                      << (void *) this->FlWindow->context() << ")");
        this->FlWindow->make_current();
        vtkDebugMacro(<< "MakeCurrent() make_current() called, new context: "
                      << (void *) this->FlWindow->context());
        this->ForceMakeCurrent = 0;
        }
      }
    }
}

void
vtkFLTKOpenGLRenderWindow::SetForceMakeCurrent (void)
{
  vtkDebugMacro(<< "SetForceMakeCurrent() (subclass method)");

  this->ForceMakeCurrent = 1;
}

bool
vtkFLTKOpenGLRenderWindow::IsCurrent ()
{
  return this->FlWindow == Fl_Window::current();
}

void
vtkFLTKOpenGLRenderWindow::CreateAWindow()
{
  return;
}

void
vtkFLTKOpenGLRenderWindow::DestroyWindow()
{
  return;
}

// ----------------------------------------------------------------------------
void*
vtkFLTKOpenGLRenderWindow::GetGenericContext (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  void* ptr = NULL;

#if defined(MESA) && defined(VTK_OPENGL_HAS_OSMESA)
  if (this->OffScreenRendering)
    {
    ptr = (void *) this->Internal->OffScreenContextId;
    }
  else
#endif /* MESA && VTK_OPENGL_HAS_OSMESA */
#if !defined(APPLE)
    {
    ptr = (void *) fl_gc;
    }
#endif /* APPLE */

  vtkDebugMacro(<< "GetGenericContext()  returning " << ptr);

  return ptr;
}

// ----------------------------------------------------------------------------
int
vtkFLTKOpenGLRenderWindow::GetEventPending (void)
{
#if defined(UNIX) && !defined(APPLE)
  if ( (fl_xevent != NULL) &&
       ( fl_find( (reinterpret_cast<const XAnyEvent *>(fl_xevent))->
                  window ) == this->FlWindow ) &&
       (fl_xevent->type == ButtonPress) )
    {
    vtkDebugMacro(<< "GetEventPending(): returning TRUE");
    return 1;
    }
#endif /* !UNIX */

  vtkDebugMacro(<< "GetEventPending(): returning FALSE");

  return 0;
}

// ----------------------------------------------------------------------------
int*
vtkFLTKOpenGLRenderWindow::GetScreenSize (void)
{
  this->ScreenSize[0] = Fl::w();
  this->ScreenSize[1] = Fl::h();

  vtkDebugMacro(<< "GetScreenSize(): returning ("
                << this->ScreenSize[0] << "," << this->ScreenSize[1] << ")");

  return this->ScreenSize;
}

int*
vtkFLTKOpenGLRenderWindow::GetSize (void)
{
  if (this->FlWindow != NULL)
    {
    // Find the current window size.
    if ( (this->Size[0] != this->FlWindow->w()) ||
         (this->Size[1] != this->FlWindow->h()) )
      {
      this->Size[0] = this->FlWindow->w();
      this->Size[1] = this->FlWindow->h();
      this->Modified();
      }
    }

  // Now do the superclass stuff.
  return this->Superclass::GetSize();
}

int*
vtkFLTKOpenGLRenderWindow::GetPosition (void)
{
  if (this->FlWindow != NULL)
    {
    // Find the current window position.
    if ( (this->Position[0] != this->FlWindow->x()) ||
         (this->Position[1] != this->FlWindow->y()) )
      {
      this->Position[0] = this->FlWindow->x();
      this->Position[1] = this->FlWindow->y();
      this->Modified();
      }
    }

  vtkDebugMacro(<< "GetPosition(): returning ("
                << this->Position[0] << ","
		<< this->Position[1] << ")");

  return this->Position;
}

// ----------------------------------------------------------------------------
Fl_Group*
vtkFLTKOpenGLRenderWindow::GetFlParent (void)
{
  if (this->FlParent == NULL && this->FlWindow != NULL)
    {
    this->SetFlParent(this->FlWindow->parent());
    }

  vtkDebugMacro(<< "GetFlParent(): returning " << (void *) this->FlParent);

  return this->FlParent;
}

Fl_VTK_Window*
vtkFLTKOpenGLRenderWindow::GetFlWindow (void)
{
  vtkDebugMacro(<< "GetFlWindow(): returning " << (void *) this->FlWindow);

  return this->FlWindow;
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetPosition (int aX, int aY)
{
  vtkDebugMacro(<< "SetPosition(" << aX << "," << aY << ")");

  // If not mapped, then just the ivars are set.
  if (this->Position[0] != aX || this->Position[1] != aY)
    {
    this->Position[0] = aX;
    this->Position[1] = aY;
    this->Modified();
    }

  if (this->FlWindow != NULL)
    {
    if ( this->Position[0] != this->FlWindow->x() ||
         this->Position[1] != this->FlWindow->y() )
      {
      this->FlWindow->resize_( this->Position[0], this->Position[0],
                               this->FlWindow->w(), this->FlWindow->h() );

      if (this->FlWindow->shown()) Fl::check();
      }
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetFlParent (Fl_Group* a)
{
  if (this->FlParent != NULL)
    {
    vtkErrorMacro("FlParent is already set.");
    return;
    }
  
  vtkDebugMacro(<< "SetFlParent(" << (void *) a << ")");

  this->FlParent = a;
}

void
vtkFLTKOpenGLRenderWindow::SetFlWindow (Fl_VTK_Window* a)
{
  vtkDebugMacro(<< "SetFlWindow(" << (void *) a << ")"); 

  this->FlWindow = a;

  if (this->CursorHidden)
    {
    this->CursorHidden = 0;
    this->HideCursor();
    }
}

void
vtkFLTKOpenGLRenderWindow::SetParentId (void* a)
{
  vtkDebugMacro(<< "SetParentId(" << a << ")");

#if !defined(APPLE)
  Fl_Window* window;

  if ((window = fl_find(WindowIdType(a))) != NULL)
    {
    this->SetFlParent(static_cast<Fl_Group*>(window));
    }
  else
#endif /* defined(APPLE) */
    {
    vtkWarningMacro(<< "SetParentId(): Fl_Window corresponding to window ID"
		    << " ( " << a << " ) not found.");
    }
}

void
vtkFLTKOpenGLRenderWindow::SetWindowId (void* a)
{
  vtkDebugMacro(<< "SetWindowId(" << a << ")");

#if !defined(APPLE)
  Fl_Window* window;

  if ((window = fl_find(WindowIdType(a))) != NULL)
    {
    if (window->type() == VTK_WINDOW_TYPE)
      {
      this->SetFlWindow(static_cast<Fl_VTK_Window*>(window));
      }
    else
      {
      vtkWarningMacro(<< "SetWindowId(): Fl_Window ( " << window
                      << " ) corresponding to window ID"
		      << " ( " << a << " ) is not a Fl_VTK_Window,");
      }
    }
  else
#endif /* defined(APPLE) */
    {
    vtkWarningMacro(<< "SetWindowId(): Fl_Window corresponding to window ID"
		    << " ( " << a << " ) not found.");
    }
}

void
#if VTK_MINOR_VERSION > 1
vtkFLTKOpenGLRenderWindow::SetWindowInfo (const char* aInfo)
#else
vtkFLTKOpenGLRenderWindow::SetWindowInfo (char* aInfo)
#endif
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  long tmp;
  sscanf(aInfo, "%ld", &tmp);
 
  this->SetWindowId((void *) tmp);
}

void
#if VTK_MINOR_VERSION > 1
vtkFLTKOpenGLRenderWindow::SetParentInfo (const char* a)
#else
vtkFLTKOpenGLRenderWindow::SetParentInfo (char* a)
#endif
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  long tmp;
  sscanf(a, "%ld", &tmp);
 
  this->SetParentId((void *) tmp);
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetNextWindowId (void* vtkNotUsed(a))
{
  vtkWarningMacro("SetNextWindowId() not implemented.");
}

void
#if VTK_MINOR_VERSION > 1
vtkFLTKOpenGLRenderWindow::SetNextWindowInfo (const char* a)
#else
vtkFLTKOpenGLRenderWindow::SetNextWindowInfo (char* a)
#endif
{
  long tmp;
  sscanf(a, "%ld", &tmp);
  
  this->SetNextWindowId((void *) tmp);
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetFlWindow (void* a)
{
  this->SetFlWindow(reinterpret_cast<Fl_VTK_Window*>(a));
}

void
vtkFLTKOpenGLRenderWindow::SetFlParent (void* a)
{
  this->SetFlParent(reinterpret_cast<Fl_Group*>(a));
}

//----------------------------------------------------------------------------
const char*
vtkFLTKOpenGLRenderWindow::ReportCapabilities (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  vtkDebugMacro(<< "ReportCapabilities() (subclass method)");

  this->MakeCurrent();

  std::ostringstream oss;

#if defined(WIN32)
#elif defined(APPLE)
#elif defined(UNIX)
  const char* serverVendor =
    glXQueryServerString(fl_display, fl_screen, GLX_VENDOR);
  const char* serverVersion =
    glXQueryServerString(fl_display, fl_screen, GLX_VERSION);
  const char* serverExtensions =
    glXQueryServerString(fl_display, fl_screen, GLX_EXTENSIONS);
  const char* clientVendor =
    glXGetClientString(fl_display, GLX_VENDOR);
  const char* clientVersion =
    glXGetClientString(fl_display, GLX_VERSION);
  const char* clientExtensions =
    glXGetClientString(fl_display, GLX_EXTENSIONS);
  const char* glxExtensions =
    glXQueryExtensionsString(fl_display, fl_screen);

  oss << "server glx vendor string:  " << serverVendor << endl;
  oss << "server glx version string:  " << serverVersion << endl;
  oss << "server glx extensions:  " << serverExtensions << endl;
  oss << "client glx vendor string:  " << clientVendor << endl;
  oss << "client glx version string:  " << clientVersion << endl;
  oss << "client glx extensions:  " << clientExtensions << endl;
  oss << "glx extensions:  " << glxExtensions << endl;
#endif

  const char* glVendor =
    reinterpret_cast<const char *>(glGetString(GL_VENDOR));
  const char* glRenderer =
    reinterpret_cast<const char *>(glGetString(GL_RENDERER));
  const char* glVersion =
    reinterpret_cast<const char *>(glGetString(GL_VERSION));
  const char* glExtensions =
    reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

  oss << "OpenGL vendor string:  " << glVendor << endl;
  oss << "OpenGL renderer string:  " << glRenderer << endl;
  oss << "OpenGL version string:  " << glVersion << endl;
  oss << "OpenGL extensions:  " << glExtensions << endl;

#if defined(UNIX) && !defined(APPLE)
  oss << "X Extensions:  ";

  int n = 0;
  char** extlist = XListExtensions(fl_display, &n);

  for (int i=0; i<n; i++) 
    {
    if (i != n-1)
      oss << extlist[i] << ", ";
    else
      oss << extlist[i] << endl;
    }
#endif

  oss << ends;

  if (this->Capabilities != NULL)
    delete [] this->Capabilities;

  int nc = oss.str().length();

  this->Capabilities = new char[ nc + 1 ];
  strcpy(this->Capabilities, oss.str().c_str());
  
  return this->Capabilities;
}

int
vtkFLTKOpenGLRenderWindow::SupportsOpenGL (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  vtkDebugMacro(<< "SupportsOpenGL() (subclass method)");

  this->MakeCurrent();

  int value = 0;

  if (this->GetDesiredVisualMode() != 0)
    {
#if defined(WIN32)
    int pixelFormat = GetPixelFormat(fl_gc);
    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(fl_gc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR),&pfd);
    value = (pfd.dwFlags & PFD_SUPPORT_OPENGL ? 1 : 0);
#elif defined(APPLE)
#  if 0
    GrafPtr  port;
    GDHandle device;
    GetGWorld(&port, &device);
    GLint attribs[] =
      {AGL_ALL_RENDERERS, AGL_RGBA, AGL_DEPTH_SIZE,32, AGL_NONE};
    AGLPixelFmtID pix = aglChoosePixelFmt(&device, 0, attribs);
    aglGetConfig(pix, AGL_USE_GL, &value);
#  endif /* 0 */
#elif defined(UNIX)
    glXGetConfig(fl_display, fl_visual, GLX_USE_GL, &value);
#endif
    }

  vtkDebugMacro(<< "SupportsOpenGL(): returning " << value);

  return value;
}

int
vtkFLTKOpenGLRenderWindow::IsDirect (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  vtkDebugMacro(<< "IsDirect() (subclass method)");

  this->MakeCurrent();

  if (this->FlWindow == NULL || this->FlWindow->context() == NULL)
    {
    return 0;
    }

#if defined(WIN32)
  int pixelFormat = GetPixelFormat(fl_gc);
  PIXELFORMATDESCRIPTOR pfd;
  DescribePixelFormat(fl_gc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
  this->UsingHardware = (pfd.dwFlags & PFD_GENERIC_FORMAT ? 1 : 0);
#elif defined(APPLE)
#  if 0
  GrafPtr  port;
  GDHandle device;
  GetGWorld(&port, &device);
  GLint attribs[] =
    {AGL_ALL_RENDERERS, AGL_RGBA, AGL_DEPTH_SIZE,32, AGL_FULLSCREEN, AGL_NONE};
  AGLPixelFmtID pix = aglChoosePixelFmt(&device, 0, attribs);
  this->UsingHardware = (pix ? 1 : 0);
#  endif /* 0 */
#elif defined(UNIX)
  this->UsingHardware =
    (glXIsDirect(fl_display,GLXContext(this->FlWindow->context())) ?1:0);
#endif

  vtkDebugMacro(<< "IsDirect(): returning " << this->UsingHardware);

  return this->UsingHardware;
}

//----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetWindowName (const char* aName)
{
  this->Superclass::SetWindowName(aName);

  if (this->FlWindow != NULL)
    {
    this->FlWindow->label(this->WindowName);
    }
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::CheckDisplayConnection (void)
{
#if !defined(APPLE)
  if (fl_display == NULL)
#endif /* APPLE */
    {
#if !defined(WIN32)
    fl_open_display();
#endif /* WIN32 */

#if !defined(APPLE)
    if (fl_display != NULL)
      {
      vtkDebugMacro(<< "Opened display " << (void *) fl_display); 
      }
    else
      {
      vtkErrorMacro(<< "Bad Display connection.\n");
      }
#endif /* APPLE */
    }
}

void
vtkFLTKOpenGLRenderWindow::SetDisplayId (void* a)
{
#if !defined(APPLE)
  if (fl_display == NULL)
#endif
    {
#if defined(APPLE)
    fl_open_display();
#elif !defined(WIN32)
    fl_open_display(reinterpret_cast<Display*>(a));
#endif

#if !defined(APPLE)
    if (fl_display != NULL)
      {
      vtkDebugMacro(<< "Opened display " << (void *) fl_display); 
      }
#  if !defined(WIN32)
    else
      {
      vtkErrorMacro(<< "Bad Display connection.\n");
      }
#  endif
#endif
    }
#if !defined(APPLE)
  else
    {
    vtkWarningMacro(<< "FLTK cannot change the Display ID"
                    << " from an existing Display connection.\n");
    }
#endif
}

void
vtkFLTKOpenGLRenderWindow::SetDisplayInfo (char* a)
{
#if !defined(APPLE)
  if (fl_display == NULL)
#endif
    {
    Fl::display(static_cast<const char *>(a));
#if !defined(WIN32)
    fl_open_display();
#endif

#if !defined(APPLE)
    if (fl_display != NULL)
      {
      vtkDebugMacro(<< "Opened display " << (void *) fl_display); 
      }
#  if !defined(WIN32)
    else
      {
      vtkErrorMacro(<< "Bad Display connection.\n");
      }
#  endif
#endif
    }
#if !defined(APPLE)
  else
    {
    vtkWarningMacro(<< "FLTK cannot change the Display ID"
                    << " from an existing Display connection.\n");
    }
#endif
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::Render (void)
{
  vtkDebugMacro(<< "Render() (subclass method)"); 

  if (this->FlWindow != NULL)
    {
    //  Find the current window position 
    this->Position[0]   = this->FlWindow->x();
    this->Position[1]   = this->FlWindow->y();
    //  Find the current window size 
    this->Size[0]       = this->FlWindow->w();
    this->Size[1]       = this->FlWindow->h();
    }

  // Now do the superclass stuff.
  this->Superclass::Render();
}

void
vtkFLTKOpenGLRenderWindow::HideCursor (void)
{
  vtkDebugMacro(<< "HideCursor()"); 

  if (!this->CursorHidden && this->FlWindow != NULL)
    {
    this->FlWindow->cursor(FL_CURSOR_NONE);
    }
  this->CursorHidden = 1;
}

void
vtkFLTKOpenGLRenderWindow::ShowCursor (void)
{
  vtkDebugMacro(<< "ShowCursor()"); 

  if (this->CursorHidden && this->FlWindow != NULL)
    {
    this->FlWindow->cursor(FL_CURSOR_DEFAULT);
    }
  this->CursorHidden = 0;
}                                  

//----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetOffScreenRendering (int aToggle)
{
  vtkDebugMacro(<< "SetOffScreenRendering( " << aToggle << " )"); 

  if (this->OffScreenRendering == aToggle)
    {
    return;
    }
  
#ifdef VTK_OPENGL_HAS_OSMESA
  // Invoke superclass method.
  this->Superclass::SetOffScreenRendering(aToggle);
  
  // setup everything
  if (aToggle)
    {
    this->Internal->ScreenDoubleBuffer  = this->DoubleBuffer;
    this->DoubleBuffer                  = 0;
    this->Internal->ScreenMapped        = this->Mapped;
    this->Mapped                        = 0;

    if (this->Internal->OffScreenWindow == NULL)
      {
      this->WindowInitialize();
      }
    }
  else
    {
    if (this->Internal->OffScreenWindow != NULL)
      {
      OSMesaDestroyContext(this->Internal->OffScreenContextId);
      this->Internal->OffScreenContextId= NULL;
      vtkOSMesaDestroyWindow(this->Internal->OffScreenWindow);
      this->Internal->OffScreenWindow   = NULL;
      }

    this->DoubleBuffer  = this->Internal->ScreenDoubleBuffer;
    this->Mapped        = this->Internal->ScreenMapped;

    this->MakeCurrent();
    (void) this->GetSize(); // reset the size based on the screen window
    this->WindowInitialize();
    }
#endif /* VTK_OPENGL_HAS_OSMESA */
}

// ----------------------------------------------------------------------------
void*
vtkFLTKOpenGLRenderWindow::GetGenericDisplayId (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

#if !defined(APPLE)
  vtkDebugMacro(<< "GetGenericDisplayId(): returning "
                << (void *) fl_display); 

  return (void *) fl_display;
#else
  return (void *) 0;
#endif /* APPLE */
}

void*
vtkFLTKOpenGLRenderWindow::GetGenericWindowId (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  void* ptr = NULL;

#ifdef VTK_OPENGL_HAS_OSMESA
  if (this->OffScreenRendering)
    {
    ptr = (void *) this->Internal->OffScreenWindow;
    }
  else
#endif /* VTK_OPENGL_HAS_OSMESA */
    {
    if (this->GetMapped())
      {
      ptr = (void *) fl_xid(this->FlWindow);
      }
    }

  vtkDebugMacro(<< "GetGenericWindowId(): returning " << ptr);

  return ptr;
}

void*
vtkFLTKOpenGLRenderWindow::GetGenericParentId (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  void* ptr = NULL;

  if ( this->GetFlParent() != NULL &&
       this->FlParent->type() >= FL_WINDOW )
    {
    Fl_Window* parent = static_cast<Fl_Window*>(this->FlParent);

    if (parent->shown())
      {
      ptr = (void *) fl_xid(parent);
      }
    }
  else
    {
    vtkWarningMacro(<< "GetGenericParentId(): Parent ("
                    << (void *) this->FlParent
                    << ") is not a Fl_Window subclass.");
    }

  vtkDebugMacro(<< "GetGenericParentId(): returning " << ptr);

  return ptr;
}

void*
vtkFLTKOpenGLRenderWindow::GetGenericDrawable (void)
{
  // Ensure existence of the default display connection.
  this->CheckDisplayConnection();

  void* ptr = NULL;

  if (this->GetMapped())
    {
    ptr = (void *) fl_xid(this->FlWindow);
    }

  vtkDebugMacro(<< "GetGenericDrawable(): returning " << ptr);

  return ptr;
}

//----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::SetCurrentCursor (int aShape)
{
  vtkDebugMacro(<< "SetCurrentCursor(" << aShape << ") (subclass method)"); 

  if (this->InvokeEvent(vtkCommand::CursorChangedEvent, &aShape))
    {
    return;
    }

  this->Superclass::SetCurrentCursor(aShape);

  if (this->FlWindow == NULL)
    {
    return;
    }

  //
  // Under X you can get any XC_cursor value by passing:
  //   this->FlWindow->cursor(Fl_Cursor((XC_foo/2)+1));
  //
  switch (aShape)
    {
    case VTK_CURSOR_DEFAULT:
      this->FlWindow->cursor(FL_CURSOR_DEFAULT);
      break;
    case VTK_CURSOR_ARROW:
      this->FlWindow->cursor(FL_CURSOR_ARROW);
      break;
    case VTK_CURSOR_SIZEALL:
      this->FlWindow->cursor(FL_CURSOR_MOVE);
      break;
    case VTK_CURSOR_SIZENS:
      this->FlWindow->cursor(FL_CURSOR_NS);
      break;
    case VTK_CURSOR_SIZEWE:
      this->FlWindow->cursor(FL_CURSOR_WE);
      break;
    case VTK_CURSOR_SIZENE:
      this->FlWindow->cursor(FL_CURSOR_NESW);
      break;
    case VTK_CURSOR_SIZENW:
      this->FlWindow->cursor(FL_CURSOR_NWSE);
      break;
    case VTK_CURSOR_SIZESE:
      this->FlWindow->cursor(FL_CURSOR_NWSE);
      break;
    case VTK_CURSOR_SIZESW:
      this->FlWindow->cursor(FL_CURSOR_NESW);
      break;
    } // switch (shape)
}

// ----------------------------------------------------------------------------
void
vtkFLTKOpenGLRenderWindow::PrintSelf (ostream& aTarget, vtkIndent aIndent)
{
  this->Superclass::PrintSelf(aTarget,aIndent);

#ifdef VTK_OPENGL_HAS_OSMESA
  aTarget << aIndent << "OffScreenContextId: "
          << this->Internal->OffScreenContextId << endl;
#endif /* VTK_OPENGL_HAS_OSMESA */

  aTarget << aIndent << "FlParent:           " << (void *) this->FlParent
          << endl;

  aTarget << aIndent << "FlWindow:           " << (void *) this->FlWindow
          << endl;

  aTarget << aIndent << "Mode:               ( "
          << ( (this->Mode & FL_INDEX) ? "FL_INDEX":
               ((this->Mode & FL_RGB8) ? "FL_RGB8" : "FL_RGB") )
          << ((this->Mode & FL_DOUBLE) ? " | FL_DOUBLE" : " | FL_SINGLE")
          << ((this->Mode & FL_ACCUM) ? " | FL_ACCUM" : "")
          << ((this->Mode & FL_ALPHA) ? " | FL_ALPHA" : "")
          << ((this->Mode & FL_DEPTH) ? " | FL_DEPTH" : "")
          << ((this->Mode & FL_STENCIL) ? " | FL_STENCIL" : "")
          << ((this->Mode & FL_MULTISAMPLE) ? " | FL_MULTISAMPLE" : "")
          << ((this->Mode & FL_STEREO) ? " | FL_STEREO" : "")
          << " )" << endl;

  aTarget << aIndent << "OwnFlWindow:        "
          << (this->OwnFlWindow ? "Yes" : "No") << endl;

  aTarget << aIndent << "ScreenSize:         ( " << this->ScreenSize[0]
          << ", " << this->ScreenSize[1] << " )" << endl;

  aTarget << aIndent << "CursorHidden:       "
          << (this->CursorHidden ? "Yes" : "No") << endl;

  aTarget << aIndent << "ForceMakeCurrent:   "
          << (this->ForceMakeCurrent ? "Yes" : "No") << endl;

  aTarget << aIndent << "UsingHardware:      "
          << (this->UsingHardware ? "Yes" : "No") << endl;

  const char* p;

  aTarget << aIndent << "Capabilities:       \n\""
          << ( (p=this->Capabilities)!=0 || (p=this->ReportCapabilities())!=0 ?
               p : "" ) << "\"" << endl;
}

/* 
 * End of: $Id: vtkFLTKOpenGLRenderWindow.cxx,v 1.42 2004/06/16 01:42:51 xpxqx Exp $.
 * 
 */
