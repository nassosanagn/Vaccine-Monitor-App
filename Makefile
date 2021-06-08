GPP = g++
CFLAGS =  -g -pthread

travelMonitorClient: clean travelMonitorClient.o monitorServer.o ReadFile.o hash.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o Commands.o requestsList.o bloomList.o SocketFunctions.o
	$(GPP) $(CFLAGS) -o travelMonitorClient travelMonitorClient.o BloomFilter.o hash.o CountryList.o Commands.o bloomList.o Date.o requestsList.o SocketFunctions.o 
	$(GPP) $(CFLAGS) -o monitorServer monitorServer.o ReadFile.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o SocketFunctions.o

.PHONY: clean

clean:
	rm -rf *.o travelMonitorClient monitorServer
