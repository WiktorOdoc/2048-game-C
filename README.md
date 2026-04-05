# 2048-game-C
2048 game with GUI and gamestate saving in C

a 4x4 grid, every turn a 2 (occasionally 4) appears in one block  
You can press arrows to slide all the blocks to one side, numbers that are the same merge and add. It is also added to score.  
The goal is to obtain a block with number 2048 (You can continue further if you want)  
If all the blocks fill up and you can't make any moves you lose.  
Like this: https://play2048.co/  
SDL2 library for GUI  

TO RUN REQUIRES SDL2.dll and SDL2_ttf.dll (it is zipped due to file size) in the same directore as .exe  
In addition a "resources" folder is required and the font file "Consolas.ttf".  
Just like in the bin/debug directory.  
