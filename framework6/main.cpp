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

// color stuff
float color_fi = 0.0;
float color_r = 1, color_g = 1, color_b = 1;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
unsigned int current_level;
level_t *levels;

b2World* world;
b2Body* body;
b2Body* finishBody;




/*
 * Function that uses openGL to draw a circle to the screen. Define the midpoint
 * with X and Y, the radius with R, and the color with CR, CG, CB.
 */

void draw_circle(float x, float y, float r, float cr, float cg, float cb) {

  glColor3f(cr, cg, cb);
  glBegin(GL_LINE_LOOP);
  float rad;
  for (int i = 0; i < 360; i++) {
    rad = i * PI_VAL / 180.0;
    glVertex2f(cos(rad)*r + x, sin(rad)*r + y);
  }
  glEnd();
  
}


/*
 * Function that uses openGL to draw a polygon to the screen. Pass an array of
 * floats containing 3-element vectors. Also pass the amount of such vectors
 * contained in ARRAY as per AMT. Pass a color with CR, CG, CB.
 */

void draw_polygon(float *array, int amt, float cr, float cg, float cb) {

  glColor3f(cr, cg, cb);
  glBegin(GL_POLYGON);
  for (int i = 0; i < amt * 3; i += 3) {
    glVertex3f(array[i], array[i+1], array[i+2]);
  }
  glEnd();

}

/*
 * Function adds a polygon to the world. Supports 3 to 8 vertices.
 */
 
void box2d_polygon(float *array, int amt) {

  b2PolygonShape shape;
  b2BodyDef bodyDef;
  b2Body *polygonBody;
  b2FixtureDef fixtureDef;
  
  b2Vec2 *vertices = new b2Vec2[amt];
  for (int i = 0; i < amt * 2; i += 2)
    vertices[i/2].Set(array[i], array[i+1]);
  
  shape.Set(vertices, amt);
  polygonBody = world->CreateBody(&bodyDef);
  fixtureDef.shape = &shape;
  fixtureDef.density = 1.0f;
  polygonBody->CreateFixture(&fixtureDef);
  polygonBody->ResetMassData();

  delete[] vertices;
}


/*
 * Function calculates the distance between two points and returns it.
 */
 
float distance_two_points(float x1, float y1, float x2, float y2) {
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */
 
void load_world(unsigned int lvl) {
  
  // ensure level is valid
  if (lvl >= num_levels) {
    printf("Warning: level %d does not exist.\n", lvl);
    return;
  }
  
  // cast level variable to global
  current_level = lvl;
  
  //
  // INITIALIZE OPENGL
  //

  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(2.0);
  
  //
  // CREATE WORLD
  //
  
  b2Vec2 gravity(0.0f, -10.0f);
  world = new b2World(gravity);

  //
  // CREATE GROUND
  //
  
  b2Body* groundBody;
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  groundBody = world->CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);

  //
  // CREATE THE BALL
  //

  // create ball
  b2Body* circleBody;
  b2BodyDef ball;
  ball.type = b2_dynamicBody;
  ball.position.Set(levels[lvl].start.x, levels[lvl].start.y);
  body = world->CreateBody(&ball);
  b2Vec2 position = body->GetPosition();
  // create a shape
  b2CircleShape dynamicBox;
  //dynamicBox.SetAsBox(.075f, .075f);
  dynamicBox.m_radius = .075f;
  // create a fixture
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = .3f;
  // apply fixture to body
  body->CreateFixture(&fixtureDef);
  
  //
  // CREATE THE FINISH
  //
  
  
  float array[8] = { levels[lvl].end.x - .025, levels[lvl].end.y - .025,
                     levels[lvl].end.x + .025, levels[lvl].end.y - .025,
                     levels[lvl].end.x + .025, levels[lvl].end.y + .025,
                     levels[lvl].end.x - .025, levels[lvl].end.y + .025 };
  box2d_polygon(array, 4);  

  //
  // CREATE THE POLYGONS
  // 
  
  for (int i = 0; i < levels[lvl].num_polygons; i++) {
    float *array = new float[levels[lvl].polygons[i].num_verts * 2];  
    for (int j = 0; j < levels[lvl].polygons[i].num_verts * 2; j += 2) {
      array[j]   = levels[lvl].polygons[i].verts[j/2].x;
      array[j+1] = levels[lvl].polygons[i].verts[j/2].y;
    }
    box2d_polygon(array, levels[lvl].polygons[i].num_verts);
    delete[] array;
  }

}


/*
 * Called when we should redraw the scene (i.e. every frame).
 * It will show the current framerate in the window title.
 */
 
void draw(void) {
  int time = glutGet(GLUT_ELAPSED_TIME);
  int frametime = time - last_time;
  frame_count++;
  
  // manage fade in at level start
  if (color_fi < 1.0)
    color_fi += 0.01;

  // cause the world to step
  world->Step(TIMESTEP, 6, 2);
    
  // draw ball
  b2Vec2 position = body->GetPosition();
  float32 angle = body->GetAngle();
  draw_circle(position.x, position.y, .075f, color_r * color_fi, .0f, .0f);
  
  // check wincondition
  if (distance_two_points(position.x, position.y, levels[current_level].end.x, levels[current_level].end.y) < 0.125f) {
    load_world(current_level+1);
  }
  
  // draw polygons
  int amt;
  for (int i = 0; i < levels[current_level].num_polygons; i++) {
    amt = levels[current_level].polygons[i].num_verts;
    int cur = 0;
    float *array = new float[amt * 3];
    for (int j = 0; j < amt * 3; j += 3) {
      array[j]   = levels[current_level].polygons[i].verts[cur].x;
      array[j+1] = levels[current_level].polygons[i].verts[cur].y;
      array[j+2] = .0f;
      cur++;
    }
    draw_polygon(array, amt, .0f, color_g * color_fi, .0f);
  }
  
  // draw finish
  float finish[12] = { levels[current_level].end.x - .025f, levels[current_level].end.y - .025f, .0f,
                       levels[current_level].end.x + .025f, levels[current_level].end.y - .025f, .0f,
                       levels[current_level].end.x + .025f, levels[current_level].end.y + .025f, .0f,
                       levels[current_level].end.x - .025f, levels[current_level].end.y + .025f, .0f };
  draw_polygon(finish, 4, .0f, .0f, color_b * color_fi);
  
  // Show rendered frame
  glutSwapBuffers();

  // Display fps in window title.
  if (frametime >= 1000) {
    char window_title[128];
    snprintf(window_title, 128,
            "Box2D: %f fps, level %d/%d",
            frame_count / (frametime / 1000.f), current_level + 1, num_levels);
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
