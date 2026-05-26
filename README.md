# Implementarea unui Seif Inteligent cu Raspberry Pi Pico W

Bunurile personale și securitatea lor reprezintă, pentru orice persoană, o grijă. Din acest motiv, dezvoltarea unor sisteme de securitate stabile cu o interfață intuitivă oricărui utilizator este necesară.

În cadrul acestui proiect este dezvoltat un sistem hardware complex îmbinat cu un software stabil, prin care se va realiza o implementare demonstrativă a unui **Seif Inteligent** cu autentificare PIN, mecanism servo și sistem de alarmă.

---

## Cuprins

1. [Context](#context)
2. [Cerințe](#cerinte)<br>
    -[Cerințe funcționale](#cerinte-functionale)<br>
    -[Cerințe non-funcționale](#cerinte-non-functionale)<br>
    -[Scenariu de testare](#scenariu-de-testare)
3. [Componente](#componente)
    -[Schema bloc](#schema-bloc)<br>
    -[Descrierea componentelor](#descrierea-componentelor)<br>
4. [Arhitectură](#arhitectura)
    -[Definire arhitectură](#definire-arhitectura)<br>
    -[Schema electrică](#schema-electrica)<br>
    -[Arhitectura Software](#arhitectura-software)
5. [Scenarii și diagrame](#scenarii-diagrame)
    -[Diagrama use-case](#use-case)<br>
    -[Scenariile de funcționare](#scenarii)
    -[Diagrama de secvențe](#diagrama-secvente)
6. [Etapele de realizare](#etape)
7. [Bibliografie](#bibliografie)

<a id="context"></a>

# 1. Context

Implementarea unui **sistem de securitate fizică** inovator (seif inteligent) pentru a demosntra integrarea componentelor hardware și software într-un dispozitiv embedded funcțional. Sistemul combină autentificarea prin cod PIN, feedback vizual pe ecran LCD, acționare mecanică prin servomotor și detecție de efracție prin senzor de lumină.

Proiectul ilustrează concepte fundamentale de sisteme embedded:<br>
  - seiful se deblochează prin introducerea unui cod PIN, folosind un modul cu butoane.<br>
  - starea sistemului afișată pe un ecran LCD.<br>
  - mecanismul de deschidere realizat prin un servo motor.<br>
  - un buzzer pentru semnalizarea tentativelor de acces neautorizat la introducerea repetată a unor coduri greșite.<br>
  - un senzor de lumină în interior care, dacă starea seifului este de "încuiat" și detectează lumină, declanșează o alarmă.
  
---

<a id="cerinte"></a>

# 2. Cerințe

<a id="cerinte-functionale"></a>

## 2.1. Cerințe funcționale:

  * autentificare și control acces: utilizatorul introduce un cod PIN format din câteva cifre folosind butoanele hardware. Sistemul verifică acest cod cu parola, iar dacă datele corespund, servo motorul descuie seiful.

  * interfața cu utilizatorul: pe ecranul LCD sunt afisate mesaje clare de stare, cum ar fi solicitarea introducerii PIN-ului, confirmarea deblocarii, notificarea unui PIN greșit sau mesaje de alarmă.

  * securitate anti-brute-force: pentru a preveni ghicirea parolei, sistemul numără de cate ori este introdus un cod greșit. Dupa 3 incercari consecutive eșuate, buzzer-ul emite o alarmă pentru a semnala o activitate suspectă.

<a id="cerinte-non-functionale"></a>

## 2.2. Cerințe non-funcționale:

  * performață și timp de raspuns: interfața trebuie să reacționeze rapid, fără întârzieri vizibile între apăsarea butoanelor și afișare pe ecran. De asemenea, reacția sistemului la efracție trebuie să fie promptă, declanșând alarma imediat dupa expunerea senzorului la lumină.

  * salvarea datelor: codul PIN este definit la compilare și stocat în memoria Flash read-only a microcontrolerului. PIN-ul nu poate fi modificat de utilizator fără a recompila și a reîncărca firmware-ul pe dispozitiv. 
  
  * alimentare și consum: sistemul este proiectat să funționeze la o tensiune standard, putand fi alimentat printr-un cablu USB.

<a id="scenariu-de-testare"></a>

## 2.3. Scenariu de testare:

Se testează funcționarea sistemului „Seif Inteligent” într-un mediu controlat. Utilizatorul introduce codul PIN prin tastatura matrieală, iar sistemul trebuie să valideze codul și să acționeze mecanismul servo corespunzător.

În timpul testării se verifică:

* afișarea corectă a stării pe LCD la fiecare apăsare de tastă;

* deblocarea seifului și deplasarea servomotorului la introducerea PIN-ului corect;

* afișarea mesajului de eroare și activarea buzzer-ului la PIN greșit;

* intrarea în modul LOCKOUT după 3 tentatice eșuate consecutive, cu un countdown pe LCD;

* declanșarea alarmei când seiful este închis și fotorezistența detectează lumină;

* oprirea corectă a alarmei și resetarea stării la deschiderea validă a seifului.

Testul este considerat reușit dacă sistemul răspunde corect la toate scenariile de mai sus fără erori sau blocaje software.

---

<a id="componente"></a>

# 3. Componente

<a id="schema-bloc"></a>

## 3.1. Schema Bloc

Schema bloc de ansamblu ilustrează conexiunile fizice dintre componentele sistemului: microcontrolerul Raspberry Pi Pico W ca element central, fotorezistența, servomotorul, tastatura matriceală, modulul LCD cu interfață SPI și buzzer-ul. Logica de verificare a PIN-ului introdus conectează tastatura cu LCD-ul și cu celelalte periferice.

![Schita arhitectura sistem](images/schita_arhitectura.jpeg)

<a id="descrierea-componentelor"></a>

## 3.2. Descrierea componentelor

1. Tastatura (Keypad)
Este interfața principală de input pentru utilizator.
  * Legături (4 GPIO OUT - Linii & 4 GPIO IN - Coloane): Tastatura funcționează prin scanarea unei matrice. Microcontrolerul trimite semnale pe cele 4 linii și citește starea celor 4 coloane pentru a determina exact ce tastă a fost apăsată.
  * Rol: Permite introducerea codului PIN de 4 cifre.
2. Modulul LCD (Afișaj)
  * Legătura SPI: Folosește protocolul de comunicare Serial Peripheral Interface (SPI).
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
6. Raspberry Pi Pico W <br>
  Unitatea centrală de procesare. Coordonează toate perifericele.
  * cirire ADC cu DMA (GP27 - canal ADC1);
  * gennerare PWM (GP15 la servo);
  * comunicare SPI0 (GP2-GP6 pentru LCD);
  * scanare tastatură (GP16-GP22, GP26);
  Firmware-ul compilat în C cu SDK-ul oficial Pico, cu output USB activat.

---

<a id="arhitectura"></a>

# 4. Arhitectură

<a id="definire-arhitectura"></a>

## 4.1. Definire arhitectură

Sistemul este organizat în jurul microcontrolerului Raspberry Pi Pico W, care coordonează cinci subsisteme principale vizibile în diagrama software:

1. Verificare efracție (ldr_lumina_detectata)<br>
  La fiecare iterație a buclei principale, se citește valoarea medie din buffer-ul DMA al ADC. Dacă seiful este închis și valoarea depășește pragul, se activează alarma_start() și se actualizează interfața.

2. Așteptare și citire input (keypad_scan)<br>
  Funcția blocează până la apăsarea și eliberarea unei taste, returând caracterul corespunzător din matricea keymap. Debounce-ul este integrat direct în driver.

3. Actualizare ecran (lcd_show_pin/ok/err/closed/alarm/lockout)<br>
  LCD-ul este actualizat sincron la fiecare eveniment: apăsare tastă, eroare PIN, alarmă, intrare în lockout.

4. Deblocare/Blocare (servo_deschide/inchide)<br>
  La validarea PIN-ului, servomotorul execută rotația lentă la 230 grade. La apăsarea * cu seiful deschis, se întoarce la 0 grade.

5. Redare alerte (buzzer_beep/alarm_start)<br>
  Bip-urile punctuale sunt sincrone (blochează scurt). Alarma de efracție este asincronă, gestionată de un repeating_timer la 500 ms.

<a id="schema-electrica"></a>

## 4.2. Schema Electrică

Schema electrică a sistemului a fost realizată în KiCad și conține toate componentele și conexiunile electrice ale proiectului. Componentele principale prezente în schemă sunt:

- A1 - Raspberry Pi Pico W<br>
- DS1 - modulul LCD 1.8 SPI (ST7735)<br>
- J1 - conectorul Keypad 4x4<br>
- BZ1 - buzzer-ul<br>
- M1 - servomotorul<br>
- LDR07 - fotorezistența<br>
- R2 - rezistența de divizor pentru fotorezistență<br>

![Schema electrică KiCad](images/schema-electrica.png)

Conexiuni principale conform schemei:

| Semnal | Pin Pico | Componentă |
|---|---|---|
| SPI0 SCK | GP2 | LCD SCL |
| SPI0 MOSI | GP3 | LCD SDA |
| LCD DC/AO | GP4 | LCD AO |
| LCD CS | GP5 | LCD CS |
| LCD RST | GP6 | LCD RESET |
| Buzzer | GP14 | BZ1 + |
| Servo PWM | GP15 | M1 Motor_Servo |
| Linii tastatura | GP16–GP19 | J1 Keypad_4x4 |
| Coloane tastatura | GP20–GP22, GP26 | J1 Keypad_4x4 |
| LDR ADC | GP27 (ADC1) | LDR07 (prin R2) |

<a id="arhitectura-software"></a>

## 4.3. Arhitectura Software

Arhitectura software este structurată modular, cu fișierele dedicate fiecărui periferic, coordonate de fișierul principal. Diagrama de mai jos ilustrează relațiile dintre module:

![Diagrama arhitectura software](images/diagrama_software.png)

Arhitectura este structurată pe două nivele ierarhice:

1. Nivelul de coordonare - seif.c<br>
Reprezintă „creierul” proiectului. Conține bucla principală care orcheztrează secvențial cele 5 acțiuni majore: verificare LDR, citire tastatură, actualizare LCD, control servo și semnalizare audio. Gestionează variabilele de stare globale seif_deschis, incercari, alarma_ldr și logica FSM implicită a sistemului.

2. Nivelul modulelor - driverele periferice<br>
- senzor.c - modul securitate (ADC, DMA, Timer): configurează ADC în free-running cu transfer DMA continuu în buffer de 8 eșantioane. Gestionează repeating_timer pentru alarma sonoră asincronă la 500 ms. Expune ldr_lumina_detectata(), alarma_start(), alarma_stop().<br>
- buzzer.c - modul audio (GPIO digital): driver simplu pentru un buzzer pasiv. Comutare digitală HIGH/LOW pe GP14. Expune buzzer_init și buzzer_beep(ms)<br>
- tastatura.c - modul input (matrice GPIO): scanare polling a matricei 4x4 cu debounce complet. Expune keypad_scan() care returnează caracterul tastei apăsate sau 0 dacă nicio tastă nu e apăsată.<br>
- lcd.c - modul afișare (driver ST7735): comunicare SPI blocantă la 20 MHz. Font 5x7 integrat cu scalare configurabilă. Expune funcții de afișare pentru fiecare stare a sistemului.<br>
- servo.c - modul acționare (control PWM): generare semnal PWM cu parametrii calibrați pentru servo standard. Rotație lentă prin incrementare. Expune servo_deschide() și servo_inchide().

---

<a id="scenarii-diagrame"></a>

# 5. Scenarii și diagrame

<a id="use-case"></a>

## 5.1. Diagrama use-case

![Diagrama use-case](images/use-case.png)

Sisetmul are două tipuri de actori:<br>
- Utilizatorul fizic - interacționează cu tastatura<br>
- Senzorul de lumină - acționează automat ca declanșator de alarme<br>

Cazurile principale:<br>
- introducere PINc<br>
- ștergere caracter<br>
- confirmare PIN<br>
- închidere seif<br>
- declanșare alarmă antifurt<br>

<a id="scenarii"></a>

## 5.2. Scenarii de funcționare

1. Pornirea sistemului
* alimentare: utilizatorul conectează RPi Pico W la sursa de alimentare (USB).
* inițializare software: se apelează secvențial: stdio_init_all(), keypad_init(), servo_init(), lcd_init(), buzzer_init(), ldr_init(). DMA-ul ADC pornește transferul continuu în adc_buff.
* starea inițială hardware: servomotorul se deplasează la 0 grade. Alarma este inactivă.
* feedback LCD: ecranul afișează promptul de introducere PIN cu 0 cifre.
* sistem activ: bucla while(true) pornește -> verificare LDR continuă, așteptare input tastatură.

2. Deblocarea seifului (PIN corect)
* introducerea PIN-ului: utilizatorul apasă cele 4 cifre. La fiecare apăsare, caracterul se adaugă în input și LCD-ul afișează un * suplimentar (lcd_show_pin(idx++)).
* confirmare (#): utilizatorul apasă #. Sistemul compară input cu PAROLA prin strcmp().
* validare pozitivă: contorul incercari se resetează la 0. Ecranul devine verde (lcd_show_ok()).
* oprire alarmă (dacă e activă): alarma_stop() oprește timer-ul și buzzer-ul.
* semnalizare și deblocare: buzzer_beep(100) de două ori. servo_deschide() rotește la 230. seif_deschis = true.

3. Tentativă eșuată și blocare anti-brute-force
* PIN greșit: utilizatorul introduce un cod incorect și apasă #. strcmp returnează valoare != 0.
* feedback negativ: ecranul devine roșu (lcd_show_err()). buzzer_beep(500), incercari++. Pauza 1500 ms.
* depășire limită: dacă incercări >= 3, se apelează lockout_sequence(). Contorul se resetează la 0.
* modul lockout: ecranul afișează BLOCAT! cu countdown de 30 secunde. La fiecare secundă: buzzer 100 ms ON, 900 ms OFF.
* revenire: la exprarea timpului (30s), sistemul revine la starea inițială de introducere PIN.

4. Alarmă de efracție (LDR)
* detecție lumină: cu seiful în stare închisă, ldr_lumină_detectată returnează true.
* declanșare alarmă: alarma_ldr = true. alarma_start() pornește repeating_timer la 500ms. LCD arată ecranul de alarmă (lcd_show_alarm())
* semnalizare continuă: buzzer comută la 2 Hz prin callback-ul timer-ului (alarm_timer_callback), independent de bucla principală.
* oprire la PIN corect: la validarea PIN-ului, alarma_stop() oprește timer-ul și buzzer-ul. alarma_ldr = false. Seiful se deschide normal.
* oprire la lockout: lockout_sequence() oprește alarma pe durata blocării, apoi o repornește dacă alarma_ldr rămâne activ.

5. Închiderea seifului
* comandă închidere: utilizatorul apasă tasta * cu seif_deschis = true.
* blocare mecanică: servo_închide rotește servomotorul înapoi la 0. seif_deschis = false, alarma_ldr = false.
* feedback LCD: lcd_show_closed() afișează INCHIS timp de 1500 ms, apoi lcd_show_pin(0).

<a id="diagrama-secventa"></a>

## 5.3. Diagrama de secvența

![Diagrama use-case](images/diagrama_secventa.png)

---

<a id="etape"></a>

# 6. Etape de realizare

Etapa I - Proiectarea și asamblarea Hardware <br>
Realizarea schemei bloc și schemei electrice și asamblarea componentelor pe breadboard. Verificarea tensiunilor de alimentare: 3.3V (VCC) pentru logică, LDR și LCD; 5V (VBUS) pentru servomotor. Verificarea continuității și a maselor comune. Alinierea fizică a fotorezistenței în interiorul seifului pentru detecția optimă a luminii la deschiderea ușii.

![Asamblarea elementelor pe breadboard](images/firee.jpeg)
![Asamblarea servomotorului și a fotorezistenței](images/seif_interior.jpeg)
![Look final](images/seif_exterior.png)

Etapa II - Drivere low-level <br>
Implementarea driveruui SPI pentru LCD ST7735 cu fontul 5x7 integrat. Configurarea ADC în free-running cu DMA pe canal ADC1 (GP27) și buffer circular de 8 eșantioane. Implementarea scanării matricei 4x4 cu debounce. Configurarea PWM pentru servo.

Etapa III - Logica de control și FSM <br>
Implementarea buclei principale în seif.c cu gestionarea tuturor stărilor: introducere PIN, validare prin strcmp(), contorizare erori, lockout sequence și control alarmă. Configurarea repeating_timer pentru alarma non-blocantă la 500 ms. Testarea tranzițiilor in toate stările.

Etapa IV - Calibrare și optimizare<br>
Calibrarea pragului LDR_THRESHOLD în condiții reale de lumină. (analizăm cea mai mare valoare citită cu seiful închis și punem o valoare ușor mai mare). Ajustarea unghiurilor servo (0/230) pentru mecanism fizic. Verificarea debounce-ului tastaturii cu apăsări rapide repetate. Monitorizarea valorilor ADC și a stărilor FSM.

<a id="bibliografie"></a>

# 7. Bibliografie

- [Raspberry Pi Pico Datasheet (RP2040)](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

- [Raspberry Pi Pico SDK — ADC, DMA, PWM, SPI](https://www.raspberrypi.com/documentation/pico-sdk/)

- [ST7735 LCD Controller Datasheet](https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf)

- [Protocolul SPI — Referință Generală](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)

- [Embedded Systems Using the Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/)