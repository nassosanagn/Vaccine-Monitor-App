GPP = g++
CFLAGS =  -g

travelMonitorClient: clean travelMonitorClient.o Monitor.o ReadFile.o hash.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o Commands.o requestsList.o bloomList.o PipeFunctions.o
	$(GPP) $(CFLAGS) -o travelMonitorClient travelMonitorClient.o BloomFilter.o hash.o CountryList.o Commands.o bloomList.o Date.o requestsList.o PipeFunctions.o 
	$(GPP) $(CFLAGS) -o Monitor Monitor.o ReadFile.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o PipeFunctions.o

.PHONY: clean

clean:
	rm -rf *.o travelMonitorClient Monitor
