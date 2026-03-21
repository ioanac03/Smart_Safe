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
