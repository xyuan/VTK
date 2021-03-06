vtk_module(vtkIOParallel
  GROUPS
    MPI
  IMPLEMENTS
    vtkIOImage
  DEPENDS
    vtkParallelCore
    vtkFiltersParallel
    vtkIOParallelMPI
    vtkIONetCDF
    vtkexodusII
    vtkVPIC
  TEST_DEPENDS
    vtkTestingCore
  )
