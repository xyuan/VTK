set(Module_SRCS
  vtkArcSource.cxx
  vtkArrowSource.cxx
  vtkButtonSource.cxx
  vtkConeSource.cxx
  vtkCubeSource.cxx
  vtkCylinderSource.cxx
  vtkDiskSource.cxx
  vtkEllipticalButtonSource.cxx
  vtkFrustumSource.cxx
  vtkGlyphSource2D.cxx
  vtkHyperTreeFractalSource.cxx
  vtkHyperOctreeFractalSource.cxx
  vtkLineSource.cxx
  #vtkOutlineCornerFilter.cxx
  vtkOutlineCornerSource.cxx
  vtkOutlineSource.cxx
  vtkParametricFunctionSource.cxx
  vtkPlaneSource.cxx
  vtkPlatonicSolidSource.cxx
  vtkPointSource.cxx
  vtkProgrammableDataObjectSource.cxx
  vtkProgrammableSource.cxx
  vtkRectangularButtonSource.cxx
  vtkRegularPolygonSource.cxx
  #vtkRRandomTableSource.cxx # Should be in R kit
  #vtkSectorSource.cxx # Needs vtkRotationalExtrusionFilter
  vtkSelectionSource.cxx
  vtkSphereSource.cxx
  vtkSuperquadricSource.cxx
  vtkTessellatedBoxSource.cxx
  vtkTextSource.cxx
  vtkTexturedSphereSource.cxx
  vtkGraphToPolyData.cxx
  vtkDiagonalMatrixSource.cxx
  )

set_source_files_properties(
  vtkButtonSource
  ABSTRACT
  )

vtk_module_library(vtkFiltersSources ${Module_SRCS})
