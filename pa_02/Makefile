
CPP        = g++
CPPFLAGS   = -Wall -g -c -I src/ -I $(NS3_INC_DIR)

NS3_INC_DIR = /usr/local/include/ns3.14.1/
NS3_LIB_DIR = /usr/local/lib/

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

SIMULATION    = bin/simulation
SIMULATION_SRC    = src/simulator/simulation.o


default: $(SIMULATION)

$(SIMULATION): $(SIMULATION_SRC)
	$(CPP) -o $@  $(SIMULATION_SRC) -L$(NS3_LIB_DIR) $(NS3_LIBS)

%.o: %.cc
	$(CPP) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o src/*/*.o  *.pcap *~  2> /dev/null

run_simulation: $(SIMULATION)
	make run_simulation_newreno
	
run_simulation_tahoe: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(SIMULATION) --tcpVersion=Tahoe
	
run_simulation_reno: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(SIMULATION) --tcpVersion=Reno
	
run_simulation_newreno: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(SIMULATION) --tcpVersion=NewReno


valgrind: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) valgrind --tool=memcheck --leak-check=yes ./$(SIMULATION)
	
plot_server_txrx: $(SIMULATION)
	rm -rf plotdata/* 2>/dev/null
	touch plotdata/.gitkeep
	mkdir plotdata/Tahoe plotdata/Reno plotdata/NewReno
	make run_simulation_tahoe
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	mv plotdata/*.data plotdata/Tahoe
	mv plotdata/*.png plotdata/Tahoe
	make run_simulation_reno
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	mv plotdata/*.data plotdata/Reno
	mv plotdata/*.png plotdata/Reno
	make run_simulation_newreno
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	mv plotdata/*.data plotdata/NewReno
	mv plotdata/*.png plotdata/NewReno


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

