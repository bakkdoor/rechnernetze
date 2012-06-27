

NS3_INC_DIR = /usr/local/include/ns3.14.1/
NS3_LIB_DIR = /usr/local/lib/


# NS3_INC_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/
# NS3_LIB_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/


CPP        = g++
CPPFLAGS   = -Wall -g -c -I src/ -I $(NS3_INC_DIR)

NS3_LIBS = -lns3.14.1-core-debug -lns3.14.1-network-debug -lns3.14.1-internet-debug -lns3.14.1-point-to-point-debug -lns3.14.1-applications-debug -lns3.14.1-bridge-debug -lns3.14.1-mpi-debug -lns3.14.1-stats-debug -lns3.14.1-csma-debug

TCP_SEND_APP_SRC = src/simulator/tcpsendapplication.o

MYFIRST	   = bin/first
MYFIRST_SRC = src/first.o

MYFIFTH	   = bin/fifth
MYFIFTH_SRC = src/fifth.o

SIMULATION    = bin/simulation
SIMULATION_SRC    = src/simulator/simulation.o

default: $(SIMULATION)

$(MYFIRST): $(MYFIRST_SRC)
	$(CPP) -o $@  $(MYFIRST_SRC) -L$(NS3_LIB_DIR) $(NS3_LIBS)
	
$(MYFIFTH): $(MYFIFTH_SRC)
	$(CPP) -o $@  $(MYFIFTH_SRC) -L$(NS3_LIB_DIR) $(NS3_LIBS)

$(SIMULATION): $(SIMULATION_SRC)
	$(CPP) -o $@  $(SIMULATION_SRC) -L$(NS3_LIB_DIR) $(NS3_LIBS)

%.o: %.cc
	$(CPP) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o src/*/*.o  *.pcap *~  2> /dev/null

run_first: $(MYFIRST)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(MYFIRST)
	
run_fifth: $(MYFIFTH)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(MYFIFTH)

run_simulation: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(SIMULATION)


vallgrind: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) valgrind --tool=memcheck --leak-check=yes ./$(SIMULATION)



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

