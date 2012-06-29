

NS3_INC_DIR = /usr/local/include/ns3.14.1/
NS3_LIB_DIR = /usr/local/lib/


# NS3_INC_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/
# NS3_LIB_DIR = /home/${USER}/workspace/ns-allinone-3.14.1/ns-3.14.1/build/


CPP        = g++
CPPFLAGS   = -Wall -g -c -I src/ -I $(NS3_INC_DIR)

NS3_LIBS = -lns3.14.1-core-debug -lns3.14.1-network-debug -lns3.14.1-internet-debug \
					 -lns3.14.1-point-to-point-debug -lns3.14.1-applications-debug -lns3.14.1-bridge-debug \
					 -lns3.14.1-mpi-debug -lns3.14.1-stats-debug -lns3.14.1-csma-debug
	
#NS3_LIBS_ALL = -lns3.14.1-antenna-debug -lns3.14.1-aodv-debug -lns3.14.1-applications-debug \
#  -lns3.14.1-bridge-debug -lns3.14.1-buildings-debug \
#  -lns3.14.1-config-store-debug -lns3.14.1-core-debug -lns3.14.1-csma-debug -lns3.14.1-csma-layout-debug -lns3.14.1-dsdv-debug \
#  -lns3.14.1-dsr-debug -lns3.14.1-emu-debug -lns3.14.1-energy-debug -lns3.14.1-flow-monitor-debug -lns3.14.1-internet-debug \
#  -lns3.14.1-lte-debug -lns3.14.1-mesh-debug -lns3.14.1-mobility-debug -lns3.14.1-mpi-debug -lns3.14.1-netanim-debug \
#  -lns3.14.1-network-debug -lns3.14.1-nix-vector-routing-debug -lns3.14.1-olsr-debug -lns3.14.1-point-to-point-debug \
#  -lns3.14.1-point-to-point-layout-debug -lns3.14.1-propagation-debug -lns3.14.1-spectrum-debug -lns3.14.1-stats-debug \
#  -lns3.14.1-tap-bridge-debug -lns3.14.1-test-debug -lns3.14.1-tools-debug -lns3.14.1-topology-read-debug \
#  -lns3.14.1-uan-debug -lns3.14.1-virtual-net-device-debug -lns3.14.1-wifi-debug -lns3.14.1-wimax-debug

MYFIRST	   = bin/first
MYFIRST_SRC = src/first.o

MYFIFTH	   = bin/fifth
MYFIFTH_SRC = src/fifth.o

SIMULATION    = bin/simulation
SIMULATION_SRC    = src/simulator/simulation.o

PLOT_DATA_FILE = plotdata/simulation.plot

GNUPLOT_SETTINGS = set autoscale xy; set offset graph 0, graph 0, graph 0.1, graph 0.1; set title 'My Plot'; set output 'plotdata/plot.ps'; set terminal postscript;
GNUPLOT_CLIENT_SENT = using 1:2 title 'Client Sent' with linespoint
GNUPLOT_CLIENT_RECEIVED = using 1:3 title 'Client Received' with linespoint
GNUPLOT_SERVER_SENT = using 1:4 title 'Server Sent' with linespoint
GNUPLOT_SERVER_RECEIVED = using 1:5 title 'Server Received' with linespoint

GNUPLOT_CMD = '$(PLOT_DATA_FILE)' $(GNUPLOT_CLIENT_SENT), \
							'$(PLOT_DATA_FILE)' $(GNUPLOT_CLIENT_RECEIVED), \
							'$(PLOT_DATA_FILE)' $(GNUPLOT_SERVER_SENT), \
							'$(PLOT_DATA_FILE)' $(GNUPLOT_SERVER_RECEIVED)

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


valgrind: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) valgrind --tool=memcheck --leak-check=yes ./$(SIMULATION)

prepare_plot: $(SIMULATION)
	mkdir -p plotdata
	./prepare_plot.rb simulation

plot: prepare_plot
	gnuplot -e "$(GNUPLOT_SETTINGS); plot $(GNUPLOT_CMD)"


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

