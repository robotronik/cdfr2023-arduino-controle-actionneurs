# CDFR 2023 Controle actionneurs

## Description



## Table des commandes

| commande | parmètres | commentaires |
| --- | --- | --- |
| !servo7:x\n | x = position en degrès (rentré = 180, déployé = 0)| étagère du haut |
| !servo6:x\n | x = position en degrès (rentré = 180, déployé = 0)| étagère du milieu |
| !servo5:x\n | x = position en degrès (rentré = 180, déployé = 0) | étagère du bas |
| !servo4:x\n | x = position en degrès (ouverte = 0, fermé = 140) | postion de la pince |
| !servo3:x\n |  | = position en degrès (ouverte = 0, fermé = 140)| position pince droit (mirroir)
| !servo2:x\n |  | Non assigné |
| !servo1:x\n |  | Non assigné |
| !stepper1:x\n |  x = postion en nb de steps (bas = 0, haut = 750) | Ascenseur |
| !LedUI1:x\n |  x = allumé/étinte (allumé = 1, allumé = 0) | led sur le dessus du robot, (led 1 celle de devant et led 4 celle de derrière) |
| !LedUI2:x\n |  x = allumé/étinte (allumé = 1, allumé = 0) | led sur le dessus du robot, (led 1 celle de devant et led 4 celle de derrière) |
| !LedUI3:x\n |  x = allumé/étinte (allumé = 1, allumé = 0) | led sur le dessus du robot, (led 1 celle de devant et led 4 celle de derrière) |
| !LedUI4:x\n |  x = allumé/étinte (allumé = 1, allumé = 0) | led sur le dessus du robot, (led 1 celle de devant et led 4 celle de derrière) |
| !BoutonUI1:\n |  retourne boutonUI1:1\r\n lors d'un appuye sinon 0 | bouton monostable du devant sur le dessus du robot |
| !BoutonUI2:\n |  retourne boutonUI2:1\r\n lors d'un appuye sinon 0 | bouton monostable du derrière sur le dessus du robot |
| !BoutonUI3:\n |  retourne boutonUI3:1\r\n ou 0 | interrupteur bistables du devant sur le dessus du robot |
| !BoutonUI4:\n |  retourne boutonUI4:1\r\n ou 0 | interrupteur bistables du devant sur le dessus du robot |
| !Bouton1:\n |  retourne bouton1:1\n\t quand l'aimant est positionné | aimant de démarage du robot |
| !Bouton2:\n |  | non assigné |
| !Bouton3:\n |  | non assigné |
| !Bouton4:\n |  | non assigné |
| !moteur1:x\n | | non assigné : permet d'avoir une sortie 12V de puissance |
| !I2CSend:c...\n | Permet d'envoyer une commande à la carte d'asservissement | exemple !I2CSend:21,0,500,90\n commande = 21 et le reste sont les paramètre de la commande|
| !I2CRequest:x\n | Nombre de bit que l'on souhaite recevoir lors de la prochaine commande | exemple !I2CRequest:6\n (2 octets pour x, 2 pour y, 2 pour teta) !I2CSend:20\n commande = 20 pour un get postion. Renvoie : I2C:x,y,teta\r\n|


## Installation


## Author

This program was created by Guillaume DALLE. (guillaume.dalle@grenoble-inp.org)
