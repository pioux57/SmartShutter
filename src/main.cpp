/*
 *  Boitier de commande pour le pilotage d'un appareil reflex et commande de focus sur telescope.
 *  Mode de detection de variation brusque de lumière via un phototransistor de type 3DU5C pour déclenchement automatique de la prise de vue en cas de détection de foudre
 *  Fonction de declenchement programmé pour fonction timelapse
 *  Composants: 
 *    - Affichage sur écran OLED via protocole I2C
 *
 *  Dev: Christian Poupounot // Date: 14 mars 2023
 *  Adapte du code de Florent Pin (https://forum.chasseurs-orages.com/viewtopic.php?t=6243) credits orginaux ;)
 *  Codage bitmap 24bits via http://javl.github.io/image2cpp/
 */

/********************************************/
//           Inclusion Bibliothéques
/********************************************/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/********************************************/
//              Hardware
/********************************************/
// Ecran OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(128,64, &Wire, OLED_RESET);

/********************************************/
//              Software
/********************************************/
// Versionning
#define SOFTWARE_VERSION 	"1.11"
#define SOFTWARE_AUTHOR		"Christian Poupounot"

// // 'logo-orionpx', 128x64px
// const unsigned char logo_orionpx [] PROGMEM = {
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfd, 0xff, 0x80, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7e, 0xc0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xbd, 0xc0, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc3, 0xe0, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc3, 0xe0, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc7, 0xf0, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xd3, 0xf0, 0x00, 0xdf, 0xf8, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xb1, 0xf8, 0x00, 0xdf, 0xf0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfb, 0xf8, 0x01, 0xdf, 0xf0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfb, 0xfc, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x03, 0xfd, 0xfc, 0x03, 0xbf, 0xe0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0xfd, 0xfe, 0x03, 0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x7d, 0xfe, 0x07, 0xbf, 0xc0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x7e, 0xff, 0x07, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x7e, 0xff, 0x0f, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x3f, 0x7f, 0x8f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0x7f, 0x9f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xdf, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0xbf, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x7f, 0xff, 0xfd, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x7f, 0xdf, 0xfd, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0xff, 0xdf, 0xfd, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x01, 0xff, 0xef, 0xfb, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x07, 0xff, 0xef, 0xfb, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xef, 0xfb, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfb, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xfb, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x9d, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xfd, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xb9, 0xfe, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfe, 0x7b, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfc, 0x7b, 0xff, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xe0, 0xfb, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x01, 0xf7, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x03, 0xf7, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x03, 0xf7, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x07, 0xff, 0x9f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x07, 0xff, 0x9f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0x0f, 0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x1f, 0xfe, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0xfe, 0x07, 0xfb, 0xc0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x3f, 0xfc, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x7f, 0xfc, 0x03, 0xfd, 0xe0, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x7f, 0xf8, 0x01, 0xfc, 0x70, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0xff, 0xb8, 0x01, 0xfc, 0x70, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x01, 0xff, 0xb8, 0x00, 0xfe, 0xf8, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x01, 0xff, 0xf0, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x03, 0xff, 0x70, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x03, 0xff, 0x60, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x07, 0xff, 0x60, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x0f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x0f, 0xff, 0xc0, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x1f, 0xff, 0x80, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x1f, 0xff, 0x80, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
// 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

////////////////////////////////////////
// Constantes de réglage
#define POTENTIOMETER_MAX   940         // Valeur maximum du potentiometre
#define POTENTIOMETER_MIN   30          // Valeur minimum du potentiometre
#define SENSIBILITY_MAX     100         // Sensibilité maximum réglable
#define SENSIBILITY_MIN     3           // Sensibilité minimum réglable
#define HISTORY_DEPTH       16          // Profondeur de la matrice d'historique des valeurs (default 16)
#define VALUE_MAX           960         // Valeur maximum donnée par le phototransistor
#define DEBOUNCE_TIME       10          // Nb de millisecondes pour debouncer les boutons
#define TIMELAPSE_GAP       10          // Nb de minutes par défaut du timelapse
#define TIMELAPSE_MAX       1440        // Durée maximum du timelapse en minutes
#define USB_LATENCY         10

////////////////////////////////////////
// Communication
struct UARTByte {
	bool value_ok = false;
	int  value = 0;
};

////////////////////////////////////////
// Config I/O
const int triggerPIN        = 11;       // Commande de l'optocoupleur pour le déclenchement
const int prefocusPIN       = 12;       // Commande de l'optocoupleur pour le focus
const int transistorPIN     = A0;       // Entrée analogique du phototransistor
const int potoPIN           = A1;       // Entrée analogique du potentiometre
/*
 *   A4 - Ecran OLED
 *   A5 - Ecran OLED
 */
const int greenLedPIN       = 5;        // Pin Led verte
const int redLedPIN         = 4;        // Pin Led rouge
const int buttonModePIN     = 2;        // Bouton Mode (PULL-DOWN)
const int buttonSetPIN      = 3;        // Bouton Set (PULL-DOWN)

////////////////////////////////////////
// Variables globales
byte shutterMode                = 0;        // 0=Mode orage     / 1=mode timelapse  / 2=remote
byte detectionMode              = 0;        // 0=Mode variation / 1=mode moyenne    / 2=mode fixe
bool displaySplash			    = true;
bool initFinished               = false;
int potentiometerValue          = 0;
byte potentiometerValueForced   = 0;
int sensibility                 = 0;
float ratioScreen               = 0.0;
byte i;
// Variable PC
bool usbDriven                  = false;    // Controle via USB
bool usbDisconnect              = false;    // Indique une deconnexion du PC
bool sendData2Serial            = false;
bool lockSensibility            = false;
// Variables mode orage
float ratioSensibility          = 0.0;
bool samplingPhase              = true;     // Mode initial d'échantillonnage avant de lancer la détection
float ratioGraph                = 0.0;
float thresholdValue            = 0.0;
int sensorValue                 = 0;        // Dernière valeur lue
int prevSensorValue             = 0;        // Précédente valeur lue
int gap                         = 0;
int averageValue                = 0;
int nbShot                      = 0;
int sampleRate                  = 0;
int sampleNb                    = 0;
unsigned long int lastRate      = 0;
int valueHistory[HISTORY_DEPTH];
byte indexHistory               = 0;
byte indexGraph                 = 0;
byte graphMaxPlot               = 28;
byte plotWidth                  = 1;
byte yThreshold                 = 0;
byte barHeight                  = 0;
// Variables mode timelapse
float ratioTimelapse            = 0.0;
int timelapseGap                = TIMELAPSE_GAP;
bool timelapseWIP               = false;
unsigned long int lastShot      = 0;

////////////////////////////////////////
// Variables hors registres pour les interruptions
volatile boolean buttonModeWIP  = false;
volatile boolean buttonSetWIP   = false;

/********************************************/
//              Déclaration fonctions
/********************************************/
void displayOLEDSplash();
void triggerShot();
bool readModeButton();
bool readSetButton();

void modeLightning();
void modeTimelapse();
void modeRemote();
void modeUSB();
void usbControl(bool state);

void modeButtonISR();       // Interrupt Service Routine on button Mode
void setButtonISR();        // Interrupt Service Routine on button Set

/********************************************/
//              Initialisation
/********************************************/
void setup() {
	// Ouverture communication avec le PC pour debug
    Serial.begin(9600);
    Serial.println("");

    // Calcul ratios
    ratioScreen         = (float)128/POTENTIOMETER_MAX;
    ratioSensibility    = (float)SENSIBILITY_MAX/POTENTIOMETER_MAX;
    ratioTimelapse      = (float)TIMELAPSE_MAX/POTENTIOMETER_MAX;
    ratioGraph          = (float)graphMaxPlot/VALUE_MAX;
    plotWidth           = 128/HISTORY_DEPTH;
    Serial.print(F("* [System] Rapport sensibilite/ecran..."));
    Serial.println(ratioScreen);
    Serial.print(F("* [System] Rapport sensibilite/potentiometre..."));
    Serial.println(ratioSensibility);

    // Initialisation de la matrice de mesure
    for ( i = 0 ; i < HISTORY_DEPTH ; i++ ){ valueHistory[i] = 0; }

    // Hardware init
    Serial.println(F("* [System] Parametrage des entrees/sorties..."));
    pinMode(triggerPIN,OUTPUT);
    pinMode(prefocusPIN,OUTPUT);
    pinMode(redLedPIN,OUTPUT);
    pinMode(greenLedPIN,OUTPUT);
    pinMode(buttonModePIN,INPUT);
    pinMode(buttonSetPIN,INPUT);

    digitalWrite(triggerPIN,LOW);
    digitalWrite(prefocusPIN,HIGH);    // Activation permanente du focus
    digitalWrite(redLedPIN,LOW);
    digitalWrite(greenLedPIN,LOW);

    // Interruptions
    Serial.println(F("* [System] Liaison des fonctions ISR et interruptions..."));
    attachInterrupt(digitalPinToInterrupt(buttonModePIN), modeButtonISR , RISING);
    attachInterrupt(digitalPinToInterrupt(buttonSetPIN) , setButtonISR  , RISING);

    // Initialisation Ecran OLED
	Serial.println(F("* [I2C] Initialisation ecran OLED...0x3C"));
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialise avec l'adresse I2C 0x3D (Pour le 128x64) 
	
    // Affichage du splash screen pour connaitre la version du software installé
    Serial.println(F("* [System] Affichage du Splash Screen..."));
    displayOLEDSplash();
	display.setTextColor(WHITE);

    // Test des leds
    Serial.println(F("* [System] Test LEDs..."));
    digitalWrite(redLedPIN,HIGH);
    delay(1000);
    digitalWrite(redLedPIN,LOW);
    digitalWrite(greenLedPIN,HIGH);
    delay(1000);
    digitalWrite(greenLedPIN,LOW);

    initFinished = true;
    Serial.println(F("* [System] ---------- Initialisation Terminee ------------"));
    delay(200);
    Serial.print(F("[SMARTSHUTTER READY]"));
    delay(500);
    Serial.println("");
}

/********************************************/
//              Main Block
/********************************************/
void loop() {
    display.clearDisplay();

    // Relevé des valeurs
    sensorValue = analogRead(transistorPIN);
    sampleNb++;
    if (!lockSensibility) { potentiometerValue = POTENTIOMETER_MAX - analogRead(potoPIN); } // On soustrait car on veut une lecture inversée de la position du potar
    else{ potentiometerValue = potentiometerValueForced * 10; }

    if (usbDriven && sendData2Serial){
        Serial.print(F("pvalue="));
        Serial.println(potentiometerValue);
        delay(USB_LATENCY);
        Serial.print(F("value="));
        Serial.println(sensorValue);
        delay(USB_LATENCY);
    }

    if (readModeButton() || usbDisconnect){
        if (!usbDisconnect){
            shutterMode++;
            if (shutterMode >= 3){shutterMode = 0;}
        }

        usbDisconnect = false;

        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0); // 9 pixels pour un char (8) + son espace (1)

        switch(shutterMode){
            case 0:
                display.println("Mode");
                display.println(" Orage");
                
                samplingPhase   = true;
                indexHistory    = 0;
                nbShot          = 0;
            break;
            case 1:
                display.println("Mode");
                display.println(" TimeLapse");
                timelapseWIP = false;
            break;
            case 2:
                display.println("Mode");
                display.println(" Controle");
            break;
        }

        display.setTextSize(1);
        display.display();
        delay(1000);
        display.clearDisplay();
    }

    // if (!usbDriven){
        switch(shutterMode){
            case 0:
                modeLightning();
            break;
            case 1:
                modeTimelapse();
            break;
            case 2:
                modeRemote();
            break;
        }
    // } else {
    //     modeUSB();
    // }
    
    display.display();

}

/********************************************/
//              Fonctions
/********************************************/

//--------------------------------------------
// Gestion du mode orage
void modeLightning(){
    
    // Gestion du mode de detection
    if (readSetButton()){
        detectionMode++;
        if (detectionMode >= 3){detectionMode = 0;}
    }

    // Taille du graph en fonction du mode
    if (samplingPhase){graphMaxPlot = 40;;}
    else{
        switch(detectionMode){
            case 0: graphMaxPlot = 28; break;
            case 1: graphMaxPlot = 28; break;
            case 2: graphMaxPlot = 40; break;
        }
    }

    // Calcul du taux de mesures
    if (millis() - lastRate > 1000){
        sampleRate = sampleNb;
        sampleNb = 0;
        lastRate = millis();
    }

    // Decalage axe x du curseur du graph
    indexGraph++;
    if (indexGraph >= HISTORY_DEPTH) {indexGraph = 0;}

    display.fillRect(indexGraph*plotWidth,10,1,graphMaxPlot,WHITE);

    // Historique des mesures
    valueHistory[indexHistory] = sensorValue;
    indexHistory++;
    if (indexHistory >= HISTORY_DEPTH) {
        indexHistory = 0;
        samplingPhase = false; // On a un tableau complet, on peut passer au mode détection
    }

    // Affichage du graph et Calcul moyenne des mesures
    for (i = 0 ; i < HISTORY_DEPTH ; i++){
        averageValue += valueHistory[i];
        barHeight = (int)valueHistory[i]*ratioGraph;
        // display.fillRect(i*2,10,2,barHeight,WHITE);
        display.fillRect(i*plotWidth,(graphMaxPlot+10)-barHeight,plotWidth,barHeight,WHITE);
    }
    averageValue = averageValue / HISTORY_DEPTH;

    // Calcul seuil de detection
    sensibility = SENSIBILITY_MAX - (potentiometerValue * ratioSensibility);

    // Saturation du phototransistor
    if(sensorValue > VALUE_MAX) {digitalWrite(redLedPIN,1);} else {digitalWrite(redLedPIN,0);}  

    // Detection d'un dépassement de seuil
    if (!samplingPhase){
        switch(detectionMode){
            // Mode variation
            case 0:
                gap = sensorValue - prevSensorValue;
                if(gap > sensibility){triggerShot();}
            break;
            // Mode moyenne
            case 1:
                // Affichage du seuil de déclenchement sur le graph
                yThreshold = (graphMaxPlot+10)-((averageValue + sensibility)*ratioGraph);
                display.drawFastHLine(0,yThreshold,128,WHITE);

                // Detection du dépassement
                if(sensorValue >= (averageValue + sensibility)){triggerShot();}
            break;
            // Mode fixe
            case 2:
                // Affichage du seuil de déclenchement sur le graph
                thresholdValue = (float)sensibility / 100;
                thresholdValue *= VALUE_MAX;
                yThreshold = (graphMaxPlot + 10)-((int)thresholdValue*ratioGraph);
                display.drawFastHLine(0,yThreshold,128,WHITE);

                // Detection du dépassement
                if(sensorValue >= (int)thresholdValue){triggerShot();}
            break;
        }
    }

    ////////////////////// MODE ECHANTILLONAGE //////////////////////
    if (samplingPhase){
        // Affichage valeur courante
        display.setCursor(0,0); // 9 pixels pour un char (8) + son espace (1)
        display.print("Valeur capteur:");
        display.println(sensorValue);
        display.setCursor(0,52); // 9 pixels pour un char (8) + son espace (1)
        display.println("Echantillonnage...");
    }
    ////////////////////// MODE DETECTION //////////////////////
    else {
        // Affichage du nombre de déclenchements
        display.setCursor(0,0); // 9 pixels pour un char (8) + son espace (1)
        display.print("nbS: ");
        display.println(nbShot);
        display.setCursor(50,0); // 9 pixels pour un char (8) + son espace (1)
        display.print("e/s: ");
        display.println(sampleRate);

        // Affichage bargraph sensibilité
        if (detectionMode == 0 || detectionMode == 1){
            display.setCursor(0,40); // 9 pixels pour un char (8) + son espace (1)
            display.println("Sensibilite:");
            display.drawRect(0, 50, 127, 5, WHITE);
            int bar_size = (potentiometerValue * ratioScreen);
            display.fillRect(0, 50, bar_size, 5, WHITE);
        }

        if (detectionMode ==1){
            // Affichage sensibilité moyenne
            display.setCursor(0,56); // 9 pixels pour un char (8) + son espace (1)
            display.print("Valeur moyenne: ");
            display.println(averageValue);
        }
    }
    prevSensorValue = sensorValue;
}

//--------------------------------------------
// Gestion du mode TimeLapse
void modeTimelapse(){
    // Demarrage / Arret du timelapse
    if (usbDriven){
        display.setTextSize(2);
        display.setCursor(0,25);
        display.println(F("Controle  Via USB"));
        display.setTextSize(1);
    }
    else {
        if (readSetButton()){
            timelapseWIP = !timelapseWIP;
            lastShot = 0;
        }

        if (timelapseWIP){
            int secElapsed = (millis() - lastShot) / 1000;  // Nombre de secondes écoulées depuis la derniere PDV
            if (secElapsed >= timelapseGap * 60){
                triggerShot();
                lastShot = millis();
            }
            display.setTextSize(1);
            display.setCursor(0,0);
            display.print(F("Decompte: "));
            display.setTextSize(2);
            display.setCursor(30,15);
            
            display.print( timelapseGap * 60 - secElapsed );
            display.print("s");
            
            display.setTextSize(1);
            display.setCursor(0,50);
            display.print(F("Interval: "));
            display.print(timelapseGap * 60);
            display.println("s");
        } else {
            // Calcul réglage du timelapse
            timelapseGap = potentiometerValue * ratioTimelapse;
            if (timelapseGap < 0){timelapseGap = 0;}
            if (timelapseGap > TIMELAPSE_MAX){timelapseGap = TIMELAPSE_MAX;}

            display.setTextSize(2);
            display.setCursor(50,10);
            display.println(timelapseGap);
            display.setTextSize(1);
            display.setCursor(40,50);
            display.println(" (minutes)");
        }
    }
}

//--------------------------------------------
// Gestion du mode Remote
void modeRemote(){
    if (readSetButton()){ triggerShot(); }

    display.setCursor(0,28);
    display.println(F(" Attente..."));
}

//--------------------------------------------
// Gestion du mode Remote
void modeUSB(){
    display.setCursor(0,28);
    display.println(F(" Controle via PC"));
}

//--------------------------------------------
// Affichage du Splash screen sur l'écran OLED
void displayOLEDSplash(){
    // display.clearDisplay();
    // display.drawBitmap(0, 0,  logo_orionpx, 128, 64, WHITE);
    // display.display();
    // delay(2000);
    display.clearDisplay();
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0,0); // 9 pixels pour un char (8) + son espace (1)
	display.println("Smart");
    display.println("  Shutter");
	display.setTextSize(1);
    display.println("");
    display.print("Firmware: v");
	display.println(SOFTWARE_VERSION);
	display.setTextSize(1);
	display.print(SOFTWARE_AUTHOR);

	display.display();
	displaySplash = false;
    delay(3000);
    display.clearDisplay();
}

//--------------------------------------------
// Déclenche la prise de vue
void triggerShot(){
    digitalWrite(greenLedPIN,1);
    digitalWrite(triggerPIN,HIGH);

    display.clearDisplay();
	display.setTextSize(3);
    display.setCursor(0,20);
    display.println(" SHOT !");
    display.display();
    display.setTextSize(1);

    delay(300);
    digitalWrite(greenLedPIN,0);
    digitalWrite(triggerPIN,LOW);
    nbShot++;
}

//--------------------------------------------
// Gère l'appui sur le bouton Mode
bool readModeButton(){
    int buttonState;
    static bool     switchingPending = false;
    static long int elapseTimer;
    if (buttonModeWIP) {
        buttonState = digitalRead(buttonModePIN);
        if (buttonState == HIGH) {
            switchingPending    = true;
            elapseTimer         = millis();
        }

        if (switchingPending && buttonState == LOW) {
            if (millis() - elapseTimer >= DEBOUNCE_TIME) {
                switchingPending = false;
                buttonModeWIP    = false;
                return true;
            }
        }
    }
    return false;
}

//--------------------------------------------
// Gère l'appui sur le bouton Mode
bool readSetButton(){
    int buttonState;
    static bool     switchingPending = false;
    static long int elapseTimer;
    if (buttonSetWIP) {
        buttonState = digitalRead(buttonSetPIN);
        if (buttonState == HIGH) {
            switchingPending    = true;
            elapseTimer         = millis();
        }

        if (switchingPending && buttonState == LOW) {
            if (millis() - elapseTimer >= DEBOUNCE_TIME) {
                switchingPending = false;
                buttonSetWIP     = false;
                return true;
            }
        }
    }
    return false;
}

//--------------------------------------------
// Prise de controle par le PC
void usbControl(bool state){
    if (state){
        // Activation du mode controle à distance
        usbDriven = true;
        Serial.println(F("[USB_CONNECT] Prise de controle PC"));
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0,0); // 9 pixels pour un char (8) + son espace (1)
        display.println("USB");
        display.println(" Connect");
        display.setTextSize(1);
        display.display();
        
        // Transmission des parametres au PC
        Serial.print(F("v="));
        Serial.println(SOFTWARE_VERSION);
        delay(USB_LATENCY);
        Serial.print(F("pmax="));
        Serial.println(POTENTIOMETER_MAX);
        delay(USB_LATENCY);
        Serial.print(F("vmax="));
        Serial.println(VALUE_MAX);
        delay(USB_LATENCY);
        delay(1000);

    } else {
        usbDriven       = false;
        usbDisconnect   = true;
        sendData2Serial = false;
        lockSensibility = false;

        Serial.print(F("* [USB] Deconnexion PC"));
    }
}

/********************************************/
//              Fonctions ISR
/********************************************/

//--------------------------------------------
// Action sur le bouton Set
void setButtonISR(){
    if (!usbDriven){
        Serial.print(F("* [I/O] Appui detecte sur bouton Set...["));
        Serial.print(digitalRead(buttonSetPIN));
        Serial.print("] - ");
        if (initFinished && !buttonSetWIP && digitalRead(buttonSetPIN) == HIGH){
            buttonSetWIP = true;
            Serial.println("WIP");
        } else { Serial.println("NOK"); }
    }
}

//--------------------------------------------
// Action sur le bouton Mode
void modeButtonISR(){
    if (!usbDriven){
        Serial.print(F("* [I/O] Appui detecte sur bouton Mode...["));
        Serial.print(digitalRead(buttonModePIN));
        Serial.print("] - ");
        if (initFinished && !buttonModeWIP && digitalRead(buttonModePIN) == HIGH){
            buttonModeWIP = true;
            Serial.println("WIP");
        } else { Serial.println("NOK"); }
    }
}

/********************************************/
//    Fonction d'interruption voie serie
/********************************************/

//--------------------------------------------
// Interruption voie serie pour le PC
void serialEvent(){
	UARTByte order;

	while (Serial.available()){
		order.value = Serial.read();
		if (order.value != 0 && order.value != -1){
			if (order.value == 65)  { triggerShot(); }              // Demande de prise de vue
            if (order.value == 18)  { usbControl(true); }           // Prise de controle du PC
            if (order.value == 16)  { usbControl(false); }          // Relache du controle du PC
            if (order.value == 5)   { sendData2Serial = false; }    // Le PC ne veut plus des données
            if (order.value == 6)   { sendData2Serial = true; }     // Le PC veut des données
            if (order.value == 10)  { lockSensibility = true; }     // On veut forcer la sensibilité
            if (order.value == 11)  { lockSensibility = false; }    // On veut forcer la sensibilité
            if (order.value == 20)  { shutterMode = 0; }            // Mode orage
            if (order.value == 21)  { shutterMode = 1; }            // Mode timelapse
            if (order.value == 22)  { shutterMode = 2; }            // Mode remote
            if (order.value == 30)  { detectionMode = 0; }          // Mode variation
            if (order.value == 31)  { detectionMode = 1; }          // Mode moyenne
            if (order.value == 32)  { detectionMode = 2; }          // Mode fixe
            if (order.value == 4)   { potentiometerValueForced = Serial.read(); } // On veut forcer la sensibilité avec une valeur entiere
		}
	}
}
