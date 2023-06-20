cmake_minimum_required(VERSION 3.16)

project(NetworkVisualizer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appNetworkVisualizer
    main.cpp
	NetworkVisualizer.qrc
	graphmodel.cpp
	graphmodel.h
	customnetworknode.h
	customnetworknode.cpp
	customnetworkgraph.h
	customnetworkgraph.cpp
	customnetworkedge.h
	customnetworkedge.cpp
)


qt_add_qml_module(appNetworkVisualizer
    URI NetworkVisualizer
    VERSION 1.0
	QML_FILES
	    Main.qml
		RoundNode.qml
		NodeListView.qml
		ColorLegend.qml
)

set_target_properties(appNetworkVisualizer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

add_subdirectory(QuickQanava)

target_link_libraries(appNetworkVisualizer
    PRIVATE Qt6::Quick
	PUBLIC
	    QuickQanava
)

install(TARGETS appNetworkVisualizer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
