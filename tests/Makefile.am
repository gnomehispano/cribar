check_SCRIPTS = run_behave.sh
TESTS = $(check_SCRIPTS)

noinst_PROGRAMS = $(TESTS)

run_behave.sh:
	echo 'CRIBAR_APP=$(top_srcdir)/src/cribar behave' > run_behave.sh
	chmod +x run_behave.sh

CLEANFILES = run_behave.sh

-include $(top_srcdir)/git.mk
