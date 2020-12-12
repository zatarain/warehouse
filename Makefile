.POSIX:
# Recomended variables/macros for installation
DESTDIR    =
PREFIX     = /usr
BINDIR     = ${DESTDIR}${PREFIX}/bin
LIBDIR     = ${DESTDIR}${PREFIX}/lib
INCLUDEDIR = ${DESTDIR}${PREFIX}/include
DATADIR    = ${DESTDIR}${PREFIX}/share
MANDIR     = ${DATADIR}/man
SRCDIR     = src
CXX        = clang++ -std=c++17 -stdlib=libc++

# Macro/variables specific for the library
SOURCES  = `find ${SRCDIR}/ -type f -name "*.cpp"`
BUILD      = out
CXXFLAGS   = -Wall -Wno-comment -fPIC -O2 -pipe -I${SRCDIR}/
LXXFLAGS   =
COVFLAGS   = -fcoverage-mapping -fprofile-instr-generate -g -O0
LUTFLAGS   = ${COVFLAGS} -shared
APPNAME    = warehouse

.SILENT: clean install uninstall

.DEFAULT:
	@echo "Defaulting $@..."
	@chmod +x Default.mk
	@./Default.mk "$@"

${BUILD}/${APPNAME}: ${OBJECTS}
	mkdir -p ${BUILD}/
	@if [ -z "${OBJECTS}" ]; then \
		make .configuration:$@;\
	else \
		echo "${CXX} ${CXXFLAGS} ${LXXFLAGS} ${OBJECTS} -o $@";\
		${CXX} ${CXXFLAGS} ${LXXFLAGS} ${OBJECTS} -o $@;\
	fi

${BUILD}/lib${APPNAME}.so:
	echo "Compiling application under test..."
	mkdir -p ${BUILD}/models
	for source in ${SOURCES}; do\
		object=${@D}/$${source#*/};\
		object=$${object%.*}.o;\
		output=$${object%/*};\
		mkdir -p $$output;\
		echo ${CXX} ${CXXFLAGS} ${COVFLAGS} -c $$source -o $$object;\
		${CXX} ${CXXFLAGS} ${COVFLAGS} -c $$source -o $$object;\
		objects="$$objects $$object";\
	done; ${CXX} ${LUTFLAGS} $$objects -o $@
	echo "Surccessfully compiled application under test library!"

install: ${BUILD}/${APPNAME}
	echo "We are about to install the library."
	cp -v ${BUILD}/${APPNAME} ${BINDIR}/
	chmod +x ${BINDIR}/${APPNAME}
	echo "The library was successfully installed!"

clean:
	rm -rf ${BUILD}/

uninstall:
	echo "Uninstalling the library..."
	rm -rfv ${BINDIR}/${APPNAME}
	echo "The library was successfully uninstalled!"
