                            vtkFLTK-V8.x-F1.4
                            =================

General
=======
This repository contains code which facilitates the use of VTK 8.x to produce
2D or 3D visualizations in a FLTK 1.4 window (Fl_VTK_Window) that is derived
from Fl_GL_Window. This means you can use a modern VTK version in modern FLTK
user interfaces that employ the usual FLTK control widgets like buttons, etc.

This repository includes code that is either based on or copied from code in
the vtkFLTK-0.6.31 project on SourceForge, copyright (c) 2002 - 2004 Sean
McInerney. The borrowed code has been modified as necessary to port it for use
with FLTK 1.4 and VTK 8.x.


LICENSE
=======
Of course, the original BSD-like license and copyright from vtkFLTK-0.6.31
still applies to all code based on or copied from that vtkFLTK-0.6.31 project.
As for all new code not in that project, a similar BSD-like license applies.
See the LICENSE file in this repository for the applicable text.


SUPPORTED PLATFORM
==================
At this time, this implementation only works on Linux, and more specifically,
on CentOS 6 Linux, and compiled with the GNU gcc 5.1 compiler. Unfortunately,
I do not have time right now to extend the range of OS and toolchain support,
but in my opinion, doing so should be straightforward (at least, on Linux).

Moreover, I have supplied simple Makefile's that are oriented for this
specific configuration, rather than trying to deal with cmake and its
obfuscations.

I may be able to get around to a more portable implementation sometime in the
future, or someone may wish to contribute in this area with a pull request.



