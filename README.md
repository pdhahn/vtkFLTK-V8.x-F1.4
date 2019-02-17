Project Name
------------
vtkFLTK-V8.x-F1.4


General
-------
This repository contains code which facilitates the use of VTK 8.x to produce
2D or 3D visualizations in a FLTK 1.4 window (Fl_VTK_Window) that is derived
from Fl_GL_Window. This means you can use a modern VTK version in modern FLTK
user interfaces that employ the usual FLTK control widgets like buttons, etc.

This repository includes code that is either based on or copied from code in
the vtkFLTK-0.6.1 project on SourceForge, copyright (c) 2002 - 2004 Sean
McInerney. The borrowed code has been modified as necessary to port it for use
with FLTK 1.4 and VTK 8.x.

See https://sourceforge.net/projects/vtkfltk.


LICENSE
-------
The original BSD-like license and copyright from vtkFLTK-0.6.1 still applies to
all code based on or copied from that vtkFLTK-0.6.1 project. As for all new code
not in that project, a similar BSD-like license applies.

You must of course also respect and follow the license terms for VTK 8.x and FLTK 1.4
when using code in this project.

See the LICENSE file in this repository for the license terms.


SUPPORTED PLATFORM
------------------
At this time, this implementation only works on Linux, and more specifically,
on CentOS 6 Linux, and compiled with the GNU gcc 5.1 compiler. Unfortunately,
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

Likewise, tthis project does NOT supply a copy of suitable VTK code; you must
download / install it yourself separately, as necessary, for your use with
this project.


PREREQUISITES
-------------

  o Linux CentOS 6

  o GNU g++ compiler version 5.1 toolchain

  o FLTK 1.4 installed / configured for your system

  o VTK 8.x (e.g., 8.1.0) installed / configured for your system

  o GNU make (gmake)


BUILDING
========

After installation of this code and all prerequisites, modify the file
Makefile with the actual paths for your local FLTK and VTK intallation,
as well as your target installation directory.

Then simply type "make" in the top level project directory. This will
compile source files and create a library in a subdirectory named "build".

If all goes well, type "make examples".

If all still goes well, type "make install" to copy releavnt files into
your target installation directory.


DISCLAIMER
==========

The code in this project is being supplied with the hope that it might be
helpful to those who might want to try to use VTK with FLTK in EXPERIMETAL
CIRCUMSTANCES ONLY.

I have not tried to debug nor test any and all possibilities. However, most
of the the sample code in "Examples" appears to work properly (or good enough
for my own purposes, anyway). I have also not tried to deal with performance
issues, if any.

IMPORTANT: I cannot be held liable for any negative or adverse effects that
might result from building, installing or using this code. IT IS PROVIDED IN
AN EXPERIMENTAL STATE ONLY, FOR USE AT YOUR OWN RISK.

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



