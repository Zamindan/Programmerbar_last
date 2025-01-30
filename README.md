Dette er alt som har med programmering å gjøre for den programmerbare lasten.

### 1. Installer ESP-IDF
Først må du installere ESP-IDF. Dette er et rammeverk som brukes for å programmere ESP32 mikrokontrollere. I vårt tilfelle ESP32-S3
  1. Gå til [ESP_IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html).
  2. Følg instruksjonene for Windows.
 > [!CAUTION]
 > 3. Pass på at path for installasjonen blir `C:/Espressif`.

### 2. Installer GitHub Desktop
GitHub Desktop er et verktøy som gjør det enkelt å jobbe med Git og GitHub. Installer det for å klone og jobbe med repositoryet.
  1. Last ned [GitHub Desktop](https://github.com/apps/desktop).
  2. Installer programmet.
  3. Lag og/eller logg inn med GitHub-konto.

### 3. Klone reositoryet
Nå må du klone prosjektets repository til din maskin.
  1. Åpne GitHub Desktop
  2. Klikk på **File > Clone Repository**.
  3. Velg eller legg til URL til `Programmerbar_last`.
  4. Klikk på **Clone**.
  > [!Caution]
  > 5. Pass på at Pathen du velger ikke har noen spesialsymboler som **SPACE, andre spesialtegn og Norske bokstaver**.

### 4. Konfigurer `init_esp_idf.bat`
I repositoryet finner du en mappe kalt `tools` som inneholder en fil kalt `init_esp_idf.bat`. Denne filen må du endre slik at den fungerer på din maskin.
  1. Åpne `tools/init_esp_idf.bat` i VS Code.
  2. I denne filen må du legge til en unik identifikator fra din ESP-IDF installasjon
     ```
     @echo off
     echo Initialiserer ESP-IDF-miljøet...
     call "C:/Espressif/idf_cmd_init.bat" esp-idf-1d48cf7427aa464d60cc013d481736ef
     echo ESP-IDF-miljøet er klart!
     cmd
     ```
     - Her er det **esp-idf-1d48cf7427aa464d60cc013d481736ef** som det er snakk om, denne vil se annerledes ut hos deg.
  3. Åpne start menyen og finn **ESP-IDF 5.4 CMD**.
  4. Høyre klikk på den og trykk open file location.
  5. Høyre klikk på shortcut til **ESP-IDF 5.4 CMD** og trykk på **Properties**.
  6. Se under Target og finn din unike identifikator på slutten av Target, kopier denne.
  7. Erstatt den eksisterende identifikatoren i `init_esp_idf.bat` med den du kopierte.
  8. Lagre filen.

### 5. Åpne prosjektet i VS Code
Nå kan du åpne prosjektet i VS Code og bruke den integrerte terminalen til å jobbe med ESP-IDF.
  1. Åpne VS Code.
  2. Klikk på **File > Open Folder** og velg mappen der du klonet repositoryet.
  3. Åpne Terminalen i VS Code.
  4. Terminalen skal nå automatisk initialisere ESP-IDF-miljøet ved hjelp av `init_esp_idf.bat` som vi satt opp tidligere.

### 6. Test at det fungerer
For å teste at alt fungerer kan du prøve å bygge prosjektet.
  1. I VS Code, åpne en terminal.
  2. Kjøre følgende kommando for å bygge prosjektet
     ```
     idf.py build
     ```
   Hvis alt er gjort riktig skal du nå ikke få noen errors og det skal bygge helt fint.

