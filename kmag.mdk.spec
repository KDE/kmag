%define name kmag
%define version 0.6
%define release 1mdk

Name: 		%{name}
Version: 	%{version}
Release: 	%{release}
Summary:  	KDE Screen Magnifier
Group: 		Graphical desktop/KDE
License: 	GPL
Source: 	%{name}-%{version}.tar.gz
URL:		http://kmag.sourceforge.net
Buildroot: 	%_tmppath/%{name}-%{version}-buildroot
BuildRequires:  gcc-cpp db3 readline pam popt libtermcap2 zlib1 XFree86-devel XFree86-libs Mesa-common
BuildRequires:  automake arts autoconf diffutils kdelibs file kdesupport-devel
BuildRequires:  m4 texinfo gettext libjpeg62-devel kdelibs-devel gcc-c++ libbinutils2 libbzip2_1
BuildRequires:  db1 freetype2 libjpeg62 libmng1 libpng libqt2 libqt2-devel rpm-build


%description
KMagnifier is a screen magnifier for KDE. It is useful for
people with bad sight or others needing to magnify a part of the screen.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -n %{name}-%{version}

%build
CFLAGS="%optflags" CXXFLAGS="%optflags" \
           ./configure --prefix=%_prefix \
	               --disable-rpath \
		       --disable-debug

%make

%install
make DESTDIR=%buildroot install

install -d $RPM_BUILD_ROOT%_menudir

kdedesktop2mdkmenu.pl kmag "Applications/Accessibility" %buildroot/%_datadir/applnk/Applications/Accessibility/kmag.desktop %buildroot/%_menudir/kmag


%clean
rm -rf $RPM_BUILD_ROOT

%post 
%update_menus

%postun 
%clean_menus

%files 
%defattr(-,root,root)
%_bindir/*
%_menudir/*
#
%_datadir/icons/locolor/16x16/apps/*.png
%_datadir/icons/locolor/32x32/apps/*.png 
%_datadir/icons/hicolor/16x16/apps/*.png
#
%_datadir/applnk/Applications/Accessibility/*.desktop 
#
%dir %_datadir/apps/kmag/
%_datadir/apps/kmag/*.rc
#
%doc README NEWS COPYING AUTHORS
#%doc %_docdir/HTML/en/*.docbook
#%doc %_docdir/HTML/en/kmag/*.docbook
#%doc %_docdir/HTML/en/kmag/common

%changelog
* Tue Nov 20 2001 Sarang Lakare <sarang@users.sf.net> 0.6-1mdk
- Rebuilding ver 0.6 for LM8.0

* Sat Oct 20 2001 Laurent MONTEL <lmontel@mandrakesoft.com> 0.5-2mdk
- Rebuild against libpng3

* Wed Sep 19 2001 Laurent MONTEL <lmontel@mandrakesoft.com> 0.5-1mdk
- update code (0.5)

* Wed Aug 08 2001 Lenny Cartier <lenny@mandrakesoft.com> 0.3-6mdk
- rebuild

* Tue Apr 10 2001 David BAUDENS <baudens@mandrakesoft.com> 0.3-5mdk
- Move KDE menu entriy in %%_datadir/applnk
- Fix and use optimizations
- Fix BuildRequires for non %%ix86 architectures
- Rebuild against latest GCC

* Fri Mar 30 2001 Laurent MONTEL <lmontel@mandrakesoft.com> 0.3-4mdk
- Fix typo in menu entry

* Wed Mar 28 2001 Laurent MONTEL <lmontel@mandrakesoft.com> 0.3-3mdk
- Add build requires

* Tue Mar 13 2001 Laurent MONTEL <lmontel@mandrakesoft.com> 0.3-2mdk
- REbuild with qt-2.3

* Sat Feb 24 2001 Daouda Lo <daouda@mandrakesoft.com> 0.3-1mdk
- first mdk package 
