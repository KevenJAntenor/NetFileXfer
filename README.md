# INF3173-232 TP2

Le but de ce TP est de transférer des fichiers par réseau. Le composant client lit le fichier spécifié et l'envoit au serveur. Le serveur reçoit le fichier et l'enregistre dans un fichier de destination.

La gestion des connexions est déjà réalisée pour vous. Vous avez directement accès à un descripteur de fichier sur lequel faire des écriture pour envoyer des données et des lecture pour recevoir.

Pour pouvoir transférer des fichiers de taille arbitraire, deux éléments sont nécessaire. Le premier est d'utiliser mmap() pour lire le fichier dans le client et écrire le fichier dans le serveur. Si le fichier n'entre pas en mémoire, alors le système d'exploitation libérera des pages automatiquement. Le deuxième élément concerne l'envoi et la réception des données, qui doivent se faire par bloc. La taille des blocs est un paramètre de la fonction que vous devez compléter.

Le code à compléter est dans le fichier `src/xfer.c`. Il contient deux fonctions, soit `xfer_send_file()` et `xfer_recv_file()`. Les commentaires indiquent la démarche générale pour réaliser ces deux fonctions. Vous devez utiliser `mmap()` pour lire et écrire le fichier, et le descripteur `sockfd` pour envoyer et recevoir le fichier sur le socket.

Pour tester, vous avez accès au programme `test_xfer`. Le test principal transfert un fichier et vérifie que celui reçu est identique pour différentes combinaison de taille de bloc et de fichiers. On vérifie également s'il y a une fuite de descripteurs de descripteur de fichiers.

Un banc d'essai est founi. Il mesure la vitesse de transfert en fonction de la taille de bloc. Vous devez l'exécuter avant de faire votre archive pour la remise avec la commande `make runbench`. Comment la taille de bloc influence la vitesse et pourquoi? Quel est la taille de bloc la plus avantageuse?

Votre travail sera corrigé sur le système [Hopper](https://hopper.info.uqam.ca). Le système vous permet de former votre équipe pour ce TP. Faire l'archive avec `make dist`, puis envoyer l'archive sur le serveur de correction. Votre note sera attribuée automatiquement. Vous pouvez soumettre votre travail plusieurs fois et la meilleure note sera conservée. D'autres détails et instructions pour l'utilisation de ce système seront fournis.

Barème de correction

 * Transfert correct du fichier: 50 points
 * Descripteurs fermés correctement: 20 points
 * Fonction de progression: 10 points
 * Validité du banc d'essai: 20 points
 * Qualité du code (fuite mémoire, gestion d'erreur, etc): pénalité jusqu'à 10 points
 * Total sur 100 points

Bon travail!
