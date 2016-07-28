Rotaru Alexandru Andrei 335CC

Implementare:
Pentru rezolvarea temei folosesc:
- 3 stari: INITIAL (starea initiala), TCOLUMN 
(pentru citirea capului de tabel) si RVALUES(pentru read values adica citirea 
valorilor pentru capul de tabel); 
- 2 vectori de char* pe care i-am lasat la dimensiunea 35 pentru identificatori si valorile asociate
- 4 variabile de contorizare a rezultatelor

La inceput sunt in starea INITIAL si incerc sa fac match pe "Boardin Pass", 
dupa ce am facut match, tot in starea INITIAL fac match pe fiecare identificator
care poate fi format din mai multe cuvinte si incrementez o variabila care numara
cati potential identificatori am gasit. Daca dupa acest identificator urmeaza
un newline inseamna ca nu citeam dintr-un cap de tabel, si merg mai departe cu 
potrivirile tot prin starea INITIAL. Daca un cuvant ca variabila este 1, gasesc
cel putin un tab, atunci inseamna ca citesc un cap de tabel, si intru in starea 
TCOLUMN. In aceasta stare atata timp cat gasesc identificatori incrementez un contor
iar cand am ajuns la newline intru in starea RVALUES in care fac potriviri pe grupuri
de cuvinte separate prin spatiu, punand fiecare astfel de grup intr-un vector de cuvinte
si contorizand.
Cand in starea TCOLUMN am ajuns la newline inseamna ca ar trebui ca numarul curent
de identificatori sa fie egal cu numarul de valori gasite. Daca este adevarat,
adaug in dimensiunea celor doi vectori noile valori, daca e fals, nu modific
dimensiunea vectorilor, urmand ca valorile adaugate sa fie suprascrise.


La final afisez cei doi vectori concomitent.
Pot avea mai multe "Boarding Pass"-uri in fisier.

Testele se numesc test1.in si test2.in
Timp de lucru 8 ore.

