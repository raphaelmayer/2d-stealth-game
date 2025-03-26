#pragma once

enum Rotation{NORTH, EAST, SOUTH, WEST }; 

// In Pixel domain:
#define TILE_SIZE 16
#define PLAYER_SIZE_X 16
#define PLAYER_SIZE_Y 24

// In Tile domain (multiply with Tile Size to get pixel values):
#define MAP_SIZE_X 90
#define MAP_SIZE_Y 30
#define WINDOW_WIDTH 220
#define WINDOW_HEIGHT 140
#define PIXEL_SIZE 3

#define LEVEL_1_EAST_BORDER 40
#define LEVEL_2_WEST_BORDER 43

#define COMPONENT_TYPES Positionable

// SpriteSheet addresses
#define PLAYER_SPRITE_SHEET_Y_WHITE 720
#define PLAYER_SPRITE_SHEET_Y_BLUE 808
#define PLAYER_SPRITE_SHEET_Y_RED 896

#define WALK_SPEED 32 // = PIXELS PER SECOND
#define FPS 120

// Animation
#define ANIMATION_UPDATE_RATE_IN_FRAMES (5 / WALK_SPEED)
#define ANIMATION_UPDATE_RATE_IN_SECONDS 0.075 //(WALK_SPEED)
#define PLAYER_STANDING_ANIMATION_NUMBER 1
#define PLAYER_NUMBER_ANIMATIONS 4



// Asset File Addresses
// Data
#define SAVEFILE_PATH "savefile.json"
#define WORLD_DEFINITION_PATH "../assets/default_game.json"
#define WEAPONDATA_PATH "../assets/weapon_data.csv"
// Graphics
#define SPRITE_SHEET "../assets/overworld_spritesheet.png"
#define HERO_SHEET "../assets/hero-spritesheet.png"
#define MAP_CHAR_MAPPING "../assets/overworld_char_mapping.csv"
#define MAP_BACKGROUND "../assets/overworld_map_background.txt"
#define MAP_OBJECTS "../assets/overworld_map_objects.txt"
#define MAINMENU_BACKGROUND "../assets/mainmenu_background.png"
#define M4A1 "../assets/weapons/m4a1.png"
// Audio
#define SFX_COLLECT_ITEM "../assets/audio/sfx/item_collect.wav"
#define SFX_REMOVE_ITEM "../assets/audio/sfx/pop.wav"
#define SFX_FOOTSTEP "../assets/audio/sfx/grass_footstep.wav"
#define SFX_SNIPER_SHOT_AND_RELOAD "../assets/audio/sfx/sniper_shot_and_reload.wav"
#define BACKGROUND_MUSIC_1 "../assets/audio/music/background_1.mp3"
#define BACKGROUND_MUSIC_2 "../assets/audio/music/background_2.mp3"
// Behavior trees
#define BT_DIRECTORY "../assets/ai/trees"
//Fonts
#define FONT_ARIAL "../assets/fonts/Arial.ttf"

// menu constants in pixel
#define PADDING 5
#define FONT_SIZE 30

// Progress
#define PROGRESS_STEPS 13

// Entity IDs
#define PLAYER 0
