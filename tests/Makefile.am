bin_PROGRAMS = \
	idiot-test \
	cli-test \
	completion-test \
	flex-test \
	invoke-test

idiot_test_SOURCES = \
	idiot-test.c

idiot_test_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_builddir)/libidiot

idiot_test_LDADD = \
	$(GLIB_LIBS) \
	$(top_builddir)/libidiot/libidiot.la

cli_test_SOURCES = \
	cli-test.c

cli_test_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(READLINE_CFLAGS)

cli_test_LDADD = \
	$(GLIB_LIBS) \
	$(READLINE_LIBS)

completion_test_SOURCES = \
	completion-test.c

completion_test_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(READLINE_CFLAGS)

completion_test_LDADD = \
	$(GLIB_LIBS) \
	$(READLINE_LIBS)

flex-test.c: lex.yy.c
	$(LEX) flex-test.lex

flex_test_SOURCES = \
	lex.yy.c

flex_test_CFLAGS = \
	$(FLEX_CFLAGS)

flex_test_LDADD = \
	$(FLEX_LIBS)


invoke_test_SOURCES = \
	invoke-test.c

invoke_test_CFLAGS = \
	$(GLIB_CFLAGS)

invoke_test_LDADD = \
	$(GLIB_LIBS)

