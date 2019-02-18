/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * $Id: MCasesBase.cxx,v 1.8 2004/05/13 18:37:49 xpxqx Exp $
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
#include "MCasesBase.h"
// C++ STL
#include <string>
#include <sstream>
// VTK Common
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkUnstructuredGrid.h"
// VTK Graphics
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkContourFilter.h"
#include "vtkTubeFilter.h"
#include "vtkExtractEdges.h"
#include "vtkShrinkPolyData.h"
#include "vtkThresholdPoints.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkGlyph3D.h"
// VTK Rendering
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
// VTK Hybrid
#include "vtkDepthSortPolyData.h"
#include "vtkVectorText.h"
// FLTK
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
// vtkFLTK
#include "Fl_VTK_Window.H"

// ----------------------------------------------------------------------------
//      M C a s e s B a s e
// ----------------------------------------------------------------------------
//vtkCxxRevisionMacro (MCasesBase, "$Revision: 1.8 $");

// ----------------------------------------------------------------------------
float lamp_black[] = { 0.180f, 0.280f, 0.230f };
float banana[]     = { 0.890f, 0.810f, 0.340f };
float khaki[]      = { 0.941f, 0.902f, 0.549f };
float tomato[]     = { 1.000f, 0.388f, 0.278f };

class MCasesInternal
{
public:
  vtkRenderer*          Renderer;
  unsigned char         Case;

  std::string           CaseName;

  vtkFloatArray*        CubeScalars;
  vtkPoints*            CubePoints;
  vtkIdList*            CubeIdList;
  vtkUnstructuredGrid*  CubeGrid;

  vtkContourFilter*     ContourFilter;

  vtkActor*             TriangleEdgeActor;
  vtkActor*             ShrinkActor;
  vtkActor*             CubeEdgeActor;
  vtkActor*             CubeVertexActor;
  vtkVectorText*        CaseLabel;
  vtkActor*             LabelActor;
  vtkActor*             BaseActor;

  MCasesInternal (void)
    : Renderer(NULL),
      Case(9),
      CaseName("Case 9 - 00001001"),
      CubeScalars(NULL),
      CubePoints(NULL),
      CubeIdList(NULL),
      CubeGrid(NULL),
      ContourFilter(NULL),
      TriangleEdgeActor(NULL),
      ShrinkActor(NULL),
      CubeEdgeActor(NULL),
      CubeVertexActor(NULL),
      CaseLabel(NULL),
      LabelActor(NULL),
      BaseActor(NULL)
    {
    }

  ~MCasesInternal()
    {
      CubeScalars->Delete();
      CubePoints->Delete();
      CubeIdList->Delete();
      CubeGrid->Delete();

      ContourFilter->Delete();

      TriangleEdgeActor->Delete();
      ShrinkActor->Delete();
      CubeEdgeActor->Delete();
      CubeVertexActor->Delete();
      CaseLabel->Delete();
      LabelActor->Delete();
      BaseActor->Delete();
    }

  void  AddActors (vtkRenderer* aRenderer)
    {
      if ((this->Renderer = aRenderer) == NULL)
        return;

      /* define a Single Cube */
      this->CubeScalars = vtkFloatArray::New();
        {
        this->CubeScalars->InsertNextValue(1.0);
        this->CubeScalars->InsertNextValue(0.0);
        this->CubeScalars->InsertNextValue(0.0);
        this->CubeScalars->InsertNextValue(1.0);
        this->CubeScalars->InsertNextValue(0.0);
        this->CubeScalars->InsertNextValue(0.0);
        this->CubeScalars->InsertNextValue(0.0);
        this->CubeScalars->InsertNextValue(0.0);
        // Case 9 - 00001001
        }
      this->CubePoints = vtkPoints::New();
        {
        this->CubePoints->InsertNextPoint(0, 0, 0);
        this->CubePoints->InsertNextPoint(1, 0, 0);
        this->CubePoints->InsertNextPoint(1, 1, 0);
        this->CubePoints->InsertNextPoint(0, 1, 0);
        this->CubePoints->InsertNextPoint(0, 0, 1);
        this->CubePoints->InsertNextPoint(1, 0, 1);
        this->CubePoints->InsertNextPoint(1, 1, 1);
        this->CubePoints->InsertNextPoint(0, 1, 1);
        }
      this->CubeIdList = vtkIdList::New();
        {
        this->CubeIdList->InsertNextId(0);
        this->CubeIdList->InsertNextId(1);
        this->CubeIdList->InsertNextId(2);
        this->CubeIdList->InsertNextId(3);
        this->CubeIdList->InsertNextId(4);
        this->CubeIdList->InsertNextId(5);
        this->CubeIdList->InsertNextId(6);
        this->CubeIdList->InsertNextId(7);
        }
      this->CubeGrid = vtkUnstructuredGrid::New();
        {
        this->CubeGrid->Allocate(10, 10);
        this->CubeGrid->InsertNextCell(12, this->CubeIdList);
        this->CubeGrid->SetPoints(this->CubePoints);
        this->CubeGrid->GetPointData()->SetScalars(this->CubeScalars);
        }

      /* Find the triangles that lie along the 0.5 contour in this cube. */
      this->ContourFilter = vtkContourFilter::New();
        {
        this->ContourFilter->SetInputData(this->CubeGrid);
        this->ContourFilter->SetValue(0, 0.5);
        }
  
      /* Draw the edges as tubes instead of lines.  Also create the associated
       * mapper and actor to display the tubes.
       */
      this->TriangleEdgeActor = vtkActor::New();
        {
        vtkPolyDataMapper* triangleEdgeMapper = vtkPolyDataMapper::New();
          {
          vtkTubeFilter* triangleEdgeTubes = vtkTubeFilter::New();
            {
            /* Extract the edges of the triangles just found. */
            vtkExtractEdges* triangleEdges = vtkExtractEdges::New();
              {
              triangleEdges->SetInputConnection(this->ContourFilter->GetOutputPort());
              }
            triangleEdgeTubes->SetInputConnection(triangleEdges ->GetOutputPort());
            triangleEdgeTubes->SetRadius(0.005);
            triangleEdgeTubes->SetNumberOfSides(8);
            triangleEdgeTubes->UseDefaultNormalOn();
            triangleEdgeTubes->SetDefaultNormal(0.577, 0.577, 0.577);
            triangleEdgeTubes->CappingOn();
            triangleEdges->Delete();
            }
          triangleEdgeMapper->SetInputConnection(triangleEdgeTubes->GetOutputPort());
          triangleEdgeMapper->ScalarVisibilityOff();
          triangleEdgeTubes->Delete();
          }
        this->TriangleEdgeActor->SetMapper(triangleEdgeMapper);
        this->TriangleEdgeActor->GetProperty()->SetDiffuseColor(lamp_black[0],
                                                                lamp_black[1],
                                                                lamp_black[2]);
        this->TriangleEdgeActor->GetProperty()->SetSpecular(0.4);
        this->TriangleEdgeActor->GetProperty()->SetSpecularPower(10);
        triangleEdgeMapper->Delete();
        }

      /* Shrink the triangles we found earlier. Create the associated mapper
       * and actor.  Set the opacity of the shrunken triangles.
       */
      this->ShrinkActor = vtkActor::New();
        {
        vtkPolyDataMapper* shrinkMapper = vtkPolyDataMapper::New();
          {
          vtkDepthSortPolyData* depthSorter = vtkDepthSortPolyData::New();
            {
            vtkShrinkPolyData* shrinker = vtkShrinkPolyData::New();
              {
              shrinker->SetInputConnection(this->ContourFilter->GetOutputPort());
              shrinker->SetShrinkFactor(1);
              }
            depthSorter->SetInputConnection(shrinker->GetOutputPort());
            depthSorter->SetProp3D(this->ShrinkActor);
            depthSorter->SetDepthSortModeToParametricCenter();
            depthSorter->SetDirectionToBackToFront();
            depthSorter->SetCamera(this->Renderer->GetActiveCamera());
            shrinker->Delete();
            }
          shrinkMapper->SetInputConnection(depthSorter->GetOutputPort());
          shrinkMapper->ScalarVisibilityOff();
          depthSorter->Delete();
          }
        this->ShrinkActor->SetMapper(shrinkMapper);
        this->ShrinkActor->GetProperty()->SetDiffuseColor(banana[0],
                                                          banana[1],
                                                          banana[2]);
        this->ShrinkActor->GetProperty()->SetOpacity(0.6);
        shrinkMapper->Delete();
        }

      /* Draw a cube the same size and at the same position as the one created
       * previously.  Extract the edges because we only want to see the outline
       * of the cube.  Pass the edges through a vtkTubeFilter so they are 
       * displayed as tubes rather than lines.
       */

      /* Create the mapper and actor to display the cube edges. */
      this->CubeEdgeActor = vtkActor::New();
        {
        vtkPolyDataMapper* cubeEdgeMapper = vtkPolyDataMapper::New();
          {
          vtkTubeFilter* cubeEdgeTubes = vtkTubeFilter::New();
            {
            vtkExtractEdges* cubeEdges = vtkExtractEdges::New();
              {
              vtkCubeSource* cubeSource = vtkCubeSource::New();
                {
                cubeSource->SetCenter(0.5, 0.5, 0.5);
                }
              cubeEdges->SetInputConnection(cubeSource->GetOutputPort());
              cubeSource->Delete();
              }
            cubeEdgeTubes->SetInputConnection(cubeEdges->GetOutputPort());
            cubeEdgeTubes->SetRadius(0.01);
            cubeEdgeTubes->SetNumberOfSides(8);
            cubeEdgeTubes->UseDefaultNormalOn();
            cubeEdgeTubes->SetDefaultNormal(0.577, 0.577, 0.577);
            cubeEdgeTubes->CappingOn();
            cubeEdges->Delete();
            }
          cubeEdgeMapper->SetInputConnection(cubeEdgeTubes->GetOutputPort());
          cubeEdgeTubes->Delete();
          }
        this->CubeEdgeActor->SetMapper(cubeEdgeMapper);
        this->CubeEdgeActor->GetProperty()->SetDiffuseColor(khaki[0],
                                                            khaki[1],
                                                            khaki[2]);
        this->CubeEdgeActor->GetProperty()->SetSpecular(0.4);
        this->CubeEdgeActor->GetProperty()->SetSpecularPower(10);
        cubeEdgeMapper->Delete();
        }

      this->CubeVertexActor = vtkActor::New();
        {
        /* Create a mapper and actor to display the glyphs. */
        vtkPolyDataMapper* cubeVertexMapper = vtkPolyDataMapper::New();
          {
          /* Display spheres at the vertices remaining in the cube data set 
           * after it was passed through vtkThresholdPoints.
           */
          vtkGlyph3D* cubeVertexGlyphs = vtkGlyph3D::New();
            {
            /* Remove the part of the cube with data values below 0.5. */
            vtkThresholdPoints* cubeThresholdPoints = vtkThresholdPoints::New();
              {
              cubeThresholdPoints->SetInputData(this->CubeGrid);
              cubeThresholdPoints->ThresholdByUpper(0.5);
              }
            /* Create a sphere to use as a glyph source for vtkGlyph3D. */
            vtkSphereSource* sphereSource = vtkSphereSource::New();
              {
              sphereSource->SetRadius(0.04);
              sphereSource->SetPhiResolution(20);
              sphereSource->SetThetaResolution(20);
              }
            cubeVertexGlyphs->SetInputConnection(cubeThresholdPoints->GetOutputPort());
            cubeVertexGlyphs->SetSourceConnection(sphereSource->GetOutputPort());
            cubeThresholdPoints->Delete();
            sphereSource->Delete();
            }
          cubeVertexMapper->SetInputConnection(cubeVertexGlyphs->GetOutputPort());
          cubeVertexMapper->ScalarVisibilityOff();
          cubeVertexGlyphs->Delete();
          }
        this->CubeVertexActor->SetMapper(cubeVertexMapper);
        this->CubeVertexActor->GetProperty()->SetDiffuseColor(tomato[0],
                                                              tomato[1],
                                                              tomato[2]);
        cubeVertexMapper->Delete();
        }

      /* Create a mapper and actor to display the text. */
      this->LabelActor = vtkActor::New();
        {
        vtkPolyDataMapper* labelMapper = vtkPolyDataMapper::New();
          {
          vtkLinearExtrusionFilter* extruder = vtkLinearExtrusionFilter::New();
            {
            /* Define the text for the label */
            this->CaseLabel = vtkVectorText::New();
              {
              this->CaseLabel->SetText(this->CaseName.c_str());
              }
            extruder->SetInputConnection(this->CaseLabel->GetOutputPort());
            extruder->SetExtrusionTypeToVectorExtrusion();
            extruder->SetVector(0, 0, 1);
            extruder->CappingOn();
            extruder->SetScaleFactor(0.25);
            }
          /* Move the label to a new position. */
          vtkTransformPolyDataFilter* labelTransformFilter =
            vtkTransformPolyDataFilter::New();
            {
            /* Set up a transform to move the label to a new position. */
            vtkTransform* labelTransform = vtkTransform::New();
              {
              labelTransform->Identity();
              labelTransform->Translate(-0.2, 0.0, 1.25);
              labelTransform->Scale(0.1, 0.1, 0.1);
              }
            labelTransformFilter->SetInputConnection(extruder->GetOutputPort());
            labelTransformFilter->SetTransform(labelTransform);
            labelTransform->Delete();
            extruder->Delete();
            }
          labelMapper->SetInputConnection(labelTransformFilter->GetOutputPort());
          labelTransformFilter->Delete();
          this->CaseLabel->Delete();
          }
        this->LabelActor->SetMapper(labelMapper);
        labelMapper->Delete();
        }

      /* Define the base that the cube sits on.  Create its associated mapper
       * and actor.  Set the position of the actor.
       */
      this->BaseActor = vtkActor::New();
        {
        vtkPolyDataMapper* baseMapper = vtkPolyDataMapper::New();
          {
          vtkCubeSource* cubeSource = vtkCubeSource::New();
            {
            cubeSource->SetXLength(1.5);
            cubeSource->SetYLength(0.01);
            cubeSource->SetZLength(1.5);
            }
          baseMapper->SetInputConnection(cubeSource->GetOutputPort());
          cubeSource->Delete();
          }
        this->BaseActor->SetMapper(baseMapper);
        this->BaseActor->SetPosition(0.5, -0.09, 0.5);
        baseMapper->Delete();
        }

      aRenderer->AddActor(this->TriangleEdgeActor);
      aRenderer->AddActor(this->BaseActor);
      aRenderer->AddActor(this->LabelActor);
      aRenderer->AddActor(this->CubeEdgeActor);
      aRenderer->AddActor(this->CubeVertexActor);
      aRenderer->AddActor(this->ShrinkActor);
    }

  void  SetCase (float in, float out, unsigned char n)
    {
      float table[8] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

      // Eliminate complaints from vtkExtractEdges when there are no edges.
      if ((this->Case = n) == 0 || this->Case == 255)
        this->TriangleEdgeActor->VisibilityOff();
      else
        this->TriangleEdgeActor->VisibilityOn();

      table[0] = (this->Case & 0x01 ? in : out);
      table[1] = (this->Case & 0x02 ? in : out);
      table[2] = (this->Case & 0x04 ? in : out);
      table[3] = (this->Case & 0x08 ? in : out);
      table[4] = (this->Case & 0x10 ? in : out);
      table[5] = (this->Case & 0x20 ? in : out);
      table[6] = (this->Case & 0x40 ? in : out);
      table[7] = (this->Case & 0x80 ? in : out);

      std::ostringstream label;

      label << "Case " << int(this->Case) << " - ";

      for (vtkIdType j=7; j>=0; j--)
        {
        this->CubeScalars->InsertComponent(j, 0, table[j]);
        label << int(table[j]==in ? (in>out) : (out>in));
        }

      this->CaseName.assign(label.str());
      this->CubeGrid->Modified();
      this->CaseLabel->SetText(this->CaseName.c_str());
    }

private:
  MCasesInternal (const MCasesInternal&); // Not Implemented
  MCasesInternal& operator= (const MCasesInternal&); // Not Implemented
};

// ----------------------------------------------------------------------------
MCasesBase::MCasesBase (void)
  : Internal(new MCasesInternal),
    FltkWindow(0),
    VtkWindow(0)
{
}

MCasesBase::~MCasesBase()
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    delete this->FltkWindow;
    }

  delete this->Internal;
}

// ----------------------------------------------------------------------------
void
MCasesBase::SetFltkWindow (Fl_Window* aWindow)
{
  if      (this->FltkWindow != (Fl_Window *) 0)
    {
    vtkErrorMacro(<< "The FltkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == (Fl_Window *) 0)
    {
    vtkErrorMacro(<< "The FltkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_Window pointer is NULL.");
    }
  else
    {
    this->FltkWindow = aWindow;
    this->FltkWindow->user_data((void *) this);
    }
}

Fl_Window*
MCasesBase::GetFltkWindow (void) const
{
  return this->FltkWindow;
}

// ----------------------------------------------------------------------------
void
MCasesBase::SetVtkWindow (Fl_VTK_Window* aWindow)
{
  if      (this->VtkWindow != (Fl_VTK_Window *) 0)
    {
    vtkErrorMacro(<< "The VtkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor.");
    }
  else if (aWindow == (Fl_VTK_Window *) 0)
    {
    vtkErrorMacro(<< "The VtkWindow is expected to be set ONCE,"
                  << " and ONLY ONCE, within the subclass constructor,"
                  << " but the Fl_VTK_Window pointer is NULL.");
    }
  else
    {
    this->VtkWindow = aWindow;
    this->VtkWindow->user_data((void *) this);
    }
}

Fl_VTK_Window*
MCasesBase::GetVtkWindow (void) const
{
  return this->VtkWindow;
}

// ----------------------------------------------------------------------------
void
MCasesBase::SetCase (float in, float out, unsigned char n)
{
  this->Internal->SetCase(in, out, n);
}

// ----------------------------------------------------------------------------
void
MCasesBase::Show (int aArgc, char* aArgv[])
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show(aArgc, aArgv);

    if (this->Internal->Renderer == NULL && this->VtkWindow != NULL)
      {
      this->Internal->AddActors(this->VtkWindow->GetDefaultRenderer());
      this->VtkWindow->GetRenderWindow()->Render();
      }
    }
}

void
MCasesBase::Show (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->show();

    if (this->Internal->Renderer == NULL && this->VtkWindow != NULL)
      {
      this->Internal->AddActors(this->VtkWindow->GetDefaultRenderer());
      this->VtkWindow->GetRenderWindow()->Render();
      }
    }
}

int
MCasesBase::Shown (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    return this->FltkWindow->shown();
    }

  return 0;
}

void
MCasesBase::Hide (void)
{
  if (this->FltkWindow != (Fl_Window *) 0)
    {
    this->FltkWindow->hide();
    }
}

/*
 * End of: $Id: MCasesBase.cxx,v 1.8 2004/05/13 18:37:49 xpxqx Exp $.
 *
 */
