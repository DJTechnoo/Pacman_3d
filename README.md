	+-----------+
	|HOW TO PLAY|
	+-----------+

Move player: WASD
Move Camera: IJKL and mouse

	+--------+
	|GAMEPLAY|
	+--------+
Player starts at startingposition
Ghosts move around.
Ghosts' AI: random direction upon collision.
Player loses one life upon ghost collision.
Player increases one score upon food collision.
Player wins when all food is collected.
Player dies when 3 lives are lost.

	+------------------+
	|HOW GAME IS SET UP|
	+------------------+
The map and food is loaded from "level0.txt" where
0 indicates food, and 1 indicates wall. 2 (only one)
is the initial player position.

	+------------------------+
	|HOW TO BUILD PROJECT    |
	+------------------------+

The way we shared the project is to just 
share the source code, includes and assets.
We initially used lab_01 as our starting point with 
cMake. Then overwrite the "src", "includes", and "Assets"
folders with the folders provided from this repo.
in the project solution, delete any duplications.
This project also uses GLUT for the menu window.
Please link it to the project before attempting to run the code.
In short:

1) cMake Lab_01
2) drag the folders to overwrite files.
3) delete duplicated stuff.
4) link GLUT



	+-------------+
	|CONSTRIBUTORS|
	+-------------+
Askel Eirik Johansson (473215)
Herman Tandberg Dybing
Sigbjørn Skar