📂 **Generator-de-snapshot**
Acest proiect C creează snapshot-uri (capturi de stare) ale directoarelor specificate. Fiecare snapshot conține informații despre fișierele dintr-un director: nume, tip, inode, și timestamp-uri de acces, modificare și creare.

🛠️ **Funcționalități**
Traversează recursiv un director și listează:
Numele fișierelor
Tipul fișierelor (regular file, directory, symlink, etc.)
Numărul inode
Timpul ultimei modificări, accesări și schimbări de status
Creează fișiere de snapshot (.txt) cu toate aceste informații.
Suportă generarea snapshot-urilor în mod normal sau paralel (cu procese copil).
Gestionare robustă a erorilor (pentru fork, open, write, stat, etc.).

📋 **Cum funcționează**
Programul are trei moduri de lucru:
1. Mod Simplu (fără procese multiple):
    ./snapshot_creator dir1 dir2 dir3 ...
Creează câte un snapshot pentru fiecare director în parte.

2. Mod cu director de snapshot (-o):
    ./snapshot_creator -o <director_snapshot> dir1 dir2 ...
Salvează toate snapshot-urile într-un director specificat (<director_snapshot>).

3. Mod paralel (-s):
    ./snapshot_creator <ceva> <director_snapshot> -s dir1 dir2 ...
Creează procese copil pentru fiecare director, generând snapshot-uri în paralel.

**🧰 Compilare**
Pentru a compila proiectul:
    gcc -o snapshot_creator snapshot_creator.c
    
**🚀 Exemplu de utilizare**
1. Mod Simplu
    ./snapshot_creator /home/user/Documents /home/user/Downloads
Va crea fișierele:
    /home/user/Documents/snapshot1_.txt
    /home/user/Downloads/snapshot2_.txt

2. Mod cu director specificat (-o)
    ./snapshot_creator -o /home/user/Snapshots /home/user/Documents /home/user/Pictures
Va crea un singur fișier:
    /home/user/Snapshots/Director_snapshot.txt

3. Mod Paralel (-s)
    ./snapshot_creator ceva /home/user/Snapshots -s /home/user/Documents /home/user/Music
Fiecare director va fi procesat într-un proces copil.

**⚙️ Cerințe**
Sistem de operare Unix/Linux (folosește POSIX API: fork, open, write, stat, etc.).
Compilator C (ex: gcc).

**📎 Note**
Pentru modurile -o și -s, numărul minim de directoare este:
1 director pentru -o
2 directoare pentru -s
Pentru modul simplu: între 1 și 10 directoare.
Fișierele generate conțin datele brute direct extrase din sistemul de fișiere.

**❗ Gestionarea erorilor**
Programul gestionează:
Erori la deschiderea directoarelor sau fișierelor.
Erori la creare de procese.
Erori la operațiuni de scriere sau stat.
Terminarea anormală a proceselor copil.
