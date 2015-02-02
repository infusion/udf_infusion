CLEANFILES=*.log *.reject *.result example_table
EXTRA_DIST=*.test *.sql test2result generate_random_data

MYSQL=mysql
MYSQLTEST=mysqltest
ALL_TESTS=$(wildcard *.test)

test_prepare: test_prepare_start example_table test_prepare_sql test_results
	@echo "$@: Test environment ready!"
	@echo "$@: You can now run \`make test\`"

test_prepare_start:
	@echo "$@: Preparing test environment (this may take a while)..."

test_prepare_sql:
	$(MYSQL) --local-infile=1 $(MYSQL_OPTIONS) < test_prepare.sql

example_table:
	./generate_random_data 1000000 > example_table

test_results: $(ALL_TESTS:%.test=%.result)

%.result: %.test
	./test2result < $*.test > $@

test_clean:
	$(MYSQL) $(MYSQL_OPTIONS) < test_clean.sql

%.run : %.test
	$(MYSQLTEST) $(MYSQL_OPTIONS) -R $*.result < $*.test

test: $(ALL_TESTS:%.test=%.run)

.PHONY: test_prepare test_prepare_start test_prepare_sql test_results test
