CATALOGS = @WPMOFILES@

nlsdir = $(NLSDIR)

CLEANFILES = WPrefs.pot $(CATALOGS)

EXTRA_DIST = bg.po ca.po cs.po de.po es.po et.po fi.po fr.po hr.po hu.po \
	it.po ja.po ko.po nl.po pt.po ru.po sk.po zh_CN.po zh_TW.po

POTFILES  = \
	$(top_srcdir)/WPrefs.app/Appearance.c \
	$(top_srcdir)/WPrefs.app/Configurations.c \
	$(top_srcdir)/WPrefs.app/Docks.c \
	$(top_srcdir)/WPrefs.app/Expert.c \
	$(top_srcdir)/WPrefs.app/Focus.c \
	$(top_srcdir)/WPrefs.app/FontSimple.c \
	$(top_srcdir)/WPrefs.app/Icons.c \
	$(top_srcdir)/WPrefs.app/KeyboardSettings.c \
	$(top_srcdir)/WPrefs.app/KeyboardShortcuts.c \
	$(top_srcdir)/WPrefs.app/Menu.c \
	$(top_srcdir)/WPrefs.app/MenuPreferences.c \
	$(top_srcdir)/WPrefs.app/MouseSettings.c \
	$(top_srcdir)/WPrefs.app/Paths.c \
	$(top_srcdir)/WPrefs.app/Preferences.c \
	$(top_srcdir)/WPrefs.app/TexturePanel.c \
	$(top_srcdir)/WPrefs.app/Themes.c \
	$(top_srcdir)/WPrefs.app/WPrefs.c \
	$(top_srcdir)/WPrefs.app/WindowHandling.c \
	$(top_srcdir)/WPrefs.app/Workspace.c \
	$(top_srcdir)/WPrefs.app/main.c

SUFFIXES = .po .mo

.po.mo: 
	msgfmt -c -o $@ $<


all-local: $(CATALOGS)

WPrefs.pot: $(POTFILES)
	xgettext --default-domain=WPrefs \
	--add-comments --keyword=_ --keyword=N_ $(POTFILES)
	if cmp -s WPrefs.po WPrefs.pot; then \
	    rm -f WPrefs.po; \
	else \
	    mv -f WPrefs.po WPrefs.pot; \
	fi


install-data-local:  $(CATALOGS)
	$(mkinstalldirs) $(DESTDIR)$(nlsdir)
	chmod 755 $(DESTDIR)$(nlsdir) 
	for n in $(CATALOGS) __DuMmY ; do \
		if test "$$n" -a "$$n" != "__DuMmY" ; then \
			l=`basename $$n .mo`; \
			$(mkinstalldirs) $(DESTDIR)$(nlsdir)/$$l/LC_MESSAGES; \
			chmod 755 $(DESTDIR)$(nlsdir)/$$l; \
			chmod 755 $(DESTDIR)$(nlsdir)/$$l/LC_MESSAGES; \
			$(INSTALL_DATA) -m 644 $$n $(DESTDIR)$(nlsdir)/$$l/LC_MESSAGES/WPrefs.mo; \
		fi; \
	done

