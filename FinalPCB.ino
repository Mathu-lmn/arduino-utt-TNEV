#include <Servo.h>
Servo myServo;
const int cp1 = 4;
const int cp2 = 5;
const int cp3 = 8;
const int cp4 = 7;
const int enablePin = 3;
const int enablePin2 = 6;

//interrupteurs

int etatinterrupteur = LOW; //interrupteur pour avancer
int etatinterrupteur2 = LOW; //interrupteur pour le lanceur
int avant = 1;

//variables pour la boucle de détection de bouteille après évitement

bool sensrota = true; // sens de rota (true = gauche)
int rota = 0; //rotation du moteur
bool bfind = false; //Détermine si la bouteille est en vue
int var = 1; //Paire ou impaire en évitant les bouteilles

//Ultrason

int trig = A4;
int echo = A3;
long lecture_echo;
long cm;

//lanceur

int angle;
int switchstate;
const int sp = 13;

void setup() {
  //module de déplacement
  pinMode(cp1, OUTPUT);
  pinMode(cp2, OUTPUT);
  pinMode(cp3, OUTPUT);
  pinMode(cp4, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(enablePin2, OUTPUT);
  //lanceur
  myServo.attach(12);
  pinMode(sp, INPUT);
  pinMode(A0, INPUT);
}
void stop() { //arrêt des roues
  digitalWrite(cp1, LOW);
  digitalWrite(cp2, LOW);
  digitalWrite(cp3, LOW);
  digitalWrite(cp4, LOW);
}
void av() { //avancer tout droit
  digitalWrite(cp1, HIGH);
  digitalWrite(cp2, LOW);
  digitalWrite(cp3, LOW);
  digitalWrite(cp4, HIGH);
}
void gl() { //tourner à gauche (go left)
  digitalWrite(cp1, HIGH);
  digitalWrite(cp2, LOW);
  digitalWrite(cp3, HIGH);
  digitalWrite(cp4, LOW);
}
void gr() { //tourner a droite (go right)
  digitalWrite(cp1, LOW);
  digitalWrite(cp2, HIGH);
  digitalWrite(cp3, LOW);
  digitalWrite(cp4, HIGH);
}

long us() { //distance de l'obstacle par ultrason
  long cm;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  lecture_echo = pulseIn(echo, HIGH);
  return lecture_echo / 58;
}
void loop() {
  analogWrite(enablePin, 225);
  analogWrite(enablePin2, 225);//les roues ne vont pas a la vitesse max
  cm = us();
  etatinterrupteur = digitalRead(A0);
  if (etatinterrupteur == HIGH) {
    avant = 2; //Permet lors de l'appui du bouton de lancer le premier test (slalom)
  }
  if (avant == 2) {
    if (cm <= 18) { //si bouteille détectée à moins de 18 cm
      if (var % 2 == 0) { //variable qui détermine si le robot évite à gauche de la bouteille
        stop();
        delay(500);
        //début de la séquence d'évitement
        gl();
        delay(775);
        sensrota = true; //définir sensrota comme true pour le balayage par la suite
        bfind = false; //bouteille n'est plus dans l'axe
        av();
        delay(1300);
        stop();
        delay(500);
        gr();
        delay(800);
        av();
        delay(2500);
        stop();
        delay(500);
        gr();
        delay(700);
        av();
        delay(1250);
        stop();
        delay(500);
        cm = us();
        //début de la séquence de détection de bouteille après évitement
        while (bfind == false) { //tant que le robot ne détecte pas la prochaine bouteille
          while (cm > 40 && rota < 50) { //tant qu'il ne détecte pas la bouteille à moins de 40 cm et qu'il a tourné de moins d'environ 120 degrés
            if (sensrota) {
              gl();
              delay(65);
            }
            else if (!sensrota) {
              gr();
              delay(70);
            }
            rota += 1;
            stop();
            delay(75);
            cm = us();
          }
          rota = 0; //reset de la rotation
          sensrota = !sensrota; //changement de sens de rotation quand les 120° sont atteints
          if (cm < 40) { //si bouteille détectée
            bfind = true;
            if (sensrota) {//le robot tourne un petit peu pour etre dans le bon axe (compense la portée latérale du capteur)
              gr();
              delay(120);
            }
            else if (!sensrota) {
              gl();
              delay(125);
            }
          }

        }
        var++; //la prochaine bouteille sera évitée de l'autre coté de la bouteille
      }
      else {
        stop();
        delay(500);
        //début de la séquence d'évitement
        gr();
        delay(650);
        sensrota = true; //définir sensrota comme true pour le balayage par la suite
        bfind = false; //bouteille n'est plus dans l'axe
        av();
        delay(1300);
        stop();
        delay(500);
        gl();
        delay(675);
        av();
        delay(2500);
        stop();
        delay(500);
        gl();
        delay(700);
        av();
        delay(1250);
        stop();
        delay(500);
        cm = us();
        //début de la séquence de détection de bouteille après évitement
        while (bfind == false) { //tant que le robot ne détecte pas la prochaine bouteille
          while (cm > 40 && rota < 50) {//tant qu'il ne détecte pas la bouteille à moins de 40 cm et qu'il a tourné de moins d'environ 120 degrés
            if (sensrota) {
              gr();
              delay(65);
            }
            else if (!sensrota) {
              gl();
              delay(70);
            }
            rota += 1;
            stop();
            delay(75);
            cm = us();
          }
          rota = 0; //reset de la rotation
          sensrota = !sensrota; //changement de sens de rotation quand les 120° sont atteints
          if (cm < 40) { //si bouteille détectée
            bfind = true;
            if (sensrota) {//le robot tourne un petit peu pour etre dans le bon axe (compense la portée latérale du capteur)
              gl();
              delay(120);
            }
            else if (!sensrota) {
              gr();
              delay(125);
            }
          }
        }
        var++;
      }
    }
    else { //si la première bouteille n'est pas détectée à moins de 18 cm
      av();
    }
  }
  else { //tant que le bouton pour lancer le moteur n'est pas activé, on peut appuyer sur l'autre bouton pour le lancer avec la catapulte
    switchstate = digitalRead(sp);
    if (switchstate == HIGH) {
      myServo.write(100); //tourner le servo pour bloquer la catapulte
      delay(1000); //attendre 1 seconde après avoir laché le bouton
    }
    else {
      myServo.write(50); //revenir a la position initiale
    }
  }
}