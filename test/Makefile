LIBNAME=rf24-sunxi
LIBDIR=../lib/

all:pingtest scanner arduino_rcv

pingtest:
	g++ pingtest.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o pingtest

scanner:
	g++ scanner.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o scanner

arduino_rcv:
	g++ arduino_rcv.cpp -I../src/ -L${LIBDIR} -l${LIBNAME} -o arduino_rcv

clean:
	rm -f scanner pingtest arduino_rcv
