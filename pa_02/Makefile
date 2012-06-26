

#NS3_INC_DIR = /usr/include/
#NS3_LIB_DIR = /usr/lib/


NS3_INC_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/
NS3_LIB_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/


CPP        = g++
CPPFLAGS   = -Wall -g -c -I src/ -I $(NS3_INC_DIR)

MYFIRST	   = bin/first
MYFIRST_SRC = src/first.o
#MYFIRST_LIBS = -lns3-core -lns3-network -lns3-internet -lns3-point-to-point -lns3-applications -lns3-bridge -lns3-mpi -lns3-stats
MYFIRST_LIBS = -lns3.14.1-core-debug -lns3.14.1-network-debug -lns3.14.1-internet-debug -lns3.14.1-point-to-point-debug -lns3.14.1-applications-debug -lns3.14.1-bridge-debug -lns3.14.1-mpi-debug -lns3.14.1-stats-debug

SIMULATION    = bin/simulation
SIMULATION_SRC    = src/simulator/simulation.o
SIMULATION_LIBS = -lns3.14.1-core-debug -lns3.14.1-network-debug -lns3.14.1-internet-debug -lns3.14.1-point-to-point-debug -lns3.14.1-applications-debug -lns3.14.1-bridge-debug -lns3.14.1-mpi-debug -lns3.14.1-stats-debug -lns3.14.1-csma-debug 


default: run_simulation

$(MYFIRST): $(MYFIRST_SRC)
	$(CPP) -o $@  $(MYFIRST_SRC) -L$(NS3_LIB_DIR) $(MYFIRST_LIBS)
	
$(SIMULATION): $(SIMULATION_SRC)
	$(CPP) -o $@  $(SIMULATION_SRC) -L$(NS3_LIB_DIR) $(SIMULATION_LIBS)	

%.o: %.cc 
	$(CPP) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o src/*/*.o  *~  2> /dev/null
	
run_first: $(MYFIRST)
	export LD_LIBRARY_PATH=$(NS3_LIB_DIR)
	./$(MYFIRST)
	
run_simulation: $(SIMULATION)
	export LD_LIBRARY_PATH=$(NS3_LIB_DIR)
	./$(SIMULATION)	


vallgrind: $(SIMULATION)
	valgrind --tool=memcheck --leak-check=yes ./$(SIMULATION) 

	
	
#SRC_DIR="pa_01"
#TARGET_DIR="release_pa_01"
#
#release:
#	cd .. && cp -rf ${SRC_DIR} ${TARGET_DIR} && cd ${TARGET_DIR} && \
#	rm -rf `find . -name *git* ` && \
#	rm -rf `find . -name nbproject* ` && \
#	make && \
#	tar -cvf ../ReNe_SoSe_2012_PA1_933230_933151.tar . -R && \
#	cd .. && rm -rf ${TARGET_DIR}
	
