Rotaru Alexandru Andrei 335CC

Se cere optimizarea unui cod care genera imagini png ce conting fractali

Pentru optimizari am facut urmatoarele:
    - in definitia structurii Image am inversat si ordonat
    variabilele membre pentru o aliniere mai buna.
    - in fisierul Image.c am sters variabila artifacts si
    in metoda readHeader am marit viteza de citire a unui
    header citind o linie o data.
    -in metoda image_create am rescris alocarea memoriei
    pentru obiectul image, am scos zerorizarea headerului
    si am facut alocarea neintializata a membrului map.
    De asemenea am deduplicat codul, avnd grija sa nu
    fac aceasi operatie de 2 ori.
    - in image close am sters partea redundata (prima)
    si am facut scrierea intr-o singura operatie.
    - Cele mai semnificative modificari le-am facut in
    fisierul Fractal.c
    - am sters vectorul intul colors si am sters
    functiile Process_Fractal, process, locate_fractal_index,
    Prepare_Fractal si acquire_line.
    - din Fractal_Destroy am ster logica inutila
    si am pastrat doar liniile care faceau ce trebuie
    si anume dealocarea memorie.
    - functia Wrap am rescris-o dupa ce am realizat
    functionalitatea ei, de asemenea am tinut cont si
    de tipul de date. De asemea am facut-o inline pentru
    a fi inserata direct in codul obiect.
    - functia iterate am rescris-o intr-un mod in care
    nu nu am if in structura repetitiva ci o structura
    repetitiva cu o conditie mai complexa si nu mai folosesc
	inmultirea standard de numere complexe ci am separat
	in partea imaginara si reala is le folosesc pe acelea
	la calcule
    -in functia Fractal_Render:
    - am micsorat numarul de variabile folosite si le-am
    decorat pe cele mai intens accesate cu register. In
    afara de structurilor repetitive am precalculat
    toti invariantii.
    - am explicitat cele 2 foruri cu ay si ax
	- am scos apelul redundant de iterate, asociat ay=1 ax=0
    - am scazut precizia datelor de la long double la double
    -am ordonat accesarile de variabile cat mai grupat
    astfel incat sa am o localitate spatiala si temporala
    buna.
    - accesarea vectorului o fac prin pointer pentru a reduce
    calculul de indecsi.

Am implementat si bonusul:
    - am pornit un numar fix de threaduri carora le asignez un numar
    fix de linii intr-un interval din liniile imaginii bine stabilit.
	- folosesc 16 threaduri

Nu compilez cu flaguri de optimizare.
