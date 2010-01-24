set(chromi_SRCS chromi.cpp chromiclient.cpp titlebar.cpp button.cpp)
kde4_add_plugin(kwin3_chromi ${chromi_SRCS})
target_link_libraries(kwin3_chromi ${KDE4_KDEUI_LIBS} kdecorations ${X11_X11_LIB})

install(TARGETS kwin3_chromi  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES chromi.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin)
