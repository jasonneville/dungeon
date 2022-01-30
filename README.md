
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/github_username/repo_name">
  </a>

<h3 align="center">Dungeon Game</h3>

  <p align="center">
    Roguelike Dungeon Game in the terminal
    <br />

  </p>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project

This is a roguelike dungeon game that I created in Spring 2019 in C++. It is turn based and features many different types of monsters and objects that you can interact with. Floors are marked with the '.' character, stairs are marked with '<' or '>', and corridors are marked with '#'. The player character is marked with '@'. The player can move by using numbers 1-9 on their keyboard. 

<!-- USAGE EXAMPLES -->
## Usage

The game can be run by running the following commands in the directory with dungeon.cpp
```
$   make
$   ./dungeon --load --save --nummon *NUMBER HERE*
```
--load means that the game will try to load the previously saved dungeon from the file system. This argument is optional.
<br/>
--save means that the game will save the dungeon to the file system after generation. This argument is optional.
<br/>
--nummon specifies the amount of monsters present in the game. This argument is optional. The game will default to 10 if not specified. 

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* Jeremy Schaefer (I.S.U) for heap and part of pathfinding algorithm 


