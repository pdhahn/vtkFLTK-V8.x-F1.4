// VTK
#  include "vtkIndent.h"
#  include "vtkSystemIncludes.h"
#  include "vtkSetGet.h"
#  include "vtkTimeStamp.h"
// FLTK
#  include <FL/Enumerations.H>

#  if defined(__cplusplus)
#    include "vtkConfigure.h"
#  endif /* __cplusplus */


#    define VTK_FLTK_STATIC

#    define VTK_FLTK_EXPORT
#    define VTK_FLTK_EXTERN(_type) extern _type
#    define VTK_FLTK_IMPLEMENT(_type) _type


#if defined( NDEBUG )  ||  VTK_FLTK_BUILD_DEBUG == 0
#  define vtkFLTKDebugMacro(m)
#else
#  define vtkFLTKDebugMacro(m)  ( cout m << endl )
#endif
