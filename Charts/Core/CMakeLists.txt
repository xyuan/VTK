set(Module_SRCS
  vtkAxis.cxx
  vtkAxisExtended.cxx
  vtkChart.cxx
  vtkChartHistogram2D.cxx
  vtkChartLegend.cxx
  vtkChartMatrix.cxx
  vtkChartParallelCoordinates.cxx
  vtkChartPie.cxx
  vtkChartXY.cxx
  vtkChartXYZ.cxx
  vtkColorLegend.cxx
  vtkColorSeries.cxx
  vtkColorTransferControlPointsItem.cxx
  vtkColorTransferFunctionItem.cxx
  vtkCompositeControlPointsItem.cxx
  vtkCompositeTransferFunctionItem.cxx
  vtkContextPolygon.cxx
  vtkControlPointsItem.cxx
  vtkLookupTableItem.cxx
  vtkPiecewiseControlPointsItem.cxx
  vtkPiecewiseFunctionItem.cxx
  vtkPiecewisePointHandleItem.cxx
  vtkPlot.cxx
  vtkPlotBar.cxx
  vtkPlotGrid.cxx
  vtkPlotHistogram2D.cxx
  vtkPlotLine.cxx
  vtkPlotParallelCoordinates.cxx # This adds a vtkInfovisCore dep for one class...
  vtkPlotPie.cxx
  vtkPlotPoints.cxx
  vtkPlotStacked.cxx
  vtkScalarsToColorsItem.cxx
  vtkScatterPlotMatrix.cxx
  )

set_source_files_properties(
  vtkChart
  vtkContextPolygon
  vtkControlPointsItem
  vtkPlot
  vtkScalarsToColorsItem
  ABSTRACT
  )

vtk_module_library(vtkChartsCore ${Module_SRCS})
