vtk_module(vtkIOExport
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonCore
    vtkRenderingCore
    vtkRenderingFreeType
    vtkImagingCore
    vtkIOCore
    vtkgl2ps
  TEST_DEPENDS
    vtkTestingRendering
    vtkRenderingOpenGL
  )
