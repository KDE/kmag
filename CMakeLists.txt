kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kmag_SRCS 
   kmagzoomview.cpp 
   kmagselrect.cpp 
   kmag.cpp 
   main.cpp )

kde4_automoc(${kmag_SRCS})

kde4_add_executable(kmag ${kmag_SRCS})

target_link_libraries(kmag  ${QT_AND_KDECORE_LIBS} kdeprint )

install_targets(/bin kmag )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kmag/icons FILES  )
install_files( ${APPLNK_INSTALL_DIR}/Applications FILES kmag.desktop )
install_files( ${DATA_INSTALL_DIR}/kmag FILES kmagui.rc )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#bin_PROGRAMS = kmag
#
### INCLUDES were found outside kdevelop specific part
#
#kmag_SOURCES = kmagzoomview.cpp kmagselrect.cpp kmag.cpp main.cpp 
#kmag_LDADD   = $(LIB_KDEPRINT)
#
#
#EXTRA_DIST = main.cpp version.h kmag.cpp kmag.h kmagui.rc kmag.desktop lo32-app-kmag.png lo16-app-kmag.png kmagselrect.cpp kmagselrect.h hi16-app-kmag.png kmagzoomview.cpp kmagzoomview.h 
#
#uninstall-local:
#	-rm -f $(DESTDIR)$(kde_appsdir)/Applications/kmag.desktop
#
#KDE_ICON = kmag 
#
#kmagdir = $(kde_appsdir)/Applications
#kmag_DATA = kmag.desktop
#
#kmagiconsdir = $(kde_datadir)/kmag/icons
#kmagicons_ICON = followmouse hidemouse window
#
## set the include path for X, qt and KDE
#INCLUDES= $(all_includes)
#
#METASOURCES = AUTO
#
#KDE_OPTIONS = nofinal
#
## the library search path. 
#kmag_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#rcdir = $(kde_datadir)/kmag
#rc_DATA = kmagui.rc
#
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kmag.pot; \
#	fi
#
