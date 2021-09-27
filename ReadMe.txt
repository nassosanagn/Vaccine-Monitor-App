Προγραμματισμός Συστήματος
3η Άσκηση

Αναγνωστόπουλος Αθανάσιος
ΑΜ: 1115201800006

Μεταγλώττιση και Εκτέλεση

Αρχικά υπάρχει ένα αρχείο Makefile για την διευκόλυνση της μεταγλώττισης και της εκτέλεσης του προγράμματος. Η εντολή make δημιουργεί το αρχείο travelMonitorClient.
Το πρόγραμμα δέχεται έξι ορίσματα, τον αριθμό των monitorServer διεργασιών που θα δημιουργήσει η εφαρμογή (numMonitors), το μέγεθος του buffer για διάβασμα απο 
τα sockets (socketBufferSize), το μεγεθός του κυκλικού buffer (cyclicBufferSize), το μέγεθος του bloom Filter (sizeOfBloom), το directory το οποίο περιέχει τα αρχεία 
που θα επεξεργάζονται τα Monitors (input_dir) και τέλος τον αριθμό των threads που θα δημιουργήσει κάθε monitorServer (numThreads) με οποιαδήποτε σειρά.

Ο αριθμός τον Monitors δίνεται μετά απο το πρόθεμα -m, το socketBufferSize μετα απο το πρόθεμα -b, το cyclicBufferSize μετά από το πρόθεμα -c, 
το sizeOfBloom μετά από το πρόθεμα -s, το όνομα του directory μετά από το -i και τέλος το numThreads μετά απο το -t.

π.χ ./travelMonitorClient -m 10 -b 50 -c 40 -s 100000 -i ./countries -t 4
Τέλος, υπάρχει και η εντολή make clean για την γρήγορη διαγραφή όλων των .o αρχείων καθώς και των εκτελέσιμων αρχείων travelMonitorClient και monitorServer.

Αρχεία και δομές

Τα αρχεία είναι τα ίδια με της δεύτερης εργασίας με την μόνη διαφορά ότι προστέθηκε το αρχείο SocketFunctions.cpp και SocketFunctions.h.

Δηλαδή υπάρχουν τα αρχεία BloomFilter.cpp, SkipList.cpp, VirusList.cpp, CitizenList.cpp, CountryList.cpp, Date.cpp, hash.cpp, ReadFile.cpp,
VaccinationInfoList.cpp, bloomList.cpp, Commands.cpp, monitorServer.cpp, requestsList.cpp και travelMonitorClient.cpp καθώς και τα αντίστοιχα .h 
αρχεία απο την δεύτερη εργασία μαζί με κάποιες επιπλέον συναρτήσεις όπως π.χ. την συνάρτηση getAllCountries στο αρχείο CountryList.cpp.

Το αρχείο Commands.cpp περιέχει τις 5 διαφορετικές εντολές που μπορεί να δώσει ο χρήστης, δηλαδή τον έλεγχο για την κάθε μία και την υλοποίηση τους.
Στο αρχείο requestsList.cpp υλοποιείται μια λίστα με requests και αποθηκεύει για κάθε ένα την ημερομηνία του, την χώρα για την οποία έγινε 
(countryFrom) καθώς και έναν request counter. 

Το αρχείο SocketFunctions.cpp περιέχει τις συναρτήσεις readInt, readString και readBloom για το διάβασμα ακεραίου, συμβολοσειράς και bloomFilter αντίστοιχα
απο ένα socket λαμβάνοντας υπόψην το socketBufferSize. Ακόμα περιέχει τις συναρτήσεις sendInt, sendString και sendBloom για την αποστολή ακεραίου, συμβολοσειράς 
και bloomFilter αντίστοιχα μέσω ενός socket λαμβάνοντας υπόψην το socketBufferSize.

travelMonitorClient

Το αρχείο travelMonitorClient.cpp ξεκινώντας κάνει έλεγχο για τα ορίσματα που του έχουν δοθεί. Αρχικά ανοίγει το input directory, παίρνει τον αριθμό των χωρών που
περιέχει και αποθηκεύει στην δομή monitorInfo τις χώρες που θα αναθέσει σε κάθε monitorServer (με round-robin κατανομή).

Στην συνέχεια κάνει fork numMonitors child proccesses (τα monitorServers) και καλεί την execvp δίνοντας σαν όρισματα σε κάθε monitorServer τον αριθμό του port 
για επικοινωνία με το parent proccess (ο οποίος έχει οριστεί με #define), τον αριθμό των threads που θα δημιουργήσει, το socketBufferSize, το cyclicBufferSize, 
το sizeOfBloom και τέλος τα paths για τις χώρες τις οποίες έχει ανατεθεί να εξυπηρετήσει το συγκεκριμένο monitorServer και κρατάει στην δομή monitorInfo τα pid 
για κάθε child proccess (δηλαδή monitorServer). 

Έπειτα το travelMonitorClient δημιουργεί τα sockets και αποθηκεύει στην δομή monitorInfo το sockedFd που αντιστοιχεί σε κάθε monitorServer.
Ακόμα το travelMonitorClient διαβάζει (χρησιμοποιώντας την select) από κάθε monitorServer τα bloomFilters που θα του επιστρέφει και τα ενώνει δημιουργώντας ένα
bloomFilter για κάθε ιό.

Τέλος καλεί την συνάρτηση Commands και εκτελεί τις εντολές που θα δωθούν από τον χρήστη μέχρι να δωθεί η εντολή "/exit" όπου απελευθερώνει ότι έχει δεσμευθεί δυναμικά 
και δημιουργεί το αρχείο log_file για το parent proccess καθώς και ένα αρχείο log_file για κάθε monitorServer.

monitorServer

Το αρχείο monitorServer.cpp κάνει έλεγχο για τα ορίσματα και δημιουργεί τα sockets για την επικοινωνία με το travelMonitorClient.

Στην συνέχεια αποθηκεύει σε ένα array (txtFilePaths) τα paths (τα οποία δέχεται ως όρισμα) για όλα τα αρχεία .txt που περιέχουν τα records των χωρών που του έχουν δοθεί.
Έπειτα ξεκινάει numThreads threads κάθε ένα απο τα οποία θα αφαιρεί απο το txtFilePaths ένα αρχείο, θα το διαβάζει και θα ενημερώνει τις δομές δεδομένων (VirusList, 
CitizenList, CountryList καθώς και τα BloomFilters και SkipLists για κάθε ιό) οι οποίες είναι κοινόχρηστες για όλα τα threads. 

Αφού τελειώσει η ανάγνωση των input files απο τα threads, το monitorServer στέλνει μέσω socket στο parent proccess (travelMonitorClient) ένα bloomFilter για κάθε ιό, τα οποία
ενώνονται στο travelMonitorClient μαζί με τα bloomFilters που έχουν σταλεί απο τα υπόλοιπα monitorServers και έτσι δημιουργείται μία λίστα απο bloomFilters απο τα οποία 
κάθε ένα αναφέρεται σε συγκεκριμένο ιό.

Τέλος το κάθε monitorServer ειδοποιεί το parent proccess πως είναι έτοιμο να δεχθεί αιτήματα και αναλόγως την εντολή που θα δεχθεί λειτουργεί όπως περιγράφεται παρακάτω.

Εντολές στο αρχείο Commands.cpp

Command 1 - /travelRequest citizenID date countryFrom countryTo virusName 
Η λειτουργία είναι ακριβώς ίδια με την 2η εργασία με την μόνη διαφορά στο γεγονός ότι η επικοινωνία γίνεται πάνω απο socket.
Αρχικά γίνεται έλεγχος για τον αν ο ιός και η χώρα που έχει δοθεί υπάρχουν η όχι. Στην συνέχεια ελέγχει το bloomFilter του συγκεκριμένου ιού για να δει αν υπάρχει 
ο πολίτης και αν όχι εκτυπώνει σχετικό μήνυμα. Αν υπάρχει στο bloomFilter στέλνει εντολή στο monitorServer που του έχει ανατεθεί το συγκεκριμένο country "countryFrom" 
για να εντοπίσει στην SkipList του συγκεκριμένου ιού αν υπάρχει ο πολίτης. Αν δεν υπάρχει επιστρέφει ως απάντηση "NO" και εκτυπώνει σχετικό μήνυμα. Αν υπάρχει επιστρέφει
"YES" καθώς και την ημερομηνία εμβολιασμού του. Στη συνέχεια ελέγχει αν έχει εμβολιαστεί λιγότερο από 6 μήνες πριν την ημερομηνία που θέλει να ταξιδέψει και αν ναι εκτυπώνει 
"HAPPY TRAVELS" αλλιώς εκτυπώνει "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE". Σε κάθε περίπτωση προσθέτει το request στην λίστα με τα requests 
και ενημερώνει τον αντίστοιχο request counter.

Command 2 - /travelStats virusName date1 date2 [country]
Η λειτουργία είναι ακριβώς ίδια με την 2η εργασία με την μόνη διαφορά στο γεγονός ότι η επικοινωνία γίνεται πάνω απο socket.
Αρχικά γίνεται έλεγχος για τον αν ο ιός και η χώρα που έχει δοθεί υπάρχουν η όχι και στην συνέχεια εκτυπώνει τον αριθμό των συνολικών requests καθώς και των requests που
έχουν εγκριθεί και απορριφθεί χρησιμοποιώντας τις συνάρτησεις BloomListGetTotalRequests, BloomListGetAcceptRequests και BloomListGetRejectRequests αντίστοιχα λαμβάνοντας υπόψην
τις ημερομηνίες που έχουν δοθεί. Ακόμα στην περίπτωση που δοθεί country σαν όρισμα καλούνται οι ίδιες συνατήσεις αλλά αυτή την φορά με την χώρα σαν όρισμα ώστε να επιστρέψουν 
τον αριθμό των requests μόνο για την συγκεκριμένη χώρα.

Command 3 - /addVaccinationRecords country
Αρχικά γίνεται έλεγχος για τον αν η χώρα που έχει δοθεί υπάρχει η όχι και στη συνέχεια στέλνεται στο monitorServer που του έχει ανατεθεί η χώρα country η συμβολοσειρά 
"/addVaccinationRecords". Το monitorServer με την σειρά του βάζει στον κυκλικό buffer τα νέα αρχεία με εγγραφές τα οποία θα διαβαστούν απο τα numThreads threads (όπως 
προηγουμένως) και θα ενημερώσουν όλες τις δομές. Τέλος το αρχικό thread θα επιστρέφει τα ενημερωμένα bloom filters πίσω στο parent proccess.

Command 4 - /searchVaccinationStatus citizenID
Η λειτουργία είναι ακριβώς ίδια με την 2η εργασία με την μόνη διαφορά στο γεγονός ότι η επικοινωνία γίνεται πάνω απο socket.
Αρχικά γίνεται έλεγχος για το αν το citizenID που έχει δοθεί υπάρχει η όχι. Αν υπάρχει στέλνεται σε κάθε monitorServer το citizenID και εκείνο επιστρέφει ότι πληροφορία έχει
για τον συγκεκριμένο πολίτη καθώς και για τους εμβολιασμούς του. Η πληροφορία εκτυπώνεται στο αρχείο Commands.cpp.

Command 5 - /exit
Η εντολή αυτή επιστρέφει 1 στο travelMonitorClient ώστε να σταματήσει το loop με τις εντολές και διαγράφει τα countryLists που υπάρχουν στην δομή monitorInfo και δείχνουν 
ποιες χώρες έχουν ανατεθεί σε κάθε monitorServer καθώς και την λίστα με τα bloom filters. Ακόμα στέλνει την συμβολοσειρά "/exit" σε κάθε monitorServer (μέσω socket) ώστε να 
απελευθερώνει την δεσμευμένη μνήμη και δημιουργεί το αρχείο log_file για το συγκεκριμένο monitorServer. Τέλος το αρχείο travelMonitorClient.cpp περιμένει να τελειώσουν 
όλα τα child proccesses, κλείνει τα sockets και δημιουργεί το δικό του log_file αρχείο.