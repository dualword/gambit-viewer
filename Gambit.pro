TEMPLATE = app
TARGET = Gambit
DEPENDPATH += . \
    src/
INCLUDEPATH += . \
    src/
RESOURCES += resources/icons.qrc

# Input
HEADERS += src/DataStructures/GSnp.h \
    src/DataStructures/GReference.h \
    src/DataStructures/GGenomicDataSet.h \
    src/DataStructures/GGenomicDataRegion.h \
    src/DataStructures/GGene.h \
    src/DataStructures/GFileInfo.h \
    src/DataStructures/GFileFormatData.h \
    src/DataStructures/GColorScheme.h \
    src/DataStructures/GAllele.h \
    src/DataStructures/GAlignment.h \
    src/Main/GMainWindow.h \
    src/Main/GMainNavigationWidget.h \
    src/Main/GHomeWidget.h \
    src/Main/GambitAPI.h \
    src/SessionManager/DataManager/GMismatchCalculator.h \
    src/SessionManager/DataManager/GGenomicDataPadder.h \
    src/SessionManager/DataManager/GDataManager.h \
    src/SessionManager/FileManager/GOpenFilesWidget.h \
    src/SessionManager/FileManager/GFilenameDialog.h \
    src/SessionManager/FileManager/GFileManager.h \
    src/SessionManager/FileManager/GCloseFilesDialog.h \
    src/SessionManager/FileManager/GAbstractFormatManager.h \
    src/SessionManager/FileManager/GAbstractFileReader.h \
    src/SessionManager/GSessionManager.h \
    src/Utilities/GTrueFalseComboDelegate.h \
    src/Utilities/GStyleHelper.h \
    src/Utilities/GStyleAnimator.h \
    src/Utilities/GManhattanStyle.h \
    src/Utilities/flickcharm.h \
    src/Viewer/AssemblyView/GVisibleSnpItem.h \
    src/Viewer/AssemblyView/GVisibleGeneItem.h \
    src/Viewer/AssemblyView/GVisibleCursorItem.h \
    src/Viewer/AssemblyView/GVisibleAlignmentItem.h \
    src/Viewer/AssemblyView/GVisibleAlignmentGroupHeader.h \
    src/Viewer/AssemblyView/GVisibleAlignmentGroup.h \
    src/Viewer/AssemblyView/GAssemblyView.h \
    src/Viewer/AssemblyView/GAssemblyToolbar.h \
    src/Viewer/AssemblyView/GAssemblySettingsManager.h \
    src/Viewer/AssemblyView/GAssemblySettingsDialog.h \
    src/Viewer/GVisibleReferenceItem.h \
    src/Viewer/GViewer.h \
    src/Viewer/GSnpInfoDialog.h \
    src/Viewer/GSliderView.h \
    src/Viewer/GReferenceView.h
SOURCES += src/DataStructures/GFileInfo.cpp \
    src/Main/GMainWindow.cpp \
    src/Main/GMainNavigationWidget.cpp \
    src/Main/GHomeWidget.cpp \
    src/Main/GambitMain.cpp \
    src/Main/GambitAPI.cpp \
    src/SessionManager/DataManager/GMismatchCalculator.cpp \
    src/SessionManager/DataManager/GGenomicDataPadder.cpp \
    src/SessionManager/DataManager/GDataManager.cpp \
    src/SessionManager/FileManager/GOpenFilesWidget.cpp \
    src/SessionManager/FileManager/GFilenameDialog.cpp \
    src/SessionManager/FileManager/GFileManager.cpp \
    src/SessionManager/FileManager/GCloseFilesDialog.cpp \
    src/SessionManager/FileManager/GAbstractFormatManager.cpp \
    src/SessionManager/GSessionManager.cpp \
    src/Utilities/GTrueFalseComboDelegate.cpp \
    src/Utilities/GStyleHelper.cpp \
    src/Utilities/GStyleAnimator.cpp \
    src/Utilities/GManhattanStyle.cpp \
    src/Utilities/flickcharm.cpp \
    src/Viewer/AssemblyView/GVisibleSnpItem.cpp \
    src/Viewer/AssemblyView/GVisibleGeneItem.cpp \
    src/Viewer/AssemblyView/GVisibleCursorItem.cpp \
    src/Viewer/AssemblyView/GVisibleAlignmentItem.cpp \
    src/Viewer/AssemblyView/GVisibleAlignmentGroupHeader.cpp \
    src/Viewer/AssemblyView/GVisibleAlignmentGroup.cpp \
    src/Viewer/AssemblyView/GAssemblyView.cpp \
    src/Viewer/AssemblyView/GAssemblyToolbar.cpp \
    src/Viewer/AssemblyView/GAssemblySettingsManager.cpp \
    src/Viewer/AssemblyView/GAssemblySettingsDialog.cpp \
    src/Viewer/GVisibleReferenceItem.cpp \
    src/Viewer/GViewer.cpp \
    src/Viewer/GSnpInfoDialog.cpp \
    src/Viewer/GSliderView.cpp \
    src/Viewer/GReferenceView.cpp
