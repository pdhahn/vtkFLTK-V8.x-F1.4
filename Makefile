#
# Copyright (c) 2019 Paul Douglas Hahn, CompIntense HPC, LLC
# All rights reserved.
#
#
# See LICENSE file for details.
#
#    This software is distributed WITHOUT ANY WARRANTY; without even 
#    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
#    PURPOSE.  See the above LICENSE notice for more information.
#
#
# Jesu Hilf Mir!
#
#
#
####################################################################
#
# Modify definitions in "Makefile.config" to match your situation.
#
####################################################################

include ./Makefile.config


####################################################################
#
# You probably don't need to modify anything past this point.
#
####################################################################

LIBRARY = libvtkFLTK-V8xF14.a

INCLUDES = -I. -Isrc -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-$(VTK_VERSION)

OBJECTS = \
	Fl_VTK_Window.o				\
	\
	vtkFLTKObjectFactory.o			\
	vtkFLTKOpenGLRenderWindow.o		\
	vtkFLTKRenderWindowInteractor.o		\
	vtkFLTKInteractorStyleSwitch.o		\
	vtkFLTKInteractorStyleJoystickCamera.o	\
	#


EXAMPLES = Cone4 Contour QuadricCut MCases MorningStar Simple Simple2


__LIBRARY = build/$(LIBRARY)


__OBJECTS = $(OBJECTS:%.o=build/%.o)


__INSTALL_HEADER_FILES  = src/Fl_VTK_Window.H  vtkFLTKConfigure.h
__INSTALL_LIB_FILES     = $(__LIBRARY)
__INSTALL_EXAMPLE_FILES = $(EXAMPLES:%=Examples/%/*.exe)


CXX ?= g++
AR  ?= ar
RM  ?= rm
CP  ?= cp


.PHONY: all examples install install-examples clean clean-examples clean-all



all: $(__LIBRARY)

examples: $(__LIBRARY)
	@						\
	if [ ! -d ./Examples ]; then exit 0; fi;	\
	cd ./Examples;					\
	for dir in $(EXAMPLES); do			\
	    if [ ! -f $$dir/Makefile ]; then continue; fi;  \
	    echo "";					\
	    pushd $$dir >& /dev/null;			\
	    make;					\
	    popd >& /dev/null;				\
	done;						\
	#

install: $(__LIBRARY)
	@						\
	mkdir -p $(INSTALL_PATH)/include;		\
	chmod a+rwx $(INSTALL_PATH)/include;		\
	mkdir -p $(INSTALL_PATH)/lib;			\
	chmod a+rwx $(INSTALL_PATH)/lib;		\
	#
	$(CP) -pu $(__INSTALL_HEADER_FILES)		\
		$(INSTALL_PATH)/include
	$(CP) -pu $(__INSTALL_LIB_FILES)		\
		$(INSTALL_PATH)/lib

install-examples: examples install
	@						\
	if [ ! -d ./Examples ]; then exit 0; fi;	\
	mkdir -p  $(INSTALL_PATH)/bin;			\
	chmod a+rwx $(INSTALL_PATH)/bin;		\
	#
	@						\
	for fil in $(__INSTALL_EXAMPLE_FILES); do	\
	    if [ ! -f $$fil ]; then continue; fi;  	\
	    echo "$(CP) -pu $$fil $(INSTALL_PATH)/bin";	\
	    $(CP) -pu $$fil $(INSTALL_PATH)/bin;	\
	done;						\
	#

clean:
	$(RM) $(__LIBRARY) $(__OBJECTS)

clean-examples:
	@						\
	if [ ! -d ./Examples ]; then exit 0; fi;	\
	cd ./Examples;					\
	for dir in $(EXAMPLES); do			\
	    if [ ! -f $$dir/Makefile ]; then continue; fi;  \
	    echo "";					\
	    pushd $$dir >& /dev/null;			\
	    make clean;					\
	    popd >& /dev/null;				\
	done;						\
	#

clean-all: clean clean-examples

clean-install:
	@						\
	if [ ! -d $(INSTALL_PATH) ]; then exit 0; fi;	\
	echo "$(RM) $(INSTALL_PATH)/include/*";		\
	if [ -d $(INSTALL_PATH)/include ]; then		\
	    $(RM) $(INSTALL_PATH)/include/*;		\
	fi;						\
	echo "$(RM) $(INSTALL_PATH)/lib/*";		\
	if [ -d $(INSTALL_PATH)/lib ]; then		\
	    $(RM) $(INSTALL_PATH)/lib/*;		\
	fi;						\
	echo "$(RM) $(INSTALL_PATH)/bin/*";		\
	if [ -d $(INSTALL_PATH)/bin ]; then		\
	    $(RM) $(INSTALL_PATH)/bin/*;		\
	fi;						\
	#


$(__LIBRARY): $(__OBJECTS) Makefile
	@mkdir -p build
	@chmod a+wrx build
	@$(RM) $@
	$(AR) qv $@ $(__OBJECTS)
	@ranlib $@


build/Fl_VTK_Window.o : src/Fl_VTK_Window.cxx src/Fl_VTK_Window.H vtkFLTKConfigure.h
	@mkdir -p build
	@chmod a+wrx build
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) $(INCLUDES) -o $@ -c $<

build/vtkFLTK%.o : src/vtkFLTK%.cxx src/vtkFLTK%.h vtkFLTKConfigure.h
	@mkdir -p build
	@chmod a+wrx build
	$(CXX) $(CXXFLAGS) $(CXX_OPTgS) $(INCLUDES) -o $@ -c $<



#
# Soli Deo Gloria!
#
