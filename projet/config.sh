#!/bin/bash

# Vérifier si les paquets nécessaires sont déjà installés
if dpkg -l | grep -q libgtk-3-dev && \
   dpkg -l | grep -q libglib2.0-dev && \
   dpkg -l | grep -q libpango1.0-dev && \
   dpkg -l | grep -q libcairo2-dev && \
   dpkg -l | grep -q libatk-bridge2.0-dev; then
    echo "Les bibliothèques GTK et leurs dépendances sont déjà installées."
else
    # Mettre à jour les informations du gestionnaire de paquets
    sudo apt update
    
    # Installer les bibliothèques et leurs dépendances
    sudo apt install -y libgtk-3-dev libglib2.0-dev libpango1.0-dev libcairo2-dev libatk-bridge2.0-dev

    # Vérifier si l'installation a réussi
    if dpkg -l | grep -q libgtk-3-dev && \
       dpkg -l | grep -q libglib2.0-dev && \
       dpkg -l | grep -q libpango1.0-dev && \
       dpkg -l | grep -q libcairo2-dev && \
       dpkg -l | grep -q libatk-bridge2.0-dev; then
        echo "Les bibliothèques GTK et leurs dépendances ont été installées avec succès."
    else
        echo "Erreur : Impossible d'installer les bibliothèques GTK et leurs dépendances. Veuillez installer manuellement."
    fi
fi
