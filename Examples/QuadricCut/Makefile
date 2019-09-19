include ../../Makefile.config


MY_CPPFLAGS_VTK = \
        -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
        #


MY_LDFLAGS_FLTK = $(shell fltk-config --use-gl --ldflags)


INCLUDES = -I. -I../.. -I../../src -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-$(VTK_VERSION)


OBJECTS = QuadricCutBase.o QuadricCut.o QuadricCutUI.o


all: QuadricCut.exe

clean:
	$(RM) $(OBJECTS)
	$(RM) QuadricCut.exe 

QuadricCut.exe : $(OBJECTS) ../../build/libvtkFLTK-V8xF14.a
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
	        -lvtkFiltersTexture-$(VTK_VERSION)              \
	#

QuadricCut.o : QuadricCut.cxx  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

QuadricCutBase.o : QuadricCutBase.cxx QuadricCutBase.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)

QuadricCutUI.o : QuadricCutUI.cxx QuadricCutUI.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)
