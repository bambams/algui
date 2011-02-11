#!/usr/bin/env bash

src_dir="`dirname $0`/src"
objs_count=`grep '\${OBJDIR}/algui.*\.o' Makefile | wc -l`
srcs_count=`find ${src_dir} -name \*.c | wc -l`

echo "Counted ${objs_count} object files in library and ${srcs_count} source files in '${src_dir}'."

