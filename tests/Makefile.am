MAINTAINERCLEANFILES 					= Makefile.in

if BUILD_TESTS

noinst_PROGRAMS				= check_test

TESTS					= check_test.c 				\
					  check_utils.c 			\
					  check-hildon-caption.c 		\
					  check-hildon-helper.c 		\
					  check-hildon-note.c 			\
					  check-hildon-banner.c 		\
					  check-hildon-wizard-dialog.c 		\
					  check-hildon-find-toolbar.c 		\
					  check-hildon-window.c 		\
					  check-hildon-program.c


DEPRECATED_TESTS			= check-hildon-range-editor.c 		\
					  check-hildon-date-editor.c		\
					  check-hildon-time-editor.c 		\
					  check-hildon-weekday-picker.c 	\
					  check-hildon-controlbar.c 		\
					  check-hildon-seekbar.c 		\
					  check-hildon-time-picker.c 		\
					  check-hildon-number-editor.c  	\
					  check-hildon-calendar-popup.c 	\
					  check-hildon-code-dialog.c 		\
					  check-hildon-sort-dialog.c 		\
					  check-hildon-volumebar.c 		\
					  check-hildon-volumebar-range.c 	\
					  check-hildon-font-selection-dialog.c 	\
					  check-hildon-color-button.c		\
					  check-hildon-color-chooser.c

if HILDON_DISABLE_DEPRECATED
check_test_SOURCES   		 	= $(TESTS)
else
check_test_SOURCES   		 	= $(TESTS) 				\
			     		  $(DEPRECATED_TESTS)
endif

check_test_LDADD			= $(HILDON_OBJ_LIBS)
check_test_LDFLAGS			= -module -avoid-version
check_test_CFLAGS			= $(HILDON_OBJ_CFLAGS)

endif
