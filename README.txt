Nicolae Alexandru-Dimitrie
Grupa: 321CD
README -Tema 3 Protocoale de Comunicatii

Implementarea acestei teme are ca punct de plecare laboratorul 10.
Tema contine urmatoarele fisiere:
  -'buffer.c' si 'buffer.h' au fost preluate din laborator fara a schimba ceva;

  -'helpers.c' si 'helpers.h' au fost, de asemenea, preluate din laborator fara a schimba ceva;

  -'parson.c' si 'parson.h' au fost luate din repository-ul de github pus ca sugestie in cerinta temei,si le-am folosit
pentru a extrage informatiile despre un utilizator si despre o carte(le-am ales deoarece am implementat tema in limbajul C);

  -'requests.c' si 'requests.h' au fost si ele preluate din laborator,doar ca am efectuat cateva schimbari:
    -compute_get_request: este la fel ca in laborator doar ca i-am adaugat un parametru numit tok ce reprezinta tokenul cererii si am adaugat campul
    corespunzator acestuia in cerere;
    -compute_delete_request: este la fel ca functia compute_get_request,doar ca numele metodei este DELETE;
    -compute_post_request: este la fel ca in laborator doar ca i-am adaugat un parametru numit tok ce reprezinta tokenul cererii si am adaugat campul
    corespunzator acestuia in cerere;

 -fisierul de Makefile este asemanator cu cel din laboratorul 10 ,doar ca am adaugat la regula de compilare si fisierul parson.c

 -'client.c':Aici am realizat implementarea efectiva a temei:
    -Initial,am inclus bibliotecile prezente si in fisierul 'client.c' din labortorul 10 si fisierele auxiliare .h.
    -Apoi,am definit variabilele IP_SERVER si PORT_SERVER cu ip-ul serverului,respectiv port-ul serverului din cerinta temei.
    -Dupa aceea,am creeat 2 structuri,una pentru un utilizator si una pentru o carte,pentru a nu mai declara alte variabile in main si pentru a
manipula datele mai usor si in cazul parsarii din fiserele JSON.
    -Dupa,am implementat o functie "commandToint" ce atribuie fiecarei comenzi date de la tastatura ,mai putin celei de exit, o valoare intreaga de la 1 la 8,
facilitand organizarea mai buna a codului in functia main,folosind un switch case in loc de foarte multe if else-uri.
    -Apoi,incep implementarea functiei main,declarand :
                                            u ->o variabila de tip User;
                                            b ->o variabila de tip Book;
                                            command ->un sir de caractere ce reprezinta comanda primita de la stdin
                                            sess_cookie ->un sir de caractere ce reprezinta cookie-ul de sesiune
                                            tok ->un sir de caractere ce retine tokenul pentru fiecare cerere facuta
                                            users ->un sir de siruri de caractere ce ajuta la parsarea JSON-ului pentru un user si efectuarea 
                                            cererilor de tip post
                                            cookies ->un sir de siruri de caractere ce ajuta la efectuarea cererilor de tip get sau delete
                                            books ->un sir de siruri de caractere ce ajuta la implementarea comenzii add_book
                                            logged ->o variabila de stare care este setata atunci cand un utilizator este logat
                                            in_lib ->o variabila de stare care este setata atunci cand un utilizator este in biblioteca
                                            sockfd ->socket-ul care deschide conexiunea la serverului
    -Dupa,incep o bucla infinita in care:
        -citesc comanda de la stdin cu fgets,apoi sterg ultimul caracter deoarece este un "\n"
        -efectuez ce trebuie executat la primirea fiecarei comenzi:
           -la primirea comenzii "exit",se iese din bucla
           -altfel,se deschide o conexiune cu serverul si se retine in variabila n valoarea intreaga a comenzii primite,aflata prin apelul functiei "commandToint",
        dupa care se face un switch dupa valoarea fiecarui n,astfel:
           -daca n = 0, este un caz default si nu se intampla nimic;
           -daca n = 1, este echivalent cu primirea comenzii "register":-se introduc username-ul si parola pe care dorim sa le avem
                                                                        -se parseaza datele folosindu-se biblioteca JSON
                                                                        -se face o cerere de tip POST pentru comanda "register"
                                                                        -se verifica daca mai exista vreun cont cu acelasi username
           -daca n = 2,este echivalent cu primirea comenzii "login":-se verifica daca utilizatorul este deja logat
                                                                    -in caz negativ,se introduc username-ul si parola user-ului pe care se doreste logarea,
                                                                    se parseaza datele folosindu-se biblioteca JSON,se efectueaza o cerere de tip POST pentru
                                                                    comanda "login",se verifica daca utilizatorul se poate loga si se retine cookie-ul de sesiune
                                                                    in cazul in care logarea este facuta
                                                                    -in caz afirmativ,se afiseaza un mesaj corespunzator
           -daca n = 3,este echivalent cu primirea comenzii "enter_library":-se verifica daca utilizatorul este deja logat
                                                                            -in caz afirmativ,se efectueaza o cerere de tip GET pentru comanda "enter_library",
                                                                            se verifica daca utilizatorul a accesat biblioteca,se extrage tokenul de acces din cerere
                                                                            -in caz negativ,se afiseaza un mesaj corespunzator
           -daca n = 4,este echivalent cu primirea comenzii "get_books":-se verifica daca utilizatorul a accesat biblioteca
                                                                        -in caz afirmativ,se efectueaza o cerere de tip GET pentru comanda "get_books",se
                                                                        extrag din cerere cartile prezente in biblioteca si se afiseaza
                                                                        -in caz negativ,se afiseaza un mesaj corespunzator
           -daca n = 5,este echivalent cu primirea comenzii "get_book":-se verifica daca utilizatorul a accesat biblioteca
                                                                      -in caz afirmativ,se introduce id-ul cartii care se doreste a fi extrasa din biblioteca,
                                                                      se verifica corectitudinea id-ului (daca este mai mare sau egal cu 0),se configureaza calea de
                                                                      acces pentru a lua cartea cu id-ul primit,se efectueaza o cerere de tip GET pe calea de acces 
                                                                      configurata precedent,se verifica daca exista in cerere o carte cu id-ul cerut
                                                                      -in caz negativ,se afiseaza un mesaj corespunzator
           -daca n = 6,este echivalent cu primirea comenzii "add_book":-se verifica daca utilizatorul a accesat biblioteca
                                                                       -in caz afirmativ,se introduc de la tastatura caracteristicile cartii si se verifica sa fie diferite
                                                                       de spatiu pentru stringuri si sa fie mai mare strict decat 0 numarul de pagini,se transforma numarul de
                                                                       pagini dintr-o valoare intreaga intr-un sir de caractere,se parseaza datele folosind biblioteca JSON,se
                                                                       efectueaza o cerere de tip POST pentru comanda "add_book"
                                                                       -in caz negativ,se afiseaza un mesaj corespunzator 
           -daca n = 7,este echivalent cu primirea comenzii "delete_book":-se verifica daca utilizatorul a accesat biblioteca
                                                                          -in caz afirmativ,se introduce id-ul cartii care se doreste a fi stearsa,verificandu-se ca id-ul sa 
                                                                          fie mai mare sau egal cu 0,se configureaza calea de acces spre cartea cunid-ul dat,se efectueaza o
                                                                          cerere de tip DELETE pentru comanda "delete_book",se verifica daca este vreo eroare in cerere
                                                                          -in caz negativ,se va afisa un mesaj corespunzator
           -daca n = 8,este echivalent cu primirea comenzii "logout":-se verifica daca utilizatorul este deja logat
                                                                     -in caz afirmativ,se efectueaza o cerere de tip GET pentru comanda "logout",se reseteaza cele doua variabile 
                                                                     de stare
                                                                     -in caz negativ,se afiseaza mesajul corespunzator
    -se inchide conexiunea cu serverul

NOTA: Aceasta tema a fost trimisa si anul trecut tot de catre mine,doar am rezolvat cateva bug-uri.
