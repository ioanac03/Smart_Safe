# Implementarea unui Seif Inteligent cu Raspberry Pi Pico W

Bunurile personale și securitatea lor reprezintă, pentru orice persoană, o grijă. Din acest motiv, dezvoltarea unor sisteme de securitate stabile cu o interfață intuitivă oricărui utilizator este necesară.

În cadrul acestui proiect este dezvoltat un sistem hardware complex îmbinat cu un software stabil, prin care se va realiza o implementare demonstrativă a unui **Seif Inteligent** cu autentificare PIN, mecanism servo și sistem de alarmă.

---

## Cuprins

1. [Context](#context)
2. [Cerințe](#cerinte)

    -[Cerințe Funcționale](#cerinte-functionale)

    -[Cerințe Non-Funcționale](#cerinte-non-functionale)

3. [Arhitectură](#arhitectură)

    -[Definire Arhitectură](#definire-arhitectura)

<a id="context"></a>

# 1. Context

Implementarea unui **sistem de securitate fizică** inovator (seif inteligent) pentru a demosntra integrarea componentelor hardware și software într-un dispozitiv embedded funcțional. Sistemul combină autentificarea prin cod PIN, feedback vizual pe ecran LCD, acționare mecanică prin servomotor și detecție de efracție prin senzor de lumină.

Proiectul ilustrează concepte fundamentale de sisteme embedded:

  -seiful se deblochează prin introducerea unui cod PIN, folosind un modul cu butoane.

  -starea sistemului afișată pe un ecran LCD.

  -mecanismul de deschidere realizat prin un servo motor.

  -un buzzer pentru semnalizarea tentativelor de acces neautorizat la introducerea repetată a unor coduri greșite.

  -un senzor de lumină în interior care, dacă starea seifului este de "încuiat" și detectează lumină, declanșează o alarmă.
  
<a id="cerinte"></a>

# 2. Cerințe

<a id="cerinte-functionale"></a>

## 2.1. Cerințe funcționale

  * a. autentificare și control acces: utilizatorul introduce un cod PIN format din câteva cifre folosind butoanele hardware. Sistemul verifică acest cod cu parola, iar dacă datele corespund, servo motorul descuie seiful.

  * b. interfața cu utilizatorul: pe ecranul LCD sunt afisate mesaje clare de stare, cum ar fi solicitarea introducerii PIN-ului, confirmarea deblocarii, notificarea unui PIN greșit sau mesaje de alarmă.

  * c. securitate anti-brute-force: pentru a preveni ghicirea parolei, sistemul numără de cate ori este introdus un cod greșit. Dupa 3 incercari consecutive eșuate, buzzer-ul emite o alarmă pentru a semnala o activitate suspectă.

<a id="cerinte-non-functionale"></a>

## 2.2. Cerințe non-funcționale:

  * a. performață și timp de raspuns: interfața trebuie să reacționeze rapid, fără întârzieri vizibile între apăsarea butoanelor și afișare pe ecran. De asemenea, reacția sistemului la efracție trebuie să fie promptă, declanșând alarma imediat dupa expunerea senzorului la lumină.

  * b. salvarea datelor: codul PIN este definit la compilare și stocat în memoria Flash read-only a microcontrolerului. PIN-ul nu poate fi modificat de utilizator fără a recompila și a reîncărca firmware-ul pe dispozitiv. 
  
  * c. alimentare și consum: sistemul este proiectat să funționeze la o tensiune standard, putand fi alimentat printr-un cablu USB.

---

<a id="arhitectura"></a>

# 3. Arhitectură

<a id="definire-arhitectura"></a>

## 3.1. Definire arhitectură

![Schita arhitectura sistem](images/schita_arhitectura.jpeg)
1. Tastatura (Keypad)
Este interfața principală de input pentru utilizator.
  * Legături (4 GPIO OUT - Linii & 4 GPIO IN - Coloane): Tastatura funcționează prin scanarea unei matrice. Microcontrolerul trimite semnale pe cele 4 linii și citește starea celor 4 coloane pentru a determina exact ce tastă a fost apăsată.
  * Rol: Permite introducerea codului PIN de 4 cifre.
2. Modulul LCD (Afișaj)
  * Legătura SPI: Folosește protocolul de comunicare Serial Peripheral Interface (SPI),.
  * Legătura „Afișează PIN”: Primește date direct de la logica centrală pentru a oferi feedback vizual utilizatorului.
3. Servomotor (Mecanismul de blocare)
  * Legătura VBUS (5V): Servomotoarele necesită de obicei o tensiune de 5V pentru a avea destul cuplu (forță) să miște zăvorul seifului.
  * Legătura GND: Împământarea comună pentru închiderea circuitului.
  * Legătura GPIO (PWM): Microcontrolerul trimite un semnal care îi spune brațului servomotorului să se rotească la un anumit unghi.
4. Fotorezistența (Senzorul LDR)
Acesta este „senzorul de efracție” din interiorul seifului.
  * Legătura VCC (3.3V) și GND: Alimentarea senzorului.
  * Legătura GPIO (ADC): Aceasta este o conexiune crucială. Fotorezistența își schimbă valoarea în funcție de lumină. Microcontrolerul folosește un Convertor Analog-Digital (ADC) pentru a citi o tensiune variabilă.
  * Logica: Dacă senzorul „simte” lumină (tensiunea scade/crește peste un prag) fără ca PIN-ul să fi fost validat, înseamnă că seiful a fost deschis forțat.
5. Buzzer (Alarma)
  * Legătura GPIO: O conexiune digitală simplă.
  * Legătura GND: Împământarea.
  * Rol: Când logica de control detectează o tentativă de efracție (lumină detectată de fotorezistență + PIN incorect/neintrodus), trimite un semnal "HIGH" către buzzer pentru a declanșa sunetul de alertă.
