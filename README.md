PROJECT NAME
------------
vtkFLTK-V8.x-F1.4


AUTHORS
-------
The author of the original vtkFLTK project software is Sean McInerney.

See https://sourceforge.net/projects/vtkfltk and http://vtkfltk.sourceforge.net
for more information about that original vtkFLTK project.

I (Paul Douglas Hahn, CompIntense HPC, LLC) have made certain modifications to
that original code, plus I have written some new code as well, for inclusion in
the current project repository.


GENERAL
-------
This repository contains "bridge" software designed to make possible the use of
VTK 8.x to produce 2D or 3D visualizations in a FLTK 1.4 window (Fl_VTK_Window)
that is derived from a Fl_GL_Window. This means you can use a fairly modern VTK
version in modern FLTK user interfaces that employ the usual FLTK control widgets
like buttons, etc.

This repository includes code that is based on source code copied from the
vtkFLTK-0.6.1 project on SourceForge, copyright (c) 2002 - 2004 Sean McInerney.
To various extents, the borrowed code has been modified and extensions made as
necessary to port it for use with FLTK 1.4 and VTK 8.x. Nevertheless, the original
code still provides the core functionality, and I congratulate the original author
for his very good work. It solves a big problem in my world: namely, how to use
VTK with a free GUI toolkit (FLTK) that has terms of use friendly to commercial
application development.


LICENSE
-------
The original BSD-like license and copyright from vtkFLTK-0.6.1 still applies to
all code based on or copied from that vtkFLTK-0.6.1 project. As for all new code
not in that project, a similar BSD-like license applies.

You must of course also respect and follow the license terms for VTK 8.x and FLTK 1.4
when using code in this project.

See the LICENSE file in this repository for the license terms.


PLATFORM(S)
-----------
At this time, this implementation only works on Linux, and more specifically,
on CentOS 6 Linux, and compiled with the GNU g++ 5.1 compiler. Unfortunately,
I do not have time right now to extend the range of OS and toolchain support,
but in my opinion, doing so should be straightforward (at least, on Linux).

Moreover, I have supplied simple Makefile's that are oriented for this
specific configuration, rather than trying to deal with cmake and its
obfuscations.

I may be able to get around to a more portable implementation sometime in the
future, or someone may wish to contribute in this area with a pull request.

Note this project does NOT supply a copy of suitable FLTK code; you must
download / install it yourself separately, as necessary, for your use with
this project.

Likewise, this project does NOT supply a copy of suitable VTK code; you must
download / install it yourself separately, as necessary, for your use with
this project.


PREREQUISITES
-------------
  * Linux CentOS 6  (or probably most other flavors of Linux)

  * GNU g++ compiler version 5.1 toolchain  (or probably later versions;
    likely best to use same toolchain that was used to build your FLTK and
    VTK packages)

  * FLTK 1.4 installed / configured for your system

  * VTK 8.x (e.g., 8.1.0) installed / configured for your system

  * GNU make (gmake)


BUILDING
--------
After installation of this code and all prerequisites, modify "Makefile.config"
in the top-level project directory with the actual paths for your local FLTK
and VTK installation, as well as your target installation directory.

Then simply type "make" in the top-level project directory. This will compile
library source files and create the static library "libvtkFLTK-V8xF14.a" in
a holding subdirectory named "build".

If all goes well, type "make install" to copy the library and relevant header
files into your target installation directory (i.e., under "lib" and "include",
resp.).

To build the examples, type "make examples". Note that the compiled programs
after this step are located in the various subdirectories under the "Examples"
subdirectory, not in the "build" subdirectory.

If all still goes well, you can do "make install-examples" to copy the example
programs into your target installation directory (i.e., under "bin").

Other valid make targets include "clean", "clean-examples", and "clean-install".
The latter removes all files previously copied into your target installation
directory, including any example programs.


PROGRAMMING NOTES
-----------------
1. The event handler method `Fl_VTK_Window::handle()` has been augmented to
support an optional internal call to `Fl_GL_Window::redraw()` after mouse
events (e.g., DRAG, MOVE, or MOUSEWHEEL). This can be useful, for example,
with VTK displays based on scene graphs containing context items (e.g.,
derived from vtkAbstractContextItem), such as the VTK chart classes vtkChartXY,
vtkChartMatrix, etc., especially in conjunction with other scene items
like crosshairs.

   You can enable / disable this feature via the new Fl_VTK_Window public method
`SetRedrawAfterMouseEvents(int)`, and check it using `GetRedrawAfterMouseEvents()`.
The initial / default setting is disabled.

2) Examine the Makefile in each example directory to see how to compile and
link. The definition and use of the `MY_CPPFLAGS_VTK` make variable there is a
way to get VTK 8.x to utilize certain required internal modules.

        MY_CPPFLAGS_VTK = \
            -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
            #
        ...
        $(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

   In addition, in each example, there is one .cxx source file containing these two lines:

        #include "vtkAutoInit.h"
        VTK_MODULE_INIT(vtkFLTKOpenGLRendering);

   This is necessary and causes VTK to utilize the vtkFLTK library bridge code that supports
   Fl_VTK_Window.

   It might also be possible to use a VTK initialization approach based solely on `VTK_MODULE_INIT`
   instead of the `MY_CPPFLAGS_VTK` thing in the Makefile. Namely, in the .cxx source file, try
   adding the following three lines after the `#include "vtkAutoInit.h"`in lieu of using
   `MY_CPPFLAGS_VTK` in the Makefile:

       VTK_MODULE_INIT(vtkInteractionStyle);
       VTK_MODULE_INIT(vtkRenderingFreeType);
       VTK_MODULE_INIT(vtkRenderingOpenGL2);

3) Not all examples from the original vtkFLTK project have been copied into
this repository. In particular, the "Volume" example has been excluded because
of the porting challenge (for me) w.r.t. VTK 8.x.

   Note the "Canny" example seems to have ported OK, but crashes when a .png image
file is loaded.

   The "Contour" example requires that the environment variable `VTK_DATA_ROOT` be defined as
   the path to standard VTK example data (e.g., download via git clone of git://vtk.org/VTKData.git).


DISCLAIMER
----------
The software in this project is being supplied with the hope that it might be
helpful to those who might want to try to use VTK with FLTK in EXPERIMENTAL
CIRCUMSTANCES ONLY.

I have not tried to debug or test all the possibilities. However, much of
the sample code in "Examples" appears to work properly (or good enough for
my own purposes, anyway).

IMPORTANT: I cannot be held liable for any negative or adverse effects that
might result from building, installing or using this software. IT IS PROVIDED
IN AN EXPERIMENTAL STATE ONLY, FOR USE AT YOUR OWN RISK.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



