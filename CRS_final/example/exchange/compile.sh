gcc 	../../src/EllipticCurves/memory.c \
	../../src/EllipticCurves/models.c \
	../../src/EllipticCurves/pretty_print.c \
	../../src/EllipticCurves/arithmetic.c \
	../../src/EllipticCurves/auxiliary.c \
	../../src/Polynomials/binary_trees.c \
	../../src/Polynomials/multieval.c \
	../../src/Polynomials/roots.c \
	../../src/Isogeny/radical.c \
	../../src/Isogeny/velu.c \
	../../src/Isogeny/walk.c \
	../../src/Exchange/setup.c \
	../../src/Exchange/keygen.c \
	../../src/Exchange/dh.c \
	../../src/Exchange/info.c \
	exchange.c \
	-g -pg  -O3  $1 $2 -lgmp -lflint -o exchange
