include ../../Makefile.config


MY_CPPFLAGS_VTK = \
        -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
        #

MY_LDFLAGS_FLTK = $(shell fltk-config --use-gl --ldflags)


INCLUDES = -I. -I../.. -I../../src -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-8.1



all: Simple2.exe

clean:
	$(RM) Simple2.o Simple2.exe 

Simple2.exe : Simple2.o ../../build/libvtkFLTK-V8xF14.a
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ $<			\
		-L../../build -lvtkFLTK-V8xF14 			\
		$(MY_LDFLAGS_FLTK)				\
		-L$(VTK_DIR)/lib				\
		-Wl,-rpath,$(VTK_DIR)/lib                      	\
		-Wl,-Bdynamic                                   \
	        -lvtkChartsCore-$(VTK_VERSION)                  \
	        -lvtkRenderingContextOpenGL2-$(VTK_VERSION)     \
	        -lvtkInteractionStyle-$(VTK_VERSION)            \
	        -lvtkFiltersSources-$(VTK_VERSION)              \
	#

Simple2.o : Simple2.cxx  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

