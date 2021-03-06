/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridFractalSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHyperTreeGridFractalSource.h"

#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeCursor.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

#include <assert.h>

vtkStandardNewMacro(vtkHyperTreeGridFractalSource);

//----------------------------------------------------------------------------
vtkHyperTreeGridFractalSource::vtkHyperTreeGridFractalSource()
{
  // This a source: no input ports
  this->SetNumberOfInputPorts( 0 );

  this->GridSize[0] = 1;
  this->GridSize[1] = 1;
  this->GridSize[2] = 1;
  this->AxisBranchFactor = 2;
  this->MaximumLevel = 1;
  this->Dimension = 3;
  this->Dual = false;

  // Grid geometry
  this->XCoordinates = vtkDoubleArray::New();
  this->XCoordinates->SetNumberOfTuples( 2 );
  this->XCoordinates->SetComponent( 0, 0, 0. );
  this->XCoordinates->SetComponent( 1, 0, 1. );
  this->YCoordinates = vtkDoubleArray::New();
  this->YCoordinates->SetNumberOfTuples( 2 );
  this->YCoordinates->SetComponent( 0, 0, 0. );
  this->YCoordinates->SetComponent( 1, 0, 1. );
  this->ZCoordinates = vtkDoubleArray::New();
  this->ZCoordinates->SetNumberOfTuples( 2 );
  this->ZCoordinates->SetComponent( 0, 0, 0. );
  this->ZCoordinates->SetComponent( 1, 0, 1. );
}

//----------------------------------------------------------------------------
vtkHyperTreeGridFractalSource::~vtkHyperTreeGridFractalSource()
{
 if ( this->XCoordinates )
    {
    this->XCoordinates->UnRegister( this );
    this->XCoordinates = NULL;
    }

  if ( this->YCoordinates )
    {
    this->YCoordinates->UnRegister( this );
    this->YCoordinates = NULL;
    }

  if ( this->ZCoordinates )
    {
    this->ZCoordinates->UnRegister( this );
    this->ZCoordinates = NULL;
    }
}

//----------------------------------------------------------------------------
// Description:
// Return the maximum number of levels of the hyperoctree.
// \post positive_result: result>=1
int vtkHyperTreeGridFractalSource::GetMaximumLevel()
{
  assert("post: positive_result" && this->MaximumLevel>=1);
  return this->MaximumLevel;
}

//----------------------------------------------------------------------------
// Description:
// Set the maximum number of levels of the hyperoctree. If
// GetMinLevels()>=levels, GetMinLevels() is changed to levels-1.
// \pre positive_levels: levels>=1
// \post is_set: this->GetLevels()==levels
// \post min_is_valid: this->GetMinLevels()<this->GetLevels()
void vtkHyperTreeGridFractalSource::SetMaximumLevel( int levels )
{
  if ( levels < 1 )
    {
    levels = 1;
    }

  if ( this->MaximumLevel == levels )
    {
    return;
    }

  this->Modified();
  this->MaximumLevel = levels;
  if(this->MinimumLevel > levels )
    {
    this->MinimumLevel = levels;
    }

  assert("post: is_set" && this->GetMaximumLevel()==levels);
  assert("post: min_is_valid" && this->GetMinimumLevel()<=this->GetMaximumLevel());
}


//----------------------------------------------------------------------------
// Description:
// Return the minimal number of levels of systematic subdivision.
// \post positive_result: result>=0
int vtkHyperTreeGridFractalSource::GetMinimumLevel()
{
  assert( "post: positive_result" && this->MinimumLevel >= 0 );
  return this->MinimumLevel;
}

//----------------------------------------------------------------------------
// Description:
// Set the minimal number of levels of systematic subdivision.
// \pre positive_minLevels: minLevels>=0 && minLevels<this->GetLevels()
// \post is_set: this->GetMinLevels()==minLevels
void vtkHyperTreeGridFractalSource::SetMinimumLevel( int minLevels )
{
  if ( minLevels < 1 )
    {
    minLevels = 1;
    }

  if ( this->MinimumLevel == minLevels )
    {
    return;
    }

  this->Modified();
  this->MinimumLevel = minLevels;
  assert( "post: is_set" && this->GetMinimumLevel() == minLevels );
}

//----------------------------------------------------------------------------
int vtkHyperTreeGridFractalSource::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector ** vtkNotUsed( inputVector ),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // We cannot give the exact number of levels of the hyperoctree
  // because it is not generated yet and this process is random-based.
  // Just send an upper limit.
  // Used by the vtkHyperTreeGridToUniformGrid to send some
  // whole extent in RequestInformation().
  outInfo->Set( vtkHyperTreeGrid::LEVELS(), this->MaximumLevel );
  outInfo->Set( vtkHyperTreeGrid::DIMENSION(), this->Dimension );
//   int ii;
//   for (ii = 0; ii < 3; ++ii)
//     {
//     this->Size[ii] = this->SizeCX[this->ProjectionAxes[ii]];
//     this->Origin[ii] = this->OriginCX[this->ProjectionAxes[ii]];
//     }
//   if (this->Dimension == 2)
//     {
//     this->Size[2] = 0.0;
//     }
//   outInfo->Set(vtkHyperTreeGrid::SIZES(),this->Size,3);
  double origin[3];
  origin[0] = this->XCoordinates->GetTuple1( 0 );
  origin[1] = this->YCoordinates->GetTuple1( 0 );
  origin[2] = this->ZCoordinates->GetTuple1( 0 );
  outInfo->Set( vtkDataObject::ORIGIN(), origin, 3 );

  return 1;
}

//----------------------------------------------------------------------------
int vtkHyperTreeGridFractalSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  vtkHyperTreeGrid *output = vtkHyperTreeGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Set grid parameters
  output->SetGridSize( this->GridSize );
  output->SetDimension( this->Dimension );
  output->SetAxisBranchFactor( this->AxisBranchFactor );

  // Per-axis scaling
  double scale[3];
  scale[0] = 1.5;
  scale[1] = 1.;
  scale[2] = .7;

  // Create geometry
  for ( int i = 0; i < 3; ++ i )
    {
    vtkDoubleArray *coords = vtkDoubleArray::New();
    int n = this->GridSize[i] + 1;
    coords->SetNumberOfValues( n );
    for ( int j = 0; j < n; ++ j )
      {
      coords->SetValue( j, scale[i] * static_cast<double>( j ) );
      }

    if ( i == 0 )
      {
      output->SetXCoordinates( coords );
      }
    else if ( i == 1 )
      {
      output->SetYCoordinates( coords );
      }
    else // i must be 2 here
      {
      output->SetZCoordinates( coords );
      }

    // Clean up
    coords->Delete();
    }

  vtkDoubleArray *scalars = vtkDoubleArray::New();
  scalars->SetNumberOfComponents( 1 );

  vtkIdType fact = 1;
  for ( int i = 1; i < this->MaximumLevel; ++ i )
    {
    fact *= this->AxisBranchFactor;
    }

  scalars->Allocate( fact * fact );
  scalars->SetName( "Test" );
  output->GetLeafData()->SetScalars( scalars );
  scalars->UnRegister( this );

  int n[3];
  output->GetGridSize( n );
  for ( int i = 0; i < n[0]; ++ i )
    {
    for ( int j = 0; j < n[1]; ++ j )
      {
      for ( int k = 0; k < n[2]; ++ k )
        {
        int index = ( k * this->GridSize[1] + j ) * this->GridSize[0] + i;

        vtkHyperTreeCursor* cursor = output->NewCellCursor( i, j, k );
        cursor->ToRoot();

        int idx[3];
        idx[0] = idx[1] = idx[2] = 0;
        int offset = output->GetLeafData()->GetScalars()->GetNumberOfTuples();
        this->Subdivide( cursor, 1, output, index, idx, offset );
        cursor->UnRegister( this );
        } // k
      } // j
    } // i

  output->SetDualGridFlag( this->Dual );
  assert("post: dataset_and_data_size_match" && output->CheckAttributes()==0);

  return 1;
}


//----------------------------------------------------------------------------
void vtkHyperTreeGridFractalSource::Subdivide( vtkHyperTreeCursor* cursor,
                                               int level,
                                               vtkHyperTreeGrid* output,
                                               int index,
                                               int idx[3],
                                               int offset )
{
  // Determine whether to subdivide.
  int subdivide = 1;

  if ( idx[0] || idx[1] || idx[2] )
    {
    subdivide = 0;
    }
  if ( ! index && idx[1] == 1 && ! idx[2] )
    {
    subdivide = 1;
    }

  // Check for hard coded minimum and maximum level restrictions.
  if ( level >= this->MaximumLevel )
    {
    subdivide = 0;
    }

  if ( subdivide )
    {
    output->SubdivideLeaf( cursor, index );

    // Now traverse to children.
    int xDim, yDim, zDim;
    xDim = yDim = zDim = 1;
    if ( this->Dimension == 1 )
      {
      xDim = this->AxisBranchFactor;
      }
    else if ( this->Dimension == 2 )
      {
      xDim = yDim = this->AxisBranchFactor;
      }
    else if ( this->Dimension == 3 )
      {
      xDim = yDim = zDim = this->AxisBranchFactor;
      }
    int childIdx = 0;
    int newIdx[3];
    for ( int z = 0; z < zDim; ++ z )
      {
      newIdx[2] = idx[2] * zDim + z;
      for ( int y = 0; y < yDim; ++ y )
        {
        newIdx[1] = idx[1] * yDim + y;
        for ( int x = 0; x < xDim; ++ x )
          {
          newIdx[0] = idx[0] * xDim + x;
          cursor->ToChild(childIdx);
          this->Subdivide( cursor,
                           level + 1,
                           output,
                           index,
                           newIdx,
                           offset );
          cursor->ToParent();
          ++ childIdx;
          }
        }
      }
    }
  else
    {
    // Cell value
    double val = idx[0] + idx[1] + idx[2];

    // Offset cell index as needed
    vtkIdType id = offset + cursor->GetLeafId();
    output->GetLeafData()->GetScalars()->InsertTuple1( id, val );
    }
}

//-----------------------------------------------------------------------------
void vtkHyperTreeGridFractalSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Dimension: " << this->Dimension << endl;
  os << indent << "GridSize: "
     << this->GridSize[0] <<","
     << this->GridSize[1] <<","
     << this->GridSize[2] << endl;
  if ( this->XCoordinates )
    {
    this->XCoordinates->PrintSelf( os, indent.GetNextIndent() );
    }
  if ( this->YCoordinates )
    {
    this->YCoordinates->PrintSelf( os, indent.GetNextIndent() );
    }
  if ( this->ZCoordinates )
    {
    this->ZCoordinates->PrintSelf( os, indent.GetNextIndent() );
    }
  os << indent << "MaximumLevel: " << this->MaximumLevel << endl;
  os << indent << "MinimumLevel: " << this->MinimumLevel << endl;
  os << indent << "Dual: " << this->Dual << endl;
}
