####### kdevelop will overwrite this part!!! (begin)##########

#SUBDIRS = HTML

EXTRA_DIST = index.docbook 

install-data-local:
	$(mkinstalldirs) $(kde_htmldir)/en/
	$(INSTALL_DATA) $(srcdir)/index.docbook $(kde_htmldir)/en/index.docbook

uninstall-local:
	-rm -f $(kde_htmldir)/en/index.docbook

####### kdevelop will overwrite this part!!! (end)############

KDE_LANG = en
KDE_DOCS = kmag

