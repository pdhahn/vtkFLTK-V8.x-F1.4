/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: $
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
 *    PURPOSE.  See the above LICENSE notice for more information.
 *
 */
#ifndef VTK_FLTK_OPENGL_RENDER_WINDOW_H_
#  define VTK_FLTK_OPENGL_RENDER_WINDOW_H_
#  include "vtkFLTKConfigure.h"
// VTK Rendering
#  include "vtkOpenGLRenderWindow.h"

//BTX
class Fl_Group;
class Fl_VTK_Window;
//ETX

class vtkFLTKOpenGLRenderWindowInternal;

/** \class   vtkFLTKOpenGLRenderWindow
 *  \brief   OpenGL rendering window for FLTK interface.
 * 
 * vtkFLTKOpenGLRenderWindow is a concrete implementation of the abstract
 * class vtkRenderWindow and the vtkOpenGLRenderer interfaces to the
 * OpenGL graphics library. Application programmers should normally use
 * vtkRenderWindow instead of the FLTK-OpenGL specific version.
 * 
 * \author  Sean McInerney
 * \version $Revision: 1.22 $
 * \date    $Date: 2004/06/15 18:40:26 $
 * 
 * \sa
 * vtkOpenGLRenderWindow Fl_VTK_Window Fl_Group
 */

class VTK_FLTK_EXPORT vtkFLTKOpenGLRenderWindow : public vtkOpenGLRenderWindow
{
public:
  virtual bool IsCurrent();
  virtual void CreateAWindow();
  virtual void DestroyWindow();

public:
  static vtkFLTKOpenGLRenderWindow* New (void);
  vtkTypeMacro (vtkFLTKOpenGLRenderWindow, vtkOpenGLRenderWindow);
  void PrintSelf (ostream&, vtkIndent);

  /** Begin the rendering process. */
  virtual void  Start (void);

  /** End the rendering process and display the image. */
  virtual void  Frame (void);

  /** Initialize the window for rendering. */
  virtual void  WindowInitialize (void);

  /** Initialize the rendering window.
   *
   * This will setup all system-specific resources. This method and
   * \c Finalize() must be symmetric and it should be possible to call
   * them multiple times, even changing the underlying window Id
   * in-between. This is what \c WindowRemap() does.
   */
  virtual void  Initialize (void);

  /** "Deinitialize" the rendering window.
   *
   * This will shutdown all system-specific resources. After having
   * called this, it should be possible to destroy a window that was
   * used for a SetWindow() call without any ill effects.
   */
  virtual void  Finalize (void);

  /** Remap the rendering window.
   *
   * This is useful for changing properties that can't normally be
   * changed once the window is up.
   */
  virtual void  WindowRemap (void);

  /** Check to see if a mouse button has been pressed.
   *
   * All other events are ignored by this method.
   * This is a useful check to abort a long render.
   *
   * Ideally, you want to abort the render on any event which causes
   * the \c DesiredUpdateRate to switch from a high-quality rate to a
   * more interactive rate.
   */
  virtual int   GetEventPending (void);

  /** Make this window the current OpenGL context. */
  void          MakeCurrent (void);

  /** If called, allow MakeCurrent() to skip cache-check when called.
   * MakeCurrent() reverts to original behavior of cache-checking
   * on the next render.
   */
  void          SetForceMakeCurrent (void);

  /** Updates window size before calling the superclass \c Render(). */
  void          Render (void);

  /** Get the size of the screen in pixels. */
  int*          GetScreenSize (void);

  /** Get the position in screen coordinates (pixels) of the window. */
  int*          GetPosition (void);

  /*@{*/
  /** Move the window to a new position on the display. */
  void          SetPosition (int x, int y);
  void          SetPosition (int a[2]) {this->SetPosition(a[0], a[1]);}
  /*@}*/

  /** Get the width and height in screen coordinates (pixels) of the window. */
  int*          GetSize (void);

  /*@{*/
  /** Specify the size of the rendering window. */
  void          SetSize (int w, int h);
  void          SetSize (int a[2]) {this->SetSize(a[0], a[1]);}
  /*@}*/

  /*@{*/
  /** Keep track of whether the rendering window has been mapped to screen. */
  void          SetMapped (int a);
  int           GetMapped (void);
  /*@}*/

  /** Set the name of the window. This normally appears at top of the window. */
  void          SetWindowName (const char* name);

  /** Render without displaying the window. */
  void          SetOffScreenRendering (int toggle);

  /*@{*/
  /** Hide or Show the mouse cursor.
   *
   * It is nice to be able to hide the default cursor if you want VTK 
   * to display a 3D cursor instead.
   *
   * \note Set cursor position in window (note that (0,0) is the lower left 
   *       corner).
   */
  void          HideCursor (void);
  void          ShowCursor (void);
  /*@}*/

  /** Change the shape of the cursor. */
  virtual void  SetCurrentCursor (int shape);

  /** Change the window to fill the entire screen. */
  void          SetFullScreen (int);

  /** Set the preferred window size to full screen. */
  void          PrefFullScreen (void);

  /** Toggle whether the window manager border is around the window.
   *
   * The default value is true. Under most X window managers
   * this does not work after the window has been mapped.
   */
  void          SetBorders (int a);

  /** Toggle whether the window will be created in a stereo-capable mode.
   *
   * This method must be called before the window is realized. This method
   * overrides the superclass method since this class can actually check
   * whether the window has been realized yet.
   */
  void          SetStereoCapableWindow (int a);

  /** Get the properties of an ideal rendering window. */
  virtual int   GetDesiredVisualMode (void);

  /** Get report of capabilities for the render window. */
  const char*   ReportCapabilities (void);

  /** Does this render window support OpenGL? 0-false, 1-true. */
  int           SupportsOpenGL (void);

  /** Is this render window using hardware acceleration? 0-false, 1-true. */
  int           IsDirect (void);

  //BTX
  /** Get this RenderWindow's parent FLTK group (if any). */
  Fl_Group*     GetFlParent (void);

  /** Sets the FLTK parent of the window that WILL BE created. */
  void          SetFlParent (Fl_Group* group);
  //ETX

  /** Sets the FLTK parent of the window that WILL BE created (dangerously). */
  void          SetFlParent (void* group);

  //BTX
  /** Get this RenderWindow's FLTK window. */
  Fl_VTK_Window* GetFlWindow (void);

  /** Set this RenderWindow to an existing FLTK window. */
  void          SetFlWindow (Fl_VTK_Window* window);
  //ETX

  /** Set this RenderWindow to an existing FLTK window (dangerously). */
  void          SetFlWindow (void* window);

  /*@{*/
  /**
   * Implementation of \c vtkWindow's system independent methods that are
   * used to help interface to native windowing systems.
   *
   * \note
   * These methods can only be used to set \c Fl_Window subclasses
   * as parent since an \c Fl_Group is never associated with an XID.
   */
  void  SetDisplayId (void* id) override;
  void  SetWindowId (void* id) override;
  void  SetParentId (void* id) override;
  void* GetGenericDisplayId (void) override;
  void* GetGenericWindowId (void) override;
  void* GetGenericParentId (void) override;
  void* GetGenericContext (void) override;
  void* GetGenericDrawable (void) override;
  void  SetDisplayInfo (char* id);
#if VTK_MINOR_VERSION > 1
  void  SetWindowInfo (const char* id) override;
  void  SetParentInfo (const char* id) override;
#else
  void  SetWindowInfo (char* id) override;
  void  SetParentInfo (char* id) override;
#endif
  void  SetNextWindowId (void* id) override;
#if VTK_MINOR_VERSION > 1
  void  SetNextWindowInfo (const char* id) override;
#else
  void  SetNextWindowInfo (char* id) override;
#endif
  /*@}*/

protected:
  vtkFLTKOpenGLRenderWindow (void);
  ~vtkFLTKOpenGLRenderWindow();

  //BTX
  vtkFLTKOpenGLRenderWindowInternal* Internal;

  Fl_Group*             FlParent;
  Fl_VTK_Window*        FlWindow;

  int                   Mode; // FLTK OpenGL window capabilities
  int                   OwnFlWindow;
  int                   ScreenSize[2];
  int                   CursorHidden;
  int                   ForceMakeCurrent;
  int                   UsingHardware;
  char*                 Capabilities;
  //ETX

  /** Set this RenderWindow to use fl_display, maybe opening the display.*/
  virtual void  CheckDisplayConnection();

private:
  vtkFLTKOpenGLRenderWindow (const vtkFLTKOpenGLRenderWindow&);  // Not implemented.
  void operator= (const vtkFLTKOpenGLRenderWindow&);  // Not implemented.
};

#endif /* VTK_FLTK_OPENGL_RENDER_WINDOW_H_ */
/* 
 * End of: $Id: vtkFLTKOpenGLRenderWindow.h,v 1.22 2004/06/15 18:40:26 xpxqx Exp $.
 * 
 */
