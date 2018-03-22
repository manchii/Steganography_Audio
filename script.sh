#!/bin/bash

for song in songs/1.wav songs/2.wav songs/input2.wav songs/SunInTheSky.wav
do
  for amp in 10 20 30 40 50
  do
    for delay in 100 1000 2300 5000
    do
      for window in 1000 5000 10000 15000
      do
        echo $song
        echo $amp
        echo $delay
        echo $window
        bin/encoder --set-song $song --set-a-one $amp --set-a-zero $amp --set-t-one $delay --set-t-zero $(expr $((delay * 3 / 2))) --set-window-size $window >> resultados.txt
        bin/decoder --set-a-one $amp --set-a-zero $amp --set-t-one $delay --set-t-zero $(expr $((delay * 3 / 2))) --set-window-size $window >> resultados.txt
        bin/error >> resultados.txt
        bin/peaqb -r $song -t encoded_song.wav | tail -10 >> resultados.txt
      done
    done
  done
done
