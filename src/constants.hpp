#pragma once

enum Rotation { NORTH, EAST, SOUTH, WEST };

// In Pixel domain:
#define TILE_SIZE 32

// In Tile domain (multiply with Tile Size to get pixel values):
#define BASE_ENTITY_HEIGHT 2
#define WINDOW_WIDTH 440
#define WINDOW_HEIGHT 280
#define PIXEL_SIZE 3
#define TILESET_COLUMNS 11 // TODO: Read from actual tileset data

#define COMPONENT_TYPES Positionable

#define WALK_SPEED 48 // = PIXELS PER SECOND
#define FPS 120

// Animation
#define ANIMATION_UPDATE_RATE_IN_FRAMES (5 / WALK_SPEED)
#define ANIMATION_UPDATE_RATE_IN_SECONDS 0.075 //(WALK_SPEED)
#define PLAYER_STANDING_ANIMATION_NUMBER 1
#define PLAYER_NUMBER_ANIMATIONS 4

// Asset File Addresses
// Data
#define SAVEFILE_PATH "savefile.json"                       // not used right now
#define WORLD_DEFINITION_PATH "../assets/default_game.json" // not used right now
#define WEAPONDATA_PATH "../assets/weapon_data.csv"
// Graphics
#define SPRITE_SHEET "../assets/jungle_full.png"
#define HERO_SHEET "../assets/hero-spritesheet.png"
#define TILE_PROPERTIES "../assets/tile_properties.csv"
#define MAINMENU_BACKGROUND "../assets/mainmenu_background.png"
#define M4A1 "../assets/weapons/m4a1.png"
// Audio
#define SFX_COLLECT_ITEM "../assets/audio/sfx/item_collect.wav"
#define SFX_REMOVE_ITEM "../assets/audio/sfx/pop.wav"
#define SFX_FOOTSTEP "../assets/audio/sfx/grass_footstep.wav"
#define SFX_SNIPER_SHOT_AND_RELOAD "../assets/audio/sfx/sniper_shot_and_reload.wav"
#define SFX_AK_SHOT_FULL_AUTO_LONG "../assets/audio/sfx/ak_shot_full-auto_long.wav"
#define BACKGROUND_JUNGLE_AMBIENCE "../assets/audio/music/background_jungle_ambience.mp3"
#define BACKGROUND_MAIN_MENU "../assets/audio/music/mainmenu_background_lttz.mp3"
// Behavior trees
#define BT_DIRECTORY "../assets/ai/trees"
// Fonts
#define FONT_ARIAL "../assets/fonts/Arial.ttf"

// menu constants in pixel
#define PADDING 5
#define FONT_SIZE 30

// Entity IDs
#define PLAYER 0
