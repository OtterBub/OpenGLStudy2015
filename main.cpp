#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <gl\glut.h>

void drawStrokeText( char *string, float x, float y, float z, float scale ){
	char *c;
	glPushMatrix();
	{
		glTranslatef( x, y, z );
		glColor3f( 1.0, 1.0, 1.0 );
		glScalef( scale, scale, scale );
		for( c = string; *c != '\0'; c++ )
			glutStrokeCharacter( GLUT_STROKE_ROMAN, *c );
	}
	glPopMatrix();
}

struct Vector3
{
	float x, y, z;
};

struct Object
{
	Vector3 Pos;
	Vector3 Dir;
	float Speed;
	float Size;
	bool Enable;
};

bool CollisionCheck2DBox( Vector3 pos1, float size1, Vector3 pos2, float size2 )
{
	float halfsize1 = size1 / 2.f;
	float halfsize2 = size2 / 2.f;

	if( ( pos1.x - halfsize1 ) <= ( pos2.x + halfsize2 ) &&
		( pos1.x + halfsize1 ) >= ( pos2.x - halfsize2 ) &&

		( pos1.z - halfsize1 ) <= ( pos2.z + halfsize2 ) &&
		( pos1.z + halfsize1 ) >= ( pos2.z - halfsize2 ) )
		return true;

	return false;
}

bool CollisionCheck2DBox( Object obj1, Object obj2 )
{
	return CollisionCheck2DBox( obj1.Pos, obj1.Size, obj2.Pos, obj2.Size );
}

bool CollisionCheck2DBox( Vector3 pos1, float size1, Object obj2 )
{
	return CollisionCheck2DBox( pos1, size1, obj2.Pos, obj2.Size );
}

GLvoid Display( GLvoid );
GLvoid Reshape( int w, int h );
GLvoid KeyBoard( unsigned char key, int x, int y );
GLvoid KeyBoardUp( unsigned char key, int x, int y );
GLvoid Mouse( int button, int state, int x, int y );
GLvoid MouseMotion( int x, int y );
GLvoid Timer( int val );
GLvoid GlobalInit();

bool g_keyState[0xff] = { 0, };
Vector3 g_myPosition;

#define MAXENEMY 50
#define MAXBULLET 50

Object g_Enemy[MAXENEMY];
Object g_Bullet[MAXBULLET];

int main()
{
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "OpenGL Study" );
	glutDisplayFunc( Display );
	glutReshapeFunc( Reshape );
	glutKeyboardFunc( KeyBoard );
	glutKeyboardUpFunc( KeyBoardUp );
	glutMouseFunc( Mouse );
	glutMotionFunc( MouseMotion );

	glutTimerFunc( 3, Timer, 0 );
	srand( (unsigned int)time( NULL ) );

	GlobalInit();

	glutMainLoop();

	return 0;
}

GLvoid GlobalInit()
{
	g_myPosition.y = 10.5;
}

GLvoid Display( GLvoid )
{
	glClearColor( 0.1, 0.8, 0.9, 1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity();
	gluLookAt( g_myPosition.x / 2.f, 100, 150, g_myPosition.x / 2.f, 0, 0, 0, 1, 0 );

	glEnable( GL_DEPTH_TEST );

	// Floor
	float floorWidth = 150;
	glColor4f( 0.8, 0.8, 0.8, 1.0 );
	glBegin( GL_QUADS );
	glVertex3f( floorWidth, 0, -floorWidth );
	glVertex3f( -floorWidth, 0, -floorWidth );
	glVertex3f( -floorWidth, 0, floorWidth );
	glVertex3f( floorWidth, 0, floorWidth );
	glEnd();

	// Player
	glPushMatrix();
	{
		glTranslatef( g_myPosition.x, g_myPosition.y, g_myPosition.z );
		glColor4f( 1, 1, 1, 1 );
		glutSolidCube( 10 );
		glColor4f( 0, 0, 0, 0 );
		glutWireCube( 10.1 );

		float shadowHalf = 10 / 2.f;
		glColor4f( 0, 0, 0, 1.0 );
		glBegin( GL_QUADS );
		glVertex3f( shadowHalf, -g_myPosition.y + 0.5, -shadowHalf );
		glVertex3f( -shadowHalf, -g_myPosition.y + 0.5, -shadowHalf );
		glVertex3f( -shadowHalf, -g_myPosition.y + 0.5, shadowHalf );
		glVertex3f( shadowHalf, -g_myPosition.y + 0.5, shadowHalf );
		glEnd();
	}
	glPopMatrix();

	// Bullet
	for( int i = 0; i < MAXBULLET; ++i )
	{
		if( g_Bullet[i].Enable )
		{
			glPushMatrix();
			{
				glTranslatef( g_Bullet[i].Pos.x, g_Bullet[i].Pos.y, g_Bullet[i].Pos.z );
				glColor4f( 1, 1, 0, 1 );
				glutSolidCube( g_Bullet[i].Size );

				float shadowHalf = g_Bullet[i].Size / 2.f;
				glColor4f( 0, 0, 0, 1.0 );
				glBegin( GL_QUADS );
				glVertex3f( shadowHalf, -g_Bullet[i].Pos.y + 0.5, -shadowHalf );
				glVertex3f( -shadowHalf, -g_Bullet[i].Pos.y + 0.5, -shadowHalf );
				glVertex3f( -shadowHalf, -g_Bullet[i].Pos.y + 0.5, shadowHalf );
				glVertex3f( shadowHalf, -g_Bullet[i].Pos.y + 0.5, shadowHalf );
				glEnd();
			}
			glPopMatrix();
		}
	}

	// Enemy
	for( int i = 0; i < MAXENEMY; ++i )
	{
		if( g_Enemy[i].Enable )
		{
			glPushMatrix();
			{
				glTranslatef( g_Enemy[i].Pos.x, g_Enemy[i].Pos.y, g_Enemy[i].Pos.z );
				glColor4f( 1, 0, 0, 1 );
				glutSolidCube( g_Enemy[i].Size );

				float shadowHalf = g_Enemy[i].Size / 2.f;
				glColor4f( 0, 0, 0, 1.0 );
				glBegin( GL_QUADS );
				glVertex3f( shadowHalf, -g_Enemy[i].Pos.y + 0.5, -shadowHalf );
				glVertex3f( -shadowHalf, -g_Enemy[i].Pos.y + 0.5, -shadowHalf );
				glVertex3f( -shadowHalf, -g_Enemy[i].Pos.y + 0.5, shadowHalf );
				glVertex3f( shadowHalf, -g_Enemy[i].Pos.y + 0.5, shadowHalf );
				glEnd();
			}
			glPopMatrix();
		}
	}

	glutSwapBuffers();
}
GLvoid Reshape( int w, int h )
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60, (float)w / (float)h, 1.0, 10000000.0 );
	glMatrixMode( GL_MODELVIEW );
}
GLvoid KeyBoard( unsigned char key, int x, int y )
{
	g_keyState[key] = true;
}
GLvoid KeyBoardUp( unsigned char key, int x, int y )
{
	g_keyState[key] = false;
}
GLvoid Mouse( int button, int state, int x, int y )
{
}
GLvoid MouseMotion( int x, int y )
{
}
GLvoid Timer( int val )
{
	static float deltaTime = 0;
	static float currentTime = clock();

	deltaTime = ( clock() - currentTime ) / 1000.f;
	printf( "%f\n", deltaTime );
	currentTime = clock();

	static float cubeSpeed = 100;

	if( g_keyState['w'] )
		g_myPosition.z -= cubeSpeed * deltaTime;
	if( g_keyState['s'] )
		g_myPosition.z += cubeSpeed * deltaTime;

	if( g_keyState['a'] )
		g_myPosition.x -= cubeSpeed * deltaTime;
	if( g_keyState['d'] )
		g_myPosition.x += cubeSpeed * deltaTime;

	// Bullet Create
	static int bulletIndex = 0;
	static float bulletTime = 0.1;
	static float currentBulletTime = bulletTime;

	currentBulletTime += deltaTime;
	if( currentBulletTime > bulletTime )
	{
		if( g_keyState['j'] )
		{
			currentBulletTime = 0;

			g_Bullet[bulletIndex].Pos = g_myPosition;
			g_Bullet[bulletIndex].Dir.z = -1;
			g_Bullet[bulletIndex].Speed = 200;

			g_Bullet[bulletIndex].Size = 2.5;

			g_Bullet[bulletIndex].Enable = true;

			bulletIndex = ( bulletIndex + 1 ) % MAXBULLET;
		}
	}

	// Enemy Create
	static int enemyIndex = 0;
	static float enemyTime = 0.25;
	static float currentEnemyTime = enemyTime;

	currentEnemyTime += deltaTime;
	if( currentEnemyTime > enemyTime )
	{
		currentEnemyTime = 0;

		g_Enemy[enemyIndex].Pos.z = -300;
		g_Enemy[enemyIndex].Pos.y = g_myPosition.y;
		g_Enemy[enemyIndex].Pos.x = ( rand() % 300 ) - 150;

		g_Enemy[enemyIndex].Dir.z = 1;
		g_Enemy[enemyIndex].Speed = 100;

		g_Enemy[enemyIndex].Size = 5;

		g_Enemy[enemyIndex].Enable = true;

		enemyIndex = ( enemyIndex + 1 ) % MAXENEMY;

	}

	// GAME UPDATE

	for( int i = 0; i < MAXBULLET; ++i )
	{
		if( g_Bullet[i].Enable )
		{
			g_Bullet[i].Pos.x += g_Bullet[i].Dir.x * ( g_Bullet[i].Speed * deltaTime );
			g_Bullet[i].Pos.y += g_Bullet[i].Dir.y * ( g_Bullet[i].Speed * deltaTime );
			g_Bullet[i].Pos.z += g_Bullet[i].Dir.z * ( g_Bullet[i].Speed * deltaTime );

			if( g_Bullet[i].Pos.z <= -350 )
				g_Bullet[i].Enable = false;
		}
	}

	for( int i = 0; i < MAXENEMY; ++i )
	{
		if( g_Enemy[i].Enable )
		{
			g_Enemy[i].Pos.x += g_Enemy[i].Dir.x * ( g_Enemy[i].Speed * deltaTime );
			g_Enemy[i].Pos.y += g_Enemy[i].Dir.y * ( g_Enemy[i].Speed * deltaTime );
			g_Enemy[i].Pos.z += g_Enemy[i].Dir.z * ( g_Enemy[i].Speed * deltaTime );

			for( int j = 0; j < MAXBULLET; ++j )
			{
				if( g_Bullet[j].Enable )
				{
					if( CollisionCheck2DBox( g_Bullet[j], g_Enemy[i] ) )
					{
						g_Enemy[i].Enable = false;
						break;
					}
				}
			}
		}
	}

	glutTimerFunc( 3, Timer, 0 );
	glutPostRedisplay();
}