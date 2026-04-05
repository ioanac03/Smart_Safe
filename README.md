# proiect_soricarii_test

Seif Inteligent

Descriere: 
  -realizarea unui seif inteligent bazat pe un microcontroler Raspberry Pi Pico.
  -seiful se deblocheaza prin introducerea unui cod PIN, folosind un modul cu butoane.
  -starea sistemului afisata pe un ecran LCD.
  -mecanismul de deschidere realizat prin un servo motor.
  -un buzzer pentru semnalizarea tentativelor de acces neautorizat la introducerea repetata a unor coduri gresite.
  -un senzor de lumina in interior care, daca starea seifului este de "incuiat" si detecteaza lumina, declanseaza o alarma.
  
Cerinte functionale:
  -autentificare si control acces: utilizatorul introduce un cod PIN format din cateva cifre folosind butoanele hardware. Sistemul verifica acest cod cu parola, iar daca datele corespund, servo motorul descuie seiful.
  -interfata cu utilizatorul: pe ecranul LCD sunt afisate mesaje clare de stare, cum ar fi solicitarea introducerii PIN-ului, confirmarea deblocarii, notificarea unui PIN gresit sau mesaje de alarma.
  -securitate anti-brute-force: pentru a preveni ghicirea parolei, sistemul numara de cate ori este introdus un cod gresit. Dupa cateva incercari consecutive esuate, buzzer-ul emite o alarma pentru a semnala o activitate suspecta.

Cerinte non-functionale:
  -performata si tim de raspuns: interfata trebuie sa reactioneze rapid, fara intarzieri vizibile intre apasarea butoanelor si afisare pe ecran. De asemenea, reactia sistemului la efractie trebuie sa fie prompta, declansand alarma imediat dupa expunerea senzorului la lumina.
  -salvarea datelor: codul PIN este setat de utilizator si este salvat intr-o memorie non-volatila a microcontrolerului. Astfel, parola nu se pierde daca sursa de alimentare cedeaza. 
  -alimentare si consum: sistemul este proiectat sa funtioneze la o tensiune standard, putand fi alimentat printr-un cablu USB sau folosind baterii.


DEFINIRE ARHITECTURA SISTEM:
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
