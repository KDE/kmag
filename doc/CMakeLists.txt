########### install files ###############

#
kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR kmag)
kdoctools_create_manpage(man-kmag.1.docbook 1 INSTALL_DESTINATION ${KDE_INSTALL_MANDIR})
