/*
 * uart.c
 *
 *  Created on: Oct 10, 2023
 *      Author: colin
 */
#include "shell.h"
#include "main.h"
#include "motor.h"
#include "adc_mes.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

// définition des messages transmis par le shell
const uint8_t help[] = "Liste des commandes\r\nhelp : donne la liste des commandes\r\npinout : donne la liste des broches connectées\r\nstart : allume le moteur\r\nstop : eteint le moteur\r\n"
		"speed : modifie la vitesse\r\ncourant : affiche le courant en mA\r\nvitesse : affiche la vitesse du moteur";
const uint8_t pinout[] = "PA8 : U\t\tPB13 : U_barre\r\nPA9 : V\t\tPB14 : V_barre\r\nBus_Imes : PC2 (mesure du courant)\r\nEncoder a : PA6\tEncoder b : PA4 (mesure de la vitesse)";
const uint8_t powerOn[] = "Powering on the motor";
const uint8_t powerOff[] = "Shutting down the motor";
const uint8_t speedChanged[] = "Changing speed";
const uint8_t cmdNotFound[] = "Command not found";
const uint8_t started[] = "Bienvenue dans ce super shell !";
const uint8_t courant[] = "Courant : ";
const uint8_t courant_txt_ma[] = " mA";
const uint8_t vitesse_txt[] = " tour/min";
const uint8_t newLine[] = "\r\n>>>";
// ____________________________________________
// Definition des variables
int idxCmd = 0; // permet de savoir ou ajouter un caractère dans la commande
char cmd[CMD_BUFFER_SIZE]; // contient la commande
char buffer_cmd[1]; // dernier charactère entré
char vitesse_str[5]; // valeur de la vitesse en texte
char courant_str[5]; // valeur du courant en texte

// Fonctions
char* UART_Create_Cmd(void){ // Création et reconnaissance de la commande entrée dans le shell
	if (buffer_cmd[0] == '\r'){ // Retour à la ligne reconnu, reconnaissance de la commande
		if (!(strncmp(cmd,"help",4))){
			HAL_UART_Transmit(&huart2, help, 264, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"pinout",6))){
			HAL_UART_Transmit(&huart2, pinout, 140, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"start",5))){
			Start_Motor(cmd); // lancement du moteur
			HAL_UART_Transmit(&huart2, powerOn, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"stop",4))){
			Stop_Motor(); // arret du moteur
			HAL_UART_Transmit(&huart2, powerOff, 23, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"speed",5))){
			HAL_UART_Transmit(&huart2, speedChanged, 14, HAL_MAX_DELAY);
			Change_Speed(cmd); // modification de la vitesse
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"courant",7))){
			int32_t courant_mA = Mes_Courant();
			sprintf(courant_str, "%ld", courant_mA); // affichage du courant
			HAL_UART_Transmit(&huart2, courant, 10, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, courant_str, 5, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, courant_txt_ma, 3, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}else if(!(strncmp(cmd,"vitesse",7))){
			int32_t vitesse = Mesure_Vitesse();
			sprintf(vitesse_str, "%ld", vitesse); // affichage de la vitesse
			HAL_UART_Transmit(&huart2, vitesse_str, strlen(vitesse_str), HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, vitesse_txt, 9, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}
		else{ // erreur si la commande n'est pas reconnue
			HAL_UART_Transmit(&huart2, cmdNotFound, 17, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, newLine, 5, HAL_MAX_DELAY);
		}
		idxCmd = 0;
	}else{ // si le caractère n'est pas \r, on ajoute le caractère à la commande en cours de création
		cmd[idxCmd] = buffer_cmd[0];
		idxCmd += 1;
		if (idxCmd > CMD_BUFFER_SIZE){ // gestion de l'erreur si la commande est trop longue
			idxCmd = 0;
		}
	}
	return cmd;
}

void UART_Echo(void){ // affichage dans le shell du caractère tapé au clavier
	if (buffer_cmd[0] == '\r'){
	  HAL_UART_Transmit(&huart2, newLine, 2, HAL_MAX_DELAY);
	}else{
	  HAL_UART_Transmit(&huart2, buffer_cmd, 1, HAL_MAX_DELAY);
	}
	HAL_UART_Receive_IT(&huart2, buffer_cmd, 1);
}
