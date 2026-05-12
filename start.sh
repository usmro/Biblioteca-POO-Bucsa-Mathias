#!/bin/bash
echo "Pornire server backend..."
cd ~/Biblioteca---POO/backend
./api &

echo "Pornire frontend React..."
cd ~/Biblioteca---POO/frontend
npm start
