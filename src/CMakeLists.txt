set(chromi_SRCS factory.cpp client.cpp)
kde4_add_plugin(kwin3_chromi ${chromi_SRCS})
target_link_libraries(kwin3_chromi ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} ${KDE4WORKSPACE_KDECORATIONS_LIBS} ${X11_X11_LIB})

install(TARGETS kwin3_chromi  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES chromi.desktop  DESTINATION ${DATA_INSTALL_DIR}/kwin)
