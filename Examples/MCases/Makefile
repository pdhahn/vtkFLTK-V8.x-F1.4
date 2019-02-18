include ../../Makefile.config


#
# We only need these flags
#
MY_CPPFLAGS_VTK = \
        -DvtkRenderingCore_AUTOINIT='3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL2)' \
        #


MY_LDFLAGS_FLTK = $(shell fltk-config --use-gl --ldflags)


INCLUDES = -I. -I../.. -I../../src -I$(FLTK_DIR)/include  -I$(VTK_DIR)/include/vtk-8.1


OBJECTS = MCasesBase.o MCases.o MCasesUI.o



all: MCases.exe

clean:
	$(RM) $(OBJECTS)
	$(RM) MCases.exe 

MCases.exe : $(OBJECTS) ../../build/libvtkFLTK-V8xF14.a
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ $(OBJECTS)		\
		-L../../build -lvtkFLTK-V8xF14 			\
		$(MY_LDFLAGS_FLTK)				\
		-L$(VTK_DIR)/lib				\
		-Wl,-rpath,$(VTK_DIR)/lib                      	\
		-Wl,-Bdynamic                                   \
	        -lvtkChartsCore-$(VTK_VERSION)                  \
	        -lvtkViewsContext2D-$(VTK_VERSION)              \
	        -lvtkRenderingContext2D-$(VTK_VERSION)          \
	        -lvtkRenderingContextOpenGL2-$(VTK_VERSION)     \
	        -lvtkInteractionStyle-$(VTK_VERSION)            \
	        -lvtkFiltersSources-$(VTK_VERSION)              \
	#

MCases.o : MCases.cxx  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES) $(MY_CPPFLAGS_VTK)

MCasesBase.o : MCasesBase.cxx MCasesBase.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)

MCasesUI.o : MCasesUI.cxx MCasesUI.h  Makefile
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) -o $@ -c $< $(INCLUDES)

