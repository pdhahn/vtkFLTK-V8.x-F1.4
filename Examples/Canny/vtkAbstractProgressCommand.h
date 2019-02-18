/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: vtkAbstractProgressCommand.h,v 1.2 2004/05/13 18:37:48 xpxqx Exp $
 *
 * Copyright (c) 2002 - 2004 Sean McInerney
 * All rights reserved.
 *
 * See Copyright.txt or http://vtkfltk.sourceforge.net/Copyright.html
 * for details.
 *
 *    This software is distributed WITHOUT ANY WARRANTY; without even 
 *    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 *    PURPOSE.  See the above copyright notice for more information.
 *
 */
#ifndef VTK_ABSTRACT_PROGRESS_COMMAND_H_
#  define VTK_ABSTRACT_PROGRESS_COMMAND_H_
// VTK Common
#  include "vtkCommand.h"
#  include "vtkAlgorithm.h"

class VTK_EXPORT vtkAbstractProgressCommand : public vtkCommand
{
protected:
  vtkAbstractProgressCommand (void) : Progress(0) { this->Buffer[0] = '\0'; }

  void Update (vtkObject* aCaller)
    {
      vtkAlgorithm* source;

      if ((source = vtkAlgorithm::SafeDownCast(aCaller)) == NULL)
        {
        this->Buffer[0] = '\0';
        return;
        }

      this->Progress  = source->GetProgress();

      const char* className = source->GetClassName();
      int         length    = strlen(className) + 3 + 3 + 5 + 1;

      (void) sprintf( this->Buffer, "%s: %3d%% done",
                      (length<128 ? className : "source"),
                      int(ceil(this->Progress * 100)) );
    }

  double        Progress;
  char          Buffer[128];

private:
  vtkAbstractProgressCommand (const vtkAbstractProgressCommand&);
  void operator= (const vtkAbstractProgressCommand&);
};
#endif /* VTK_ABSTRACT_PROGRESS_COMMAND_H_ */
/*
 * End of: $Id: vtkAbstractProgressCommand.h,v 1.2 2004/05/13 18:37:48 xpxqx Exp $.
 *
 */
