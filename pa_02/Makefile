
CPP        = g++
CPPFLAGS   = -Wall -g -c -I src/ -I $(NS3_INC_DIR)

NS3_INC_DIR = /usr/local/include/ns3.14.1/
NS3_LIB_DIR = /usr/local/lib/

NS3_LIBS = -lns3.14.1-core-debug -lns3.14.1-network-debug -lns3.14.1-internet-debug \
					 -lns3.14.1-point-to-point-debug -lns3.14.1-applications-debug -lns3.14.1-bridge-debug \
					 -lns3.14.1-mpi-debug -lns3.14.1-stats-debug -lns3.14.1-csma-debug -lns3.14.1-config-store-debug

SIMULATION    = bin/simulation
SIMULATION_SRC    = src/simulator/simulation.o
TCP_SEND_APP_SRC = src/simulator/tcpsendapplication.o

default: $(SIMULATION)
	mkdir -p plotdata

$(SIMULATION): $(SIMULATION_SRC) $(TCP_SEND_APP_SRC)
	$(CPP) -o $@  $(SIMULATION_SRC) $(TCP_SEND_APP_SRC) -L$(NS3_LIB_DIR) $(NS3_LIBS)

%.o: %.cc
	$(CPP) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o src/*/*.o *.pcap output-attributes.txt *~  2> /dev/null

run_simulation: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) ./$(SIMULATION) --tcp=$(TCP) --looseMorePackets=$(LOOSE_MORE)

valgrind: $(SIMULATION)
	LD_LIBRARY_PATH=$(NS3_LIB_DIR) valgrind --tool=memcheck --leak-check=yes ./$(SIMULATION) --tcp=NewReno

plot_server_traces: $(SIMULATION)
	rm -rf plotdata/* 2>/dev/null
	mkdir plotdata/Tahoe plotdata/Reno plotdata/NewReno
	mkdir plotdata/Tahoe/11 plotdata/Reno/11 plotdata/NewReno/11
	mkdir plotdata/Tahoe/11-16-22 plotdata/Reno/11-16-22 plotdata/NewReno/11-16-22
	make run_simulation TCP=Tahoe LOOSE_MORE=no
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/Tahoe/11
	mv plotdata/*.png plotdata/Tahoe/11
	make run_simulation TCP=Reno LOOSE_MORE=no
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/Reno/11
	mv plotdata/*.png plotdata/Reno/11
	make run_simulation TCP=NewReno LOOSE_MORE=no
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/NewReno/11
	mv plotdata/*.png plotdata/NewReno/11
	make run_simulation TCP=Tahoe LOOSE_MORE=yes
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/Tahoe/11-16-22
	mv plotdata/*.png plotdata/Tahoe/11-16-22
	make run_simulation TCP=Reno LOOSE_MORE=yes
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/Reno/11-16-22
	mv plotdata/*.png plotdata/Reno/11-16-22
	make run_simulation TCP=NewReno LOOSE_MORE=yes
	gnuplot < plot-tx-rx.script
	gnuplot < plot-tx-rx-zoom.script
	gnuplot < plot-cw-ssth.script
	gnuplot < plot-cw-ssth-zoom.script
	mv plotdata/*.data plotdata/NewReno/11-16-22
	mv plotdata/*.png plotdata/NewReno/11-16-22


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

