/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: Fl_VTK_Window.H,v 1.22 2004/06/15 18:40:25 xpxqx Exp $
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
#ifndef Fl_VTK_WINDOW_H_
#  define Fl_VTK_WINDOW_H_
#  include "vtkFLTKConfigure.h"
// FLTK
#  include <FL/Fl_Gl_Window.H>

// hack alert!!! ... for VTK event handler callback (not currently used)
// all subclasses have type() >= this
#  define VTK_WINDOW_TYPE       (FL_WINDOW+2)
#  define VTK_WINDOW_XCLASS     "vtkfltk, Vtkfltk" 

class vtkProp;
class vtkCamera;
class vtkRenderer;
class vtkRendererCollection;
class vtkRenderWindowInteractor;
class vtkRenderWindow;
class vtkObjectFactory;

/** \class   Fl_VTK_Window
 *  \brief   FLTK event-driven interface to a RenderWindow.
 * 
 * FLTK user interface window peer class to a VTK rendering classes with 
 * additional methods for integrating a FLTK event-driven interface to a VTK 
 * pipeline. Fl_VTK_Window provides methods for tracking modification time, 
 * debugging, printing, and handling events. Fl_VTK_Window also performs 
 * reference counting: objects that are reference counted exist as long as 
 * another object uses them. Once the last reference to a reference counted 
 * object is removed, the object will spontaneously destruct. 
 * 
 * \author  Sean McInerney
 * \version $Revision: 1.22 $
 * \date    $Date: 2004/06/15 18:40:25 $
 * 
 * \sa
 * vtkRenderWindowInteractor vtkRenderWindow Fl_Gl_Window
 */

class VTK_FLTK_EXPORT Fl_VTK_Window : public Fl_Gl_Window
{
  // constructor internals
  void Fl_VTK_Window_ (vtkRenderWindowInteractor* = 0);
  void Fl_VTK_Window_ (vtkRenderWindow*);
  static vtkObjectFactory* Factory;

protected:
  // Fl_Gl_Window overrides
  virtual void flush (void);
  virtual void draw (void);
  /** Handle an event (e.g., mouse move, etc.). **/
  virtual int  handle (int event);

  friend class vtkFLTKRenderWindow;
  friend class vtkFLTKOpenGLRenderWindow;
  friend class vtkFLTKRenderWindowInteractor;

  /** Resize the FLTK window by calling ancestor method. */
  virtual void resize_ (int x, int y, int w, int h)
    { this->Fl_Gl_Window::resize(x, y, w, h); }

public:
  /** Change the size or position of the window. */
  virtual void resize (int x, int y, int w, int h);

  /** Make the displayed window contents current. */
  void Update (void);

  /*@{*/
  /** Set/Get the Interactor. */
  virtual void SetInteractor (vtkRenderWindowInteractor*);
  virtual vtkRenderWindowInteractor* GetInteractor (void);
  /*@}*/

  /*@{*/
  /** Set/Get the RenderWindow. */
  virtual void SetRenderWindow (vtkRenderWindow*);
  virtual vtkRenderWindow* GetRenderWindow (void);
  /*@}*/

  /** Return the Collection of Renderers in the RenderWindow. */
  virtual vtkRendererCollection* GetRenderers (void);

  /** Get the first Renderer held in the RenderWindow's Collection. */
  virtual vtkRenderer* GetDefaultRenderer (void);

  /** Get the active Camera held by the default Renderer. */
  virtual vtkCamera* GetDefaultCamera (void);

  /** Add a Prop to the current Renderer. */
  virtual void AddProp (vtkProp* prop);

  /** Remove a Prop from the current Renderer. */
  virtual void RemoveProp (vtkProp* prop);

public:
  Fl_VTK_Window (int w = 300, int h = 300, const char* label = "");
  Fl_VTK_Window (int x, int y, int w, int h, const char* label = "");
  Fl_VTK_Window (vtkRenderWindowInteractor* o,
                 int w = 300, int h = 300, const char* label = "");
  Fl_VTK_Window (vtkRenderWindowInteractor* o,
                 int x, int y, int w, int h, const char* label = "");

  ~Fl_VTK_Window();

  /** Get this classes name as a string. */
  virtual const char* GetClassName (void) { return "Fl_VTK_Window"; }

  /*@{*/
  /** Get/Set the debugging toggle. */
  void SetDebug (int a) { this->Debug = a; }
  int  GetDebug (void) const { return this->Debug; }
  void DebugOn (void) { this->SetDebug(1); }
  void DebugOff (void) { this->SetDebug(0); }
  /*@}*/

  /** Get a string describing an event enumeration. */
  static const char* GetEventAsString (int event);

  /** Returns a mode given the desired parameters. */
  static int    desired_mode (int& doubleBuffer, int& stereo, int& multiSample);

  /** Set whether Fl_GL_Window::redraw() should be called after a
      mouse event is handled (MOVE, DRAG, or WHEEL). The initial
      default is false. Alternately, you could simply call redraw()
      yourself, in a handle() override method in your subclass. **/
  virtual int  SetRedrawAfterMouseEvents (int yesno);
  int          GetRedrawAfterMouseEvents (void) const { return RedrawAfterMouseEvents; }
  
protected:
  Fl_VTK_Window (vtkRenderWindow* o,
                 int w = 300, int h = 300, const char* label = "");
  Fl_VTK_Window (vtkRenderWindow* o,
                 int x, int y, int w, int h, const char* label = "");

  vtkRenderWindowInteractor*    Interactor;
  vtkRenderWindow*              RenderWindow;

  vtkRenderer*                  QueuedRenderer;

  int RedrawAfterMouseEvents;

  int Debug;

  static void ClassInitialize (void);
  static void ClassFinalize (void);

  friend vtkObjectFactory* FlVtkWindowCheckForVtkFLTKObjectFactory (void);

public:
  class Init;
  friend class Init;
  class Init
  {
    friend class Fl_VTK_Window;
  public:
    Init (void);
    ~Init();

  private:
    static long TranslationUnits;
  };

private:
  Fl_VTK_Window (const Fl_VTK_Window&);  // Not implemented.
  void operator= (const Fl_VTK_Window&);  // Not implemented.
};

/** \internal
 * Global initializer object, to ensure construction of static objects.
 */
static Fl_VTK_Window::Init FlVtkWindowInitInstance;

#endif /* Fl_VTK_WINDOW_H_ */
/* 
 * End of: $Id: Fl_VTK_Window.H,v 1.22 2004/06/15 18:40:25 xpxqx Exp $.
 * 
 */
