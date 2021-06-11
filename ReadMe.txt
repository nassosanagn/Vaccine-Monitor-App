Προγραμματισμός Συστήματος
3η Άσκηση

Αναγνωστόπουλος Αθανάσιος
ΑΜ: 1115201800006

Μεταγλώττιση και Εκτέλεση

Αρχικά υπάρχει ένα αρχείο Makefile για την διευκόλυνση της μεταγλώττισης και της εκτέλεσης του προγράμματος. Η εντολή make δημιουργεί το αρχείο travelMonitorClient.
Το πρόγραμμα δέχεται έξι ορίσματα, τον αριθμό των monitorServe διεργασιών που θα δημιουργήσει η εφαρμογή (numMonitors), το μέγεθος του buffer για διάβασμα απο 
τα sockets (socketBufferSize), το μεγεθός του κυκλικού buffer (cyclicBufferSize), το μέγεθος του bloom Filter (sizeOfBloom), το directory το οποίο περιέχει τα αρχεία 
που θα επεξεργάζονται τα Monitors (input_dir) και τέλος τον αριθμό των threads που θα δημιουργήσει κάθε monitorServer (numThreads) με οποιαδήποτε σειρά.

Ο αριθμός τον Monitors δίνεται μετά απο το πρόθεμα -m, το socketBufferSize μετα απο το πρόθεμα -b, το cyclicBufferSize μετά από το πρόθεμα -c, 
το sizeOfBloom μετά από το πρόθεμα -s, το όνομα του directory μετά από το -i και τέλος το numThreads μετά απο το -t.

π.χ ./travelMonitorClient -m 10 -b 50 -c 40 -s 100000 -i ./countries -t 4
Τέλος, υπάρχει και η εντολή make clean για την γρήγορη διαγραφή όλων των .o αρχείων καθώς και των εκτελέσιμων αρχείων travelMonitorClient και monitorServer.

Αρχεία και δομές

Τα αρχεία είναι τα ίδια με της δεύτερης εργασίας με την μόνη διαφορά ότι προστέθηκε το αρχείο SocketFunctions.cpp και SocketFunctions.h. 
Δηλαδή υπάρχουν τα αρχεία BloomFilter.cpp, SkipList.cpp, VirusList.cpp, CitizenList.cpp, CountryList.cpp, Date.cpp, hash.cpp, ReadFile.cpp,
VaccinationInfoList.cpp, bloomList.cpp, Commands.cpp, monitorServer.cpp, PipeFunctions.cpp, requestsList.cpp και travelMonitorClient.cpp καθώς και
τα αντίστοιχα .h αρχεία απο την δεύτερη εργασία μαζί με κάποιες επιπλέον συναρτήσεις όπως π.χ. την συνάρτηση getAllCountries στο αρχείο CountryList.cpp.

Το αρχείο Commands.cpp περιέχει τις 5 διαφορετικές εντολές που μπορεί να δώσει ο χρήστης, δηλαδή τον έλεγχο για την κάθε μία και την υλοποίηση τους.
Το αρχείο requestsList.cpp κρατάει για κάθε request που έχει γίνει την ημερομηνία του, την χώρα για την οποία έγινε "countryFrom" καθώς και έναν request counter. 

Το αρχείο SocketFunctions.cpp, το οποίο περιέχει τις συναρτήσεις readInt, readString και readBloom για το διάβασμα ακεραίου, συμβολοσειράς και bloomFilter αντίστοιχα
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
Ακόμα το travelMonitor διαβάζει (χρησιμοποιώντας την select) από κάθε monitorServer τα bloomFilters που θα του επιστρέφει και τα ενώνει δημιουργώντας ένα
bloomFilter για κάθε ιό.

Τέλος καλεί την συνάρτηση Commands και εκτελεί τις εντολές που θα δωθούν από τον χρήστη μέχρι να δωθεί η εντολή "/exit" όπου απελευθερώνει ότι έχει δεσμευθεί δυναμικά 
και δημιουργεί το αρχείο log_file για το parent proccess καθώς και ένα για κάθε monitorServer.

monitorServer

Το αρχείο Monitor.cpp κάνει έλεγχο για τα ορίσματα, ανοίγει τα named pipes για read και write και διαβάζει απο το travelMonitor το bufferSize, το
sizeOfBloom καθώς και το input_dir. Στην συνέχεια διαβάζει το όνομα κάθε χώρας (subdirectory) που του στέλνει το travelMonitor μέχρι να του στείλει
τον αριθμό -1, δηλαδή ότι σταμάτησε να στέλνει χώρες. Στην συνέχεια για κάθε subdirectory διαβάζει όλα του τα αρχεία (με την συνάρτηση ReadFile της
πρώτης εργασίας) και δημιουργεί τις δομές VirusList,CountryList και CitizenList καθώς και τα BloomFilters και SkipLists για κάθε ιό. Τέλος το Monitor
στέλνει πίσω στο travelMonitor ένα bloomFilter για κάθε ιό, τα οποία ενώνονται στο travelMonitor μαζί με τα bloomFilters που έχουν σταλεί απο κάποιο
άλλο Monitor και έτσι έχουμε μία λίστα απο bloomFilters απο τα οποία κάθε ένα αναφέρεται σε συγκεκριμένο ιό.

Εντολές στο αρχείο Commands.cpp

Command 1 - /travelRequest citizenID date countryFrom countryTo virusName 
Αρχικά γίνεται έλεγχος για τον αν ο ιός και η χώρα που έχει δοθεί υπάρχουν η όχι. Στην συνέχεια θα ελέγχει το bloomFilter 
του συγκεκριμένου ιού για να δει αν υπάρχει ο πολίτης και αν όχι θα εκτυπώνει σχετικό μήνυμα, θα προσθέτει το request στην λίστα με τα requests και θα ενημερώνει 
τον rejectedcounter. Αν υπάρχει στο bloomFilter στέλνει εντολή στο Monitor που του έχει ανατεθεί το συγκεκριμένο country "countryFrom" για να εντοπίσει στην SkipList
του συγκεκριμένου ιού αν υπάρχει ο πολίτης. Αν δεν υπάρχει επιστρέφει ως απάντηση "NO" και εκτυπώνει σχετικό μήνυμα. Αν υπάρχει επιστρέφει "YES" καθώς και την ημερομηνία
εμβολιασμού του. Στη συνέχεια ελέγχει αν έχει εμβολιαστεί λιγότερο από 6 μήνες πριν την ημερομηνία που θέλει να ταξιδέψει και αν ναι εκτυπώνει "HAPPY TRAVELS" αλλιώς 
εκτυπώνει "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE".


Command 2 - /travelStats virusName date1 date2 [country]
Αρχικά γίνεται έλεγχος για τον αν ο ιός και η χώρα που έχει δοθεί υπάρχουν η όχι και στην συνέχεια εκτυπώνει τον αριθμό των συνολικών requests καθώς και των requests που
έχουν εγκριθεί και απορριφθεί χρησιμοποιώντας τις συνάρτησεις BloomListGetTotalRequests, BloomListGetAcceptRequests και BloomListGetRejectRequests αντίστοιχα λαμβάνοντας υπόψην
τις ημερομηνίες που έχουν δοθεί. Ακόμα στην περίπτωση που δοθεί country σαν όρισμα καλούνται οι ίδιες συνατήσεις αλλά αυτή την φορά με την χώρα σαν όρισμα ώστε να επιστρέψουν 
τον αριθμό των requests μόνο για την συγκεκριμένη χώρα. 


Command 3 - /addVaccinationRecords country
Αρχικά γίνεται έλεγχος για τον αν η χώρα που έχει δοθεί υπάρχει η όχι και στη συνέχεια στέλνεται στο Monitor που του έχει ανατεθεί η χώρα country το σήμα SIGUSR1.
Το Monitor θα κάνει handle το σήμα διαβάζοντας όποιο νέο αρχείο υπάρχει στο subdirectory με το όνομα της χώρας και ενημερώνοντας τις δομές που κρατάει (VirusList,
CitizenList, CountryList) και επιστρέφοντας ενημερωμένα bloom filters τα οποία ενώνονται με τα υπόλοιπα bloom filters, ώστε τελικά να έχουμε ένα bloom για κάθε ιό.

Command 4 - /searchVaccinationStatus citizenID
Αρχικά γίνεται έλεγχος για το αν το citizenID που έχει δοθεί υπάρχει η όχι. Αν υπάρχει στέλνεται σε κάθε Monitor το citizenID και εκείνο επιστρέφει ότι πληροφορία έχει
για τον συγκεκριμένο πολίτη. Η πληροφορία εκτυπώνεται στο αρχείο Commands.cpp.

Command 5 - /exit 
Η εντολή αυτή επιστρέφει 1 στο travelMonitor ώστε να σταματήσει το loop με τις εντολές και διαγράφει τα countryLists που υπάρχουν στην δομή monitorInfo και δείχνουν ποιες χώρες έχουν 
ανατεθεί σε κάθε Monitor καθώς και την λίστα με τα bloom filters. Τέλος αφού βγεί απο το loop με τις εντολές το αρχείο travelMonitor.cpp στέλνει ένα σήμα SIGINT στον ευατό του.

Signals

SIGINT ή SIGQUIT στο travelMonitor
Στην περίπτωση που δοθεί η εντολή "/exit" ή ctrl + c από τον χρήστη το travelMonitor κάνει handle το SIGINT στέλνοντας SIGKILL σε όλα τα Monitors,
περιμένοντας να σταματήσουν και δημιουργώντας το αρχείο log_file. Επίσης υπάρχει το flag suicide όπου όταν τα Monitors πεθάνουν και επιστρέψουν SIGCHLD 
φροντίζει να το αγνοήσει ώστε να μην ξαναδημιουργηθούν με fork όπως στην περίπτωση που τερματίζονται στέλνοντας SIGINT απο το travelMonitor.

SIGINT ή SIGQUIT στα Monitors
Στην περίπτωση όπου ένα Monitor δεχθεί SIGINT απο το travelMonitor, θα δημιουργήσει το αρχείο log_file και στην συνέχεια θα τερματήσει αφού πρώτα
ελευθερώσει τις δομές που χρησιμοποιεί. Αφού τερματίσει θα επιστρέψει σήμα SIGCHLD στο travelMonitor το οποίο με την σειρά του θα αντικαταστήσει το 
Monitor που τερμάτισε ξαφνικά με ένα νέο Monitor χρησιμοποιώντας fork. Το travelMonitor (αφού έχουν επιστρέψει όλα τα Monitors τα bloom filters) στέλνει
σε κάθε Monitor ένα σήμα SIGINT ώστε να τερματίσει "ξαφνικά" και να υλοποιηθεί η διαδικασία που ζητείται στην εκφώνηση).

SIGUSR1 στα Monitors
Στην περίπτωση όπου δοθεί η εντολή "/addVaccinationRecords" το travelMonitor και συγκεκριμένα το αρχείο "Commands.cpp" θα στείλει ένα σήμα SIGUSR1
στο Monitor που του έχει ανατεθεί η συγκεκριμένη χώρα. Το Monitor θα κάνει handle το σήμα διαβάζοντας όποιο νέο αρχείο υπάρχει στο subdirectory
με το όνομα της χώρας και ενημερώνοντας τις δομές που κρατάει (VirusList,CitizenList,CountryList) και επιστρέφοντας ενημερωμένα το bloom filters.