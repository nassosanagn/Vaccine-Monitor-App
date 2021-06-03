GPP = g++
CFLAGS =  -g

travelMonitor: clean travelMonitor.o Monitor.o ReadFile.o hash.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o Commands.o requestsList.o bloomList.o PipeFunctions.o
	$(GPP) $(CFLAGS) -o travelMonitor travelMonitor.o BloomFilter.o hash.o CountryList.o Commands.o bloomList.o Date.o requestsList.o PipeFunctions.o 
	$(GPP) $(CFLAGS) -o Monitor Monitor.o ReadFile.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o PipeFunctions.o

.PHONY: clean

clean:
	rm -rf travelMonitor.o Monitor.o ReadFile.o hash.o CountryList.o CitizenList.o VirusList.o VaccinationInfoList.o SkipList.o hash.o Date.o BloomFilter.o Commands.o bloomList.o requestsList.o PipeFunctions.o travelMonitor Monitor
