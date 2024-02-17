#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <ctime>
#include <sstream>


using namespace sf;
using namespace std;


void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];


enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];


int main()
{

	//VideoMode

	VideoMode vm(1366, 768);
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);


	//Declarations

	bool paused = true;
	int score = 0;
	Text messageText;
	Text scoreText;
	Font font;
	Clock clock;
	Time gameTimeTotal;
	RectangleShape timeBar;


	//Textures

	Texture textureBackground;
	Texture textureTree;
	Texture textureBee;
	Texture textureCloud;
	Texture textureBranch;
	Texture texturePlayer;
	Texture textureRip;
	Texture textureAxe;
	Texture textureLog;



	//Sprites

	Sprite spriteBackground;
	Sprite spriteTree;
	Sprite spriteBee;
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	Sprite spritePlayer;
	Sprite spriteRip;
	Sprite spriteAxe;
	Sprite spriteLog;


	//LoadFromFiles

	textureBackground.loadFromFile("graphics/background.png");
	textureTree.loadFromFile("graphics/tree.png");
	textureBee.loadFromFile("graphics/bee.png");
	textureCloud.loadFromFile("graphics/cloud.png");
	font.loadFromFile("fonts/myfont.ttf");
	textureBranch.loadFromFile("graphics/branch.png");
	texturePlayer.loadFromFile("graphics/player.png");
	textureRip.loadFromFile("graphics/rip.png");
	textureAxe.loadFromFile("graphics/axe.png");
	textureLog.loadFromFile("graphics/log.png");


	//background sprite

	spriteBackground.setTexture(textureBackground);
	spriteBackground.setPosition(0, 0);


	//tree sprite

	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(565, 0);


	//bee sprite

	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(-100, 480);
	bool beeActive = false;
	float beeSpeed = 0.0f;


	//cloud sprite

	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);
	spriteCloud1.setPosition(-300, 0);
	spriteCloud2.setPosition(-300, 150);
	spriteCloud3.setPosition(-300, 300);
	bool cloud1Active = false;
	bool cloud2Active = false;
	bool cloud3Active = false;
	float cloud1Speed = 0;
	float cloud2Speed = 0;
	float cloud3Speed = 0;


	//player sprite

	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(405, 480);

	side playerSide = side::RIGHT;

	//rip sprite

	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(405, 1500);


	//axe sprite

	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(490, 558);


	//log sprite
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(565, 554);
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;
	
	//Text customization
	
	messageText.setFont(font);
	scoreText.setFont(font);
	messageText.setString("Press space to start!");
	scoreText.setString("Score = 0");
	messageText.setCharacterSize(100);
	scoreText.setCharacterSize(60);
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);


	//Text positions

	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
						  textRect.width / 2.0f,
						  textRect.top +
						  textRect.height / 2.0f);
	messageText.setPosition(1366 / 2.0f, 768 / 2.0f);
	scoreText.setPosition(20, 20);


	//Time bar

	float timeBarStartWidth = 500;
	float timeBarHeight = 60;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1366.0 / 2) - timeBarStartWidth / 2, 700);

	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;


	//branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 14);
	}


	bool acceptInput = false;


	// Prepare the sounds
	// The player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);
	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/deathSound.mp3");
	Sound death;
	death.setBuffer(deathBuffer);
	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);


	while (window.isOpen()) {

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				// Listen for key presses again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			paused = false;

			score = 0;
			timeRemaining = 6;
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}
			// Make sure the gravestone is hidden
			spriteRip.setPosition(675, 2000);
			// Move the player into position
			spritePlayer.setPosition(405, 480);
			acceptInput = true;
		}

		
		if (acceptInput)
		{
			if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2.0 / score) + .15;
				spriteAxe.setPosition(727, 555);
				spritePlayer.setPosition(820, 480);
				// Update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(565, 550);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}

			if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
			{
				// Make sure the player is on the left
				playerSide = side::LEFT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2.0 / score) + .15;
				spriteAxe.setPosition(490, 558);
				spritePlayer.setPosition(405, 480);
				// update the branches
				updateBranches(score);
				// set the log flying
				spriteLog.setPosition(565, 550);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				chop.play();
			}
			
		}
		/*
		****************************************
		Update the scene
		****************************************
		*/

			Time dt = clock.restart(); //deltatime


			if (!paused) 
			{

				// Subtract from the amount of time remaining
				timeRemaining -= dt.asSeconds();
				// size up the time bar
				timeBar.setSize(Vector2f(timeBarWidthPerSecond *
								timeRemaining, timeBarHeight));

				if (timeRemaining <= 0.0f) {

					paused = true;

					messageText.setString("Out of time!!");

					FloatRect textRect = messageText.getLocalBounds();
					messageText.setOrigin(textRect.left +
						                  textRect.width / 2.0f,
									  	  textRect.top +
										  textRect.height / 2.0f);
					messageText.setPosition(1366 / 2.0f, 768 / 2.0f);
					outOfTime.play();
				}


				if (!beeActive)
				{
					// Velocity
					srand((int)time(0));
					beeSpeed = (rand() % 200) + 200;
					// Y-coor
					srand((int)time(0) * 10);
					float height = (rand() % 300) + 200;
					spriteBee.setPosition(1500, height);
					beeActive = true;
				}
				else
				{                          /*                                 Coordinates                                   */
					// Moving             /*                           X                        */ /*           Y          */
					spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

					if (spriteBee.getPosition().x < -400)  // Last pos
					{
						beeActive = false;
					}
				}


				if (!cloud1Active)
				{
					// Velocity
					srand((int)time(0) * 10);
					cloud1Speed = (rand() % 200);
					// Y-coor
					srand((int)time(0) * 10);
					float height = (rand() % 100);
					spriteCloud1.setPosition(-250, height);
					cloud1Active = true;
				}
				else
				{                             /*                                      Coordinates                                      */
					//Moving                 /*                             X                          */ /*             Y            */
					spriteCloud1.setPosition(spriteCloud1.getPosition().x + cloud1Speed * dt.asSeconds(), spriteCloud1.getPosition().y);

					if (spriteCloud1.getPosition().x > 1600)  //Last pos
					{
						cloud1Active = false;
					}

				}

				if (!cloud2Active)
				{
					// Velocity
					srand((int)time(0) * 20);
					cloud2Speed = (rand() % 200);
					// Y-coor
					srand((int)time(0) * 20);
					float height = (rand() % 200) - 100;
					spriteCloud2.setPosition(-230, height);
					cloud2Active = true;
				}
				else
				{                             /*                                      Coordinates                                      */
					//Moving                 /*                             X                          */ /*             Y            */
					spriteCloud2.setPosition(spriteCloud2.getPosition().x + cloud2Speed * dt.asSeconds(), spriteCloud2.getPosition().y);

					if (spriteCloud2.getPosition().x > 1600)  //Last pos
					{
						cloud2Active = false;
					}

				}

				if (!cloud3Active)
				{
					// Velocity
					srand((int)time(0) * 30);
					cloud3Speed = (rand() % 200);
					// Y-coor
					srand((int)time(0) * 30);
					float height = (rand() % 300) - 100;
					spriteCloud3.setPosition(-260, height);
					cloud3Active = true;
				}
				else
				{                             /*                                      Coordinates                                      */
					//Moving                 /*                             X                          */ /*             Y            */
					spriteCloud3.setPosition(spriteCloud3.getPosition().x + cloud3Speed * dt.asSeconds(), spriteCloud3.getPosition().y);

					if (spriteCloud3.getPosition().x > 1600)  //Last pos
					{
						cloud3Active = false;
					}

				}
			}


			//Text
			stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			if (!paused) {
				// update the branch sprites
				for (int i = 0; i < NUM_BRANCHES; i++)
				{
					float height = (i * 106.5);
					if (branchPositions[i] == side::LEFT)
					{
						// Move the sprite to the left side
						branches[i].setPosition(345, height);
						// Flip the sprite round the other way
						branches[i].setRotation(180);
					}
					else if (branchPositions[i] == side::RIGHT)
					{
						// Move the sprite to the right side
						branches[i].setPosition(998, height);
						// Set the sprite rotation to normal
						branches[i].setRotation(0);
					}
					else
					{
						// Hide the branch
						branches[i].setPosition(3000, height);
					}
				}
			}

			
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));
				// Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 1500)
				{
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(565, 554);					
				}
			}

			if (branchPositions[5] == playerSide)
			{
				// death
				paused = true;
				acceptInput = false;
				

				if (playerSide == side::LEFT)
				{
					spriteRip.setPosition(405, 540);
				}
				else
				{
					spriteRip.setPosition(815, 540);
				}
				
				// hide the player
				spritePlayer.setPosition(2000, 1000);
				spriteAxe.setPosition(2000, 1000);
				messageText.setString("SQUISHED!!");
				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1366 / 2.0f,
					768 / 2.0f);
				death.play();
			}



		/*
		****************************************
		Draw the scene
		****************************************
		*/
		// Clear everything from the last frame
		window.clear();
		// Draw our game scene here
		window.draw(spriteBackground);
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);
		window.draw(spriteBee);
		window.draw(spriteTree);
		window.draw(spritePlayer);
		window.draw(spriteRip);
		window.draw(spriteLog);
		window.draw(spriteAxe);
		window.draw(scoreText);

		


		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		if (!paused) {
			window.draw(timeBar);
		}

		if (paused) {
			window.draw(messageText);

		}

		
		// Show everything we just drew
		window.display();
	}

	return 0;
}


void updateBranches(int seed) 
{
	for (int i = NUM_BRANCHES - 1; i > 0; i--)
	{
		branchPositions[i] = branchPositions[i - 1];
	}
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
		case 0:
			branchPositions[0] = side::LEFT;
			break;
		case 1:
			branchPositions[0] = side::RIGHT;
			break;
		default:
			branchPositions[0] = side::NONE;
			break;
	}
}