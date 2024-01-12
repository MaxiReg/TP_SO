# Libraries
LIBS=static commons pthread readline m

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=../static

# Compiler flags
CDEBUG=-g -Wall -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS=INTEGRAL_3 2048

# Valgrind flags
MEMCHECK_FLAGS=--track-origins=yes --log-file="log/memcheck-consola.log"
HELGRIND_FLAGS=--log-file="log/helgrind-consola.log"
