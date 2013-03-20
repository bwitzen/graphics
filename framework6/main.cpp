/* Computer Graphics and Game Technology, Assignment Box2D game
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
 */

#include <cstdio>
#include <GL/gl.h>
#include <GL/glut.h>
#include <Box2D/Box2D.h>
#include "levels.h"

#define TIMESTEP    1.0 / 60.0
#define PI_VAL      3.1415

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;


b2Vec2 gravity(0.0f, -10.0f);
b2World world(gravity);
b2Body* body;
b2Body* stupidpolygon;
b2Body* groundBody;
b2Body* circleBody;
//b2CircleShape circle;
b2PolygonShape polygon;
//b2PolygonShape polygon;

/*
 * Function that can draw a circle. Hooray!
 */

void draw_circle(float x, float y, float r) {
  // begin line segment
  glBegin(GL_LINE_LOOP);
  float rad;  
  // draw circle
  for (int i = 0; i < 360; i++) {
    rad = i * PI_VAL / 180.0;
    glVertex2f(cos(rad)*r + x, sin(rad)*r + y);
  }
  // end
  glEnd();
}


/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */
void load_world(unsigned int level)
{
  if (level >= num_levels) {
    printf("Warning: level %d does not exist.\n", level);
    return;
  }

  //
  // THIS PIECE OF CODE CREATES THE GROUND
  //

  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  groundBody = world.CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);

  //
  // THIS PIECE OF CODE CREATES THE BALL
  //

  // create ball
  b2BodyDef ball;
  ball.type = b2_dynamicBody;
  ball.position.Set(levels[1].start.x, levels[1].start.y);
  body = world.CreateBody(&ball);
  b2Vec2 position = body->GetPosition();
  // create a shape
  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(.075f, .075f);
  // create a fixture
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = .3f;
  // apply fixture to body
  body->CreateFixture(&fixtureDef);

  //
  // THIS PIECE OF CODE CREATES THE POLYGONS
  // 

  b2BodyDef whatever;
  groundBodyDef.position.Set(0.0f, 0.0f);
  b2Vec2 vertices[3];
  vertices[0].Set(1.0f, 4.0f);
  vertices[1].Set(1.0f, 1.0f);
  vertices[2].Set(6.0f, 1.0f);
  int32 count = 3;
  polygon.Set(vertices, count);
  stupidpolygon = world.CreateBody(&whatever);

  fixtureDef.shape = &polygon;
  fixtureDef.density = 1.0f;
  stupidpolygon->CreateFixture(&fixtureDef);
  stupidpolygon->ResetMassData();


}


/*
 * Called when we should redraw the scene (i.e. every frame).
 * It will show the current framerate in the window title.
 */
void draw(void) {
  int time = glutGet(GLUT_ELAPSED_TIME);
  int frametime = time - last_time;
  frame_count++;

  // Clear the buffer
  glColor3f(0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // cause the world to step
  world.Step(TIMESTEP, 6, 2);
  b2Vec2 position = body->GetPosition();
  float32 angle = body->GetAngle();
  // print ball position to terminal
  printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);

  // prepare openGL for use
  glLineWidth(3.0);
  glColor3f(1.0, 0.0, 0.0);
  // call our own draw function to make that circle visible!
  draw_circle(position.x, position.y, .075f);
  // draw that fooking polygon
  //glBegin(GL_TRIANGLES);
  //glVertex3f(1.0f, 4.0f, 0);
  //glVertex3f(1.0f, 1.0f, 0);
  //glVertex3f(6.0f, 1.0f, 0);
  // end
  //glEnd();  

  // Show rendered frame
  glutSwapBuffers();

  // Display fps in window title.
  if (frametime >= 1000) {
    char window_title[128];
    snprintf(window_title, 128,
            "Box2D: %f fps, level %d/%d",
            frame_count / (frametime / 1000.f), -1, num_levels);
    glutSetWindowTitle(window_title);
    last_time = time;
    frame_count = 0;
  }
}

/*
 * Called when window is resized. We inform OpenGL about this, and save this
 * for future reference.
 */
void resize_window(int width, int height)
{
    glViewport(0, 0, width, height);
    reso_x = width;
    reso_y = height;
}

/*
 * Called when the user presses a key.
 */
void key_pressed(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // Esc
        case 'q':
            exit(0);
            break;
        // Add any keys you want to use, either for debugging or gameplay.
        default:
            break;
    }
}

/*
 * Called when the user clicked (or released) a mouse buttons inside the window.
 */
void mouse_clicked(int button, int state, int x, int y)
{

}

/*
 * Called when the mouse is moved to a certain given position.
 */
void mouse_moved(int x, int y)
{

}


int main(int argc, char **argv)
{
    // Create an OpenGL context and a GLUT window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(reso_x, reso_y);
    glutCreateWindow("Box2D");

    // Bind all GLUT events do callback function.
    glutDisplayFunc(&draw);
    glutIdleFunc(&draw);
    glutReshapeFunc(&resize_window);
    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_clicked);
    glutMotionFunc(&mouse_moved);
    glutPassiveMotionFunc(&mouse_moved);

    // Initialise the matrices so we have an orthogonal world with the same size
    // as the levels, and no other transformations.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, world_x, 0, world_y, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Read the levels into a bunch of structs.
    num_levels = load_levels(&levels);
    printf("Loaded %d levels.\n", num_levels);

    // Load the first level (i.e. create all Box2D stuff).
    load_world(0);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    frame_count = 0;
    glutMainLoop();

    return 0;
}
