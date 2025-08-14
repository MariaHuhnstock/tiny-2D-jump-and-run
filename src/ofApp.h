#pragma once

#include "ofMain.h"
#include <vector>

int screen_width = 600;
int screen_height = 400;

float circle_x;
float circle_y;
float circle_radius;
ofColor background_col;
ofColor ground_col;
ofColor circle_col;
ofVec2f circle_movement;
float gravity;
vector<ofRectangle> obstacles;

// Position und Eigenschaften des Spielers
float player_x;
float player_y;
float player_radius;
ofColor player_col;

// Steuerung des Spielers
bool player_moving_left;
bool player_moving_right;

ofVec2f player_movement; // Bewegung des Spielers (x und y)
float player_gravity;     // Schwerkraft für Spieler
bool player_on_ground;    // Steht der Spieler auf dem Boden?
bool player_jumping;      // Verhindert mehrfaches Springen

bool player_hits_circle;

ofSoundPlayer jumpSound; // Variable für Springsound

ofImage playerImage; 

// rote Kugeln
struct Collectible 
{
	float x, y;
	float radius;
	bool collected;
};

vector<Collectible> collectibles;
int collected_count;

// Zeitlimit
float startTime;
float timeLimit = 30.0f; // 30 Sekunden
bool gameOver;



class ofApp : public ofBaseApp 
{
public:
	void setup();
	void update();
	void draw();

	vector<ofRectangle> obstacles; // Dynamische Liste von Rechtecken
	int screen_width, screen_height;

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	bool checkPlayerCollision(const ofRectangle& rect);
};
