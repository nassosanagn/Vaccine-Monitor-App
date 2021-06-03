Προγραμματισμός Συστήματος
2η Άσκηση

Αναγνωστόπουλος Αθανάσιος
ΑΜ: 1115201800006

Μεταγλώττιση και Εκτέλεση

Αρχικά υπάρχει ένα αρχείο Makefile για την διευκόλυνση της μεταγλώττισης και της εκτέλεσης του προγράμματος. Η εντολή make δημιουργεί το αρχείο travelMonitor.
Το πρόγραμμα δέχεται τέσσερα ορίσματα, τον αριθμό των Monitor διεργασιών που θα δημιουργήσει η εφαρμογή (numMonitors), το μέγεθος του buffer για διάβασμα απο 
τα pipes (bufferSize), το μέγεθος του bloom Filter (sizeOfBloom) καθώς και το directory το οποίο περιέχει τα αρχεία που θα επεξεργάζονται τα Monitors (input_dir) 
με οποιαδήποτε σειρά.

Ο αριθμός τον Monitors δίνεται μετά απο το πρόθεμα -m, το bufferSize μετα απο το πρόθεμα -b, το sizeOfBloom μετά από το πρόθεμα -s και το όνομα του 
directory μετά απο το -i.

π.χ. ./travelMonitor -m 4 -b 100 -s 100000 -i ./countries
Τέλος, υπάρχει και η εντολή make clean για την γρήγορη διαγραφή όλων των .o αρχείων καθώς και των εκτελέσιμων αρχείων travelMonitor και Monitor.
 
create_infiles.sh

Το συγκεκριμένο bash script δημιουργεί ένα directory το οποίο χρησιμοποιείται ως είσοδος στο πρόγραμμα (input_dir). Το directory που θα παραχθεί από το script 
θα περιέχει subdirectories με τα αρχεία txt τα οποία θα επεξεργάζονται τα Monitors.

Δέχεται 3 ορίσματα με συγκεκριμένη σειρά τα οποία είναι:
•	ένα αρχείο (inputFile) το οποίο θα έχει ίδιο format με το αρχείο που παράγει το script της πρώτης εργασίας,
•	το όνομα του directory (input_dir) που θα τοποθετηθούν τα subdirectories καθώς και τα input files,
•	και τέλος τον αριθμό των αρχείων που θα φτιαχτούν σε κάθε subdirectory (numFilesPerDirectory). 

π.χ. ./create_infiles.sh inputFile.txt countries 5

Αρχικά το script ελέγχει αν δόθηκαν σωστά τα ορίσματα και στη συνέχεια δημιουργεί τους πίνακες countries και index. Ο πίνακας countries θα περιέχει τα ονόματα όλων 
των χωρών ,δηλαδή όλα τα ονόματα όλων των subdirectories, και ο πίνακας index θα περέχει τον αριθμό του αρχείου txt μέσα στο subdirectory στο οποίο πρέπει να εκτυπωθεί 
η επόμενη εγγραφή. Για παράδειγμα αν index[3] = 5, γνωρίζουμε πως στην χώρα countries[3] η επόμενη εγγραφή πρέπει να εκτυπωθεί στο αρχείο "countries[3]"-5.txt.

Αν η χώρα δεν υπάρχει στον πίνακα με τις χώρες τότε την προσθέτει και δημιουργεί ένα subdirectory με το όνομα της νέας χώρας καθώς και numFilesPerDirectory αρχεία μέσα
στο συγκεκριμένο subdirectory και τέλος εκτυπώνει την πρώτη γραμμή του αρχείου με όνομα country-1 όπου "country" το όνομα του subdirectory. Επίσης προσθέτει 2 στον πίνακα 
με τα indexes ώστε την επόμενη φορά που θα βρει εγγραφή για την συγκεκριμένη χώρα να την εκτυπώσει στο αρχείο με όνομα country-2.

Αν η χώρα υπάρχει ήδη στον πίνακα με τις χώρες τότε θα προσθέσει την εγγραφή στο subdirectory με το όνομα της χώρας και πιο συγκεκριμένα στο αρχείο txt που μας δείχνει 
ο πίνακας index. Για παράδειγμα αν η χώρα βρίσκεται ήδη στον πίνακα στην θέση x θα εκτυπώσουμε την εγγραφή στο αρχείο με όνομα "countries[x]"-"index[x]". Στην συνέχεια 
θα αυξήσουμε το index[x] κατα ένα ώστε την επόμενη φορά η εγγραφή να γίνει στο επόμενο αρχείο, αλλά στην περίπτωση όπου index[x] == numFilesPerDirectory θα ξανα αρχίσουμε
να εκτυπώνουμε τις εγγραφές από το πρώτο αρχείο.

Αρχεία και δομές

Αρχικά υπάρχουν τα αρχεία BloomFilter.cpp, SkipList.cpp, VirusList.cpp, CitizenList.cpp, CountryList.cpp, Date.cpp, hash.cpp, ReadFile.cpp και
VaccinationInfoList.cpp και τα αντίστοιχα .h αρχεία απο την πρώτη εργασία μαζί με κάποιες επιπλέον συναρτήσεις όπως π.χ. την συνάρτηση lessThan6Months
στο αρχείο Date.cpp.

Ακόμα υπάρχουν τα αρχεία bloomList.cpp, Commands.cpp, Monitor.cpp, PipeFunctions.cpp, requestsList.cpp και travelMonitor.cpp καθώς και τα αντίστοιχα
.h αρχεία.

Το αρχείο Commands.cpp περιέχει τις 5 διαφορετικές εντολές που μπορεί να δώσει ο χρήστης, δηλαδή τον έλεγχο για την κάθε μία και την υλοποίηση τους.

Το αρχείο PipeFunctions.cpp περιέχει τις συναρτήσεις readInt, readString και readBloom για το διάβασμα ακεραίου, συμβολοσειράς και bloomFilter αντίστοιχα
απο ένα named pipe λαμβάνοντας υπόψην το bufferSize

Το αρχείο requestsList.cpp κρατάει για κάθε request που έχει γίνει την ημερομηνία του, την χώρα για την οποία έγινε "countryFrom" καθώς και έναν counter. 

travelMonitor

Το αρχείο travelMonitor.cpp αρχικά κάνει έλεγχο για τα ορίσματα που του έχουν δοθεί. Στην συνέχεια δημιουργεί 2 * numMonitors named pipes ένα για read και ένα για write 
για κάθε ένα Monitor, τα οποία κάνει open και κρατάει στην δομή monitorInfo το readFd και το writeFd για κάθε Monitor. Ακόμα κάνει fork numMonitors child proccesses 
(τα Monitors) και καλεί την execvp δίνοντας σαν όρισμα για σε Monitor δύο paths σε named pipes στα οποία μπορεί να διαβάσει και να γράψει και κρατάει στην δομή monitorInfo
τα pid για κάθε child proccess (δηλαδή Monitor). Έπειτα στέλνει σε κάθε Monitor το bufferSize, το sizeOfBloom, το input_dir καθώς και τις χώρες που θα αναλάβει (με κατανομή
round-robin). Τέλος το travelMonitor θα διαβάζει (χρησιμοποιώντας την select) από κάθε Monitor τα bloomFilters που θα του επιστρέφει και θα τα ενώνει δημιουργώντας ένα
bloomFilter για κάθε ιό.

Monitor

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