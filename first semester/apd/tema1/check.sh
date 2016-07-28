#!/bin/bash

echo "" 
echo "Panarama.pnm res_factor 2\n" 
./homework in/panarama.pnm out/panarama_my.pnm 2 1 
./homework in/panarama.pnm out/panarama_my.pnm 2 2 
./homework in/panarama.pnm out/panarama_my.pnm 2 4 
./homework in/panarama.pnm out/panarama_my.pnm 2 8 
./homework in/panarama.pnm out/panarama_my.pnm 2 16 
echo "\n" 

echo "Panarama.pnm res_factor 3\n" 
./homework in/panarama.pnm out/panarama_my.pnm 3 1 
./homework in/panarama.pnm out/panarama_my.pnm 3 2 
./homework in/panarama.pnm out/panarama_my.pnm 3 4 
./homework in/panarama.pnm out/panarama_my.pnm 3 8 
./homework in/panarama.pnm out/panarama_my.pnm 3 16 
echo "\n" 


echo "Panarama.pnm res_factor 8\n" 
./homework in/panarama.pnm out/panarama_my.pnm 8 1 
./homework in/panarama.pnm out/panarama_my.pnm 8 2 
./homework in/panarama.pnm out/panarama_my.pnm 8 4 
./homework in/panarama.pnm out/panarama_my.pnm 8 8 
./homework in/panarama.pnm out/panarama_my.pnm 8 16 
echo "\n" 

#===============================================================
!#
echo " " 
echo "Panarama.pgm res_factor 2\n" 
./homework in/panarama.pgm out/panarama_my.pgm 2 1 
./homework in/panarama.pgm out/panarama_my.pgm 2 2 
./homework in/panarama.pgm out/panarama_my.pgm 2 4 
./homework in/panarama.pgm out/panarama_my.pgm 2 8 
./homework in/panarama.pgm out/panarama_my.pgm 2 16 
echo "\n" 

echo "Panarama.pgm res_factor 3\n" 
./homework in/panarama.pgm out/panarama_my.pgm 3 1 
./homework in/panarama.pgm out/panarama_my.pgm 3 2 
./homework in/panarama.pgm out/panarama_my.pgm 3 4 
./homework in/panarama.pgm out/panarama_my.pgm 3 8 
./homework in/panarama.pgm out/panarama_my.pgm 3 16 
echo "\n" 


echo "Panarama.pgm res_factor 8\n" 
./homework in/panarama.pgm out/panarama_my.pgm 8 1 
./homework in/panarama.pgm out/panarama_my.pgm 8 2 
./homework in/panarama.pgm out/panarama_my.pgm 8 4 
./homework in/panarama.pgm out/panarama_my.pgm 8 8 
./homework in/panarama.pgm out/panarama_my.pgm 8 16 
echo "\n" 

#=================================================================
echo "Render resolution=100" 
./homework1 out/render.pgm 100 1 
./homework1 out/render.pgm 100 2 
./homework1 out/render.pgm 100 4 
./homework1 out/render.pgm 100 8 
./homework1 out/render.pgm 100 16 
echo "\n" 

echo "Render resolution=10000" 
./homework1 out/render.pgm 10000 1 
./homework1 out/render.pgm 10000 2 
./homework1 out/render.pgm 10000 4 
./homework1 out/render.pgm 10000 8 
./homework1 out/render.pgm 10000 16 
echo "\n" 