#include "ofApp.h"
#include <vector>

void ofApp::setup()
{
	// Player initialisieren
	player_x = 100;
	player_y = 100;
	player_radius = 20;
	player_col.set(30, 210, 76, 255); // Grün
	playerImage.load("player.png");

	// Bewegungen initial auf false
	player_moving_left = false;
	player_moving_right = false;

	// Gravitation & Bewegung für Player
	player_movement.set(0, 0);
	player_gravity = 0.2f;
	player_on_ground = false;
	player_jumping = false;
	
	// Kreisposition und Radius
	circle_x = 20; 
	circle_y = 50; 
	circle_radius = 10;

	// Die Hintergrundfarbe lautet:
	// Rot Grün Blau Alpha (Transparenz)
	// 20 100 200 255
	background_col.set(20, 100, 200, 255);

	// Dasselbe für die Farbe des Kreises:
	circle_col.set(200, 200, 255);

	// Wir legen die Hintergrundfarbe fest.
	ofSetBackgroundColor(background_col);

	screen_width = ofGetWidth();
	screen_height = ofGetHeight();

	ofRectangle new_rect;

	// Boden
	new_rect.set(5, screen_height - 10, screen_width - 10, 10);
	obstacles.push_back(new_rect);

	// Plattform 1
	new_rect.set(200, screen_height - 60, 200, screen_height - 20);
	obstacles.push_back(new_rect);

	// Bewegung & Gravitation
	circle_movement.set(1, 0);
	gravity = 0.1f;

	player_hits_circle = false;

	// Jumpsound
	jumpSound.load("jump.wav"); // Sound wird geladen
	jumpSound.setMultiPlay(true); // erlaubt mehrfaches Abspielen

	for (int i = 0; i < 5; i++) 
	{
		Collectible c;
		c.x = ofRandom(50, screen_width - 50);
		c.y = ofRandom(-200, -50); // startet über dem Bildschirm
		c.radius = 10;
		c.collected = false;
		collectibles.push_back(c);
	}
	collected_count = 0;

	startTime = ofGetElapsedTimef();
	gameOver = false;
}

void ofApp::update()
{
	// Schwerkraft anwenden
	circle_movement.y += gravity;

	// Kreis Bewegung nach unten
	circle_x += circle_movement.x;
	circle_y += circle_movement.y;

	// Kollision mit allen Hindernissen (Plattformen) prüfen
	for (auto& rect : obstacles)
	{
		// Prüfe, ob der Kreis auf der Plattform "landet"
		bool überDerPlattform = circle_y + circle_radius >= rect.getY();
		bool unterDerOberkante = circle_y + circle_radius <= rect.getY() + rect.getHeight();
		bool innerhalbDerBreite = circle_x >= rect.getX() && circle_x <= rect.getX() + rect.getWidth();

		// Wenn der Kreis auf der Plattform landet und sich nach unten bewegt (y > 0)
		if (überDerPlattform && unterDerOberkante && innerhalbDerBreite && circle_movement.y >= 0)
		{
			// Die Y-Position des Kreises so anpassen, dass er auf der Plattform bleibt
			circle_y = rect.getY() - circle_radius;

			// Richtung der vertikalen Bewegung umkehren (abprallen)
			circle_movement.y = -circle_movement.y;
		}
	}

	// Kreis prallt von Wänden ab
	if (circle_x <= circle_radius)
	{
		circle_x = circle_radius; // Korrigiere Position
		circle_movement.x = -circle_movement.x;
	}
	else if (circle_x >= screen_width - circle_radius)
	{
		circle_x = screen_width - circle_radius; // Korrigiere Position
		circle_movement.x = -circle_movement.x;
	}

	for (int n = 0; n < obstacles.size(); n++) 
	{
		ofDrawRectangle(obstacles[n]);
	}

	// Spieler bewegen, wenn Taste gedrückt
	if (player_moving_right) 
	{
		player_x += 1; // Nach rechts bewegen
	}
	else if (player_moving_left) 
	{
		player_x -= 1; // Nach links bewegen
	}

	// Gravitation auf den Spieler anwenden
	player_movement.y += player_gravity;
	player_on_ground = false;

	// Spieler-Bewegung anwenden
	player_y += player_movement.y;

	// Prüfen ob Spieler mit einer Plattform kollidiert
	for (auto& rect : obstacles)
	{
		if (checkPlayerCollision(rect))
		{
			// Kollision von oben?
			if (player_movement.y >= 0 && player_y + player_radius <= rect.getY() + 10)
			{
				player_y = rect.getY() - player_radius;
				player_movement.y = 0;
				player_on_ground = true;
				player_jumping = false;
			}
		}
	}

	// Verhindern, dass der Spieler durch den Boden fällt
	if (player_y + player_radius > screen_height)
	{
		player_y = screen_height - player_radius;
		player_movement.y = 0;
		player_on_ground = true;
		player_jumping = false;
	}

	for (auto& c : collectibles) {
		if (!c.collected) {
			c.y += 0.5f;

			// Kollisionsprüfung mit Spieler
			float dist = ofDist(player_x, player_y, c.x, c.y);
			if (dist < player_radius + c.radius) {
				c.collected = true;
				collected_count++;
			}

			// Kollision mit Plattformen – dann verschwinden
			for (auto& rect : obstacles) {
				if (c.y + c.radius >= rect.getY() &&
					c.x >= rect.getX() && c.x <= rect.getX() + rect.getWidth()) {
					c.collected = true;
				}
			}
		}
	}
	if (!gameOver && ofGetElapsedTimef() - startTime > timeLimit) 
	{
		gameOver = true;
	}
}

void ofApp::draw()
{
	// Kreis und Farbe
	ofSetColor(circle_col);
	ofDrawCircle(circle_x, circle_y, circle_radius);

	// Boden und Farbe
	ofSetColor(ground_col);
	for (auto& rect : obstacles)
	{
		ofDrawRectangle(rect);
	}

	// Text anzeigen je nach Kollision
	ofSetColor(255);
	if (player_hits_circle)
	{
		ofDrawBitmapString("Ouch!", 10, 20);
	}
	else
	{
		ofDrawBitmapString("Alles gut!", 10, 20);
	}

	// Spieler zeichnen
	if (playerImage.isAllocated())
	{
		playerImage.draw(player_x - playerImage.getWidth() / 2, player_y - playerImage.getHeight() / 2);
	}
	else
	{
		ofSetColor(player_col);
		ofDrawCircle(player_x, player_y, player_radius);
	}

	// Kollision Spieler vs. Kreis
	float dx = player_x - circle_x;
	float dy = player_y - circle_y;
	float distance = sqrt(dx * dx + dy * dy);

	if (distance < player_radius + circle_radius)
	{
		player_hits_circle = true;
	}
	else
	{
		player_hits_circle = false;
	}

	for (auto& c : collectibles) 
	{
		if (!c.collected) {
			ofSetColor(255, 0, 0);
			ofDrawCircle(c.x, c.y, c.radius);
		}
	}
	ofSetColor(255);
	ofDrawBitmapString("Gesammelt: " + ofToString(collected_count), 10, 40);

	if (gameOver) 
	{
		ofSetColor(255, 255, 0);
		ofDrawBitmapString("Zeit vorbei! Punkte: " + ofToString(collected_count), 10, 60);
	}
}

void ofApp::keyPressed(int key)
{
	if (key == 'a') 
	{
		player_moving_left = true;
	}
	else if (key == 'd') 
	{
		player_moving_right = true;
	}
	else if (key == 'w' || key == ' ')
	{
		if (player_on_ground && !player_jumping)
		{
			player_movement.y = -5.5f; // Sprunghöhe
			player_jumping = true;
			player_on_ground = false;
			jumpSound.play();
		}
	}
}

void ofApp::keyReleased(int key)
{
	if (key == 'a') 
	{
		player_moving_left = false;
	}
	else if (key == 'd') 
	{
		player_moving_right = false;
	}
}

void ofApp::mouseMoved(int x, int y)
{

}

void ofApp::mouseDragged(int x, int y, int button)
{

}

void ofApp::mousePressed(int x, int y, int button)
{

}

void ofApp::mouseReleased(int x, int y, int button)
{

}

void ofApp::mouseEntered(int x, int y)
{

}

void ofApp::mouseExited(int x, int y)
{

}

void ofApp::windowResized(int w, int h)
{

}

void ofApp::gotMessage(ofMessage msg)
{

}

void ofApp::dragEvent(ofDragInfo dragInfo)
{
	
}

bool ofApp::checkPlayerCollision(const ofRectangle& rect)
{
	float player_bottom = player_y + player_radius;
	float player_top = player_y - player_radius;
	float player_left = player_x - player_radius;
	float player_right = player_x + player_radius;

	float rect_bottom = rect.getY() + rect.getHeight();
	float rect_top = rect.getY();
	float rect_left = rect.getX();
	float rect_right = rect.getX() + rect.getWidth();

	bool collision =
		player_bottom >= rect_top &&
		player_top <= rect_bottom &&
		player_right >= rect_left &&
		player_left <= rect_right;

	return collision;
}