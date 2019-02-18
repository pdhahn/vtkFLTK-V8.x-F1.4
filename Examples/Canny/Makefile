include ../../Makefile.config


MY_CPPFLAGS_VTK = \
        -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
        #


MY_LDFLAGS_FLTK = $(shell fltk-config --use-gl --use-images --ldflags)


INCLUDES = -I. -I../.. -I../../src -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-8.1


OBJECTS = Canny.o  CannyBase.o  CannyUI.o


all: Canny.exe

clean:
	$(RM) $(OBJECTS)
	$(RM) Canny.exe 

Canny.exe : $(OBJECTS) ../../build/libvtkFLTK-V8xF14.a
	$(CXX) $(CXXFLAGS) $(CXX_OPTS)  -o $@  $(OBJECTS)	\
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
	        -lvtkImagingMorphological-$(VTK_VERSION)        \
	        -lvtkImagingMath-$(VTK_VERSION)        		\
	        -lvtkIOLegacy-$(VTK_VERSION)        		\
	#

Canny.o : Canny.cxx  Makefile
	g++ -std=gnu++11 -g -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

CannyBase.o : CannyBase.cxx CannyBase.h  Makefile
	g++ -std=gnu++11 -g -o $@ -c $< $(INCLUDES)

CannyUI.o : CannyUI.cxx CannyUI.h  Makefile
	g++ -std=gnu++11 -g -o $@ -c $< $(INCLUDES)
