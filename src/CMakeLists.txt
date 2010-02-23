set(chromi_SRCS factory.cpp client.cpp button.cpp themeconfig.cpp)
kde4_add_plugin(kwin3_chromi ${chromi_SRCS})
target_link_libraries(kwin3_chromi ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} ${KDE4WORKSPACE_KDECORATIONS_LIBS} ${X11_X11_LIB})

install(TARGETS kwin3_chromi  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES chromi.desktop  DESTINATION ${DATA_INSTALL_DIR}/kwin)

if(NOT EXISTS ${CONFIG_INSTALL_DIR}/aurorae.knsrc)
  install(FILES aurorae.knsrc  DESTINATION  ${CONFIG_INSTALL_DIR})
endif(NOT EXISTS ${CONFIG_INSTALL_DIR}/aurorae.knsrc)

########### config ###############
set(kwin_chromi_config_PART_SRCS config/config.cpp themeconfig.cpp)
kde4_add_ui_files(kwin_chromi_config_PART_SRCS config/config.ui)
kde4_add_plugin(kwin_chromi_config ${kwin_chromi_config_PART_SRCS})
target_link_libraries(kwin_chromi_config  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_PLASMA_LIBS} ${QT_QTGUI_LIBRARY} ${KDE4_KNEWSTUFF2_LIBS})
install(TARGETS kwin_chromi_config  DESTINATION ${PLUGIN_INSTALL_DIR})
