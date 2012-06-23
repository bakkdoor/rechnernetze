
CPP        = g++
CPPFLAGS=  = -Wall -g -c -I src/


MYFIRST	   = bin/first
MYFIRST_SRC = src/first.o
MYFIRST_LIBS = -lns3-core -lns3-network -lns3-internet -lns3-point-to-point -lns3-applications


default: $(MYFIRST)

$(MYFIRST): $(MYFIRST_SRC)
	$(CPP) -o $@  $(MYFIRST_SRC) $(MYFIRST_LIBS)

%.o: %.cc 
	$(CPP) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o *~  2> /dev/null
	



vallgrind: $(MYFIRST)
	valgrind --tool=memcheck --leak-check=yes ./$(MYFIRST) 

	
	
SRC_DIR="pa_01"
TARGET_DIR="release_pa_01"

release:
	cd .. && cp -rf ${SRC_DIR} ${TARGET_DIR} && cd ${TARGET_DIR} && \
	rm -rf `find . -name *git* ` && \
	rm -rf `find . -name nbproject* ` && \
	make && \
	tar -cvf ../ReNe_SoSe_2012_PA1_933230_933151.tar . -R && \
	cd .. && rm -rf ${TARGET_DIR}
	
