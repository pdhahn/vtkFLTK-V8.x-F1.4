include ../../Makefile.config


MY_CPPFLAGS_VTK = \
        -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
        #

MY_LDFLAGS_FLTK = $(shell fltk-config --use-gl --ldflags)


INCLUDES = -I. -I../.. -I../../src -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-8.1


OBJECTS = Contour.o  ContourBase.o  ContourUI.o  vtkOrthogonalImagePlaneSet.o


all: Contour.exe

clean:
	$(RM) $(OBJECTS)
	$(RM) Contour.exe 

Contour.exe : $(OBJECTS) ../../build/libvtkFLTK-V8xF14.a
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@  $(OBJECTS)	\
		-L../../build -lvtkFLTK-V8xF14 			\
		$(MY_LDFLAGS_FLTK)				\
		-L$(VTK_DIR)/lib				\
		-Wl,-rpath,$(VTK_DIR)/lib                      	\
		-Wl,-Bdynamic                                   \
	        -lvtkChartsCore-$(VTK_VERSION)                  \
	        -lvtkViewsContext2D-$(VTK_VERSION)              \
	        -lvtkRenderingContext2D-$(VTK_VERSION)          \
	        -lvtkRenderingContextOpenGL2-$(VTK_VERSION)     \
	        -lvtkRenderingLOD-$(VTK_VERSION)     		\
	        -lvtkInteractionStyle-$(VTK_VERSION)            \
	        -lvtkFiltersSources-$(VTK_VERSION)              \
	#

Contour.o : Contour.cxx  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

ContourBase.o : ContourBase.cxx ContourBase.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)

ContourUI.o : ContourUI.cxx ContourUI.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)

vtkOrthogonalImagePlaneSet.o : vtkOrthogonalImagePlaneSet.cxx vtkOrthogonalImagePlaneSet.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)
