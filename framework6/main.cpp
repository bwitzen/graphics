/* *****************************************************************************
 *
 * Computer graphics and game design
 * assignment 6
 *
 * ************************************************************************** */

#include <cstdio>
#include <GL/gl.h>
#include <GL/glut.h>
#include <Box2D/Box2D.h>
#include <unistd.h>
#include "levels.h"

#define TIMESTEP    1.0 / 60.0
#define PI_VAL      3.1415
#define PLAYER_MAX  8
#define PLAYER_VERT 4
#define GRAVITY     -.1f

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;

// Global variables to make our lifes easier.
int lvl;
b2World* world;
b2Body* ball;

// Player drawing 'buffers'
float x_draw[PLAYER_VERT], y_draw[PLAYER_VERT], z_draw[PLAYER_VERT];
int mouse_clicks;

// Polygon linked-list struct to make drawing easier.
typedef struct polydraw {
  float x[8], y[8], z[8];
  int amt_vertices;
  polydraw *next;
} polydraw;

polydraw * first;


int music_player () {
        pid_t x;      // a special kind of int
        char kil[20] = "kill -s 9 ";
 
        x = fork();   /* now there's actually two "x"s:
if fork succeeds, "x" to the CHILD PROCESS is the return value of fork (0)
and "x" to the PARENT PROCESS is the actual system pid of the child process.*/
         
        if (x < 0) {  // just in case fork fails
                puts("fork failure");
                exit(-1);
        }  
        else if (x == 0) { // therefore this block will be the child process
                execlp("mpg123", "mpg123", "-q", "/songs/bertha.mp3", 0);
        }                   // see GNU docs, "system" also works               
        else {  printf("from parent: mpg123 is pid %d\nENTER to quit\n", x);
                sprintf(kil,"%s%d",kil,x);
                getchar();  // wait for user input
                system(kil);
                printf("All ");
        }       /* witness that the "else if" and "else" blocks are both executed here in parallel. The "else"
   (parent) block is continuous with the rest of the program (since the PARENT PROCESS is actually
   the program itself) so... */
        printf("done.\n");
        exit(0);
}

/*
 * Function adds a polygon to the world. Does no error handling, but does
 * bounce if AMT is invalid. It also adds a polydraw to our linked list!
 */
 
void box2d_polygon(float *array, int amt) {

  // bounce if invalid amount
  if (amt < 3 || amt > 8)
    return;

  // helper variables
  b2PolygonShape shape;
  b2BodyDef bodyDef;
  b2Body *polygonBody;
  b2FixtureDef fixtureDef;
  
  // define vertices
  b2Vec2 *vertices = new b2Vec2[amt];
  for (int i = 0; i < amt * 3; i += 3)
    vertices[i/3].Set(array[i], array[i+1]);
  
  // define properties
  shape.Set(vertices, amt);
  polygonBody = world->CreateBody(&bodyDef);
  fixtureDef.shape = &shape;
  fixtureDef.density = 1.0f;
  polygonBody->CreateFixture(&fixtureDef);
  polygonBody->ResetMassData();

  // delete allocated help resources
  delete[] vertices;

  // define polydraw
  polydraw *handle = first;

  if (handle != NULL) {
    while (handle->next != NULL)
      handle = handle->next;

    handle->next = new polydraw;
    for (int i = 0; i < amt * 3; i += 3) {
      handle->next->x[i/3] = array[i];
      handle->next->y[i/3] = array[i+1];
      handle->next->z[i/3] = .0f;
    }
    printf("\n");
    handle->next->amt_vertices = amt;
    handle->next->next = NULL;
  }
  else {
    first = new polydraw;
    for (int i = 0; i < amt * 3; i += 3) {
      first->x[i/3] = array[i];
      first->y[i/3] = array[i+1];
      first->z[i/3] = .0f;
    }
    printf("\n");
    first->amt_vertices = amt;
    first->next = NULL;
  }

}


/*
 * Load a given world, i.e. read the world from the `levels' data structure and
 * convert it into a Box2D world.
 */

void load_world(int level) {

  // ENSURE LEVEL BEING LOADED IS VALID
  if (level >= num_levels) {
    printf("Warning: level %d does not exist.\n", level);
    return;
  }
  
  // INITIALIZE OPENGL

  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(2.0);
  
  // CREATE WORLD
  
  b2Vec2 gravity(0.0f, GRAVITY);
  world = new b2World(gravity);
  lvl = level; // global
  first = NULL; // fixme, this is rediculous
  mouse_clicks = 0;

  // CREATE GROUND
  
  b2Body* groundBody;
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);
  groundBody = world->CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);
  groundBody->CreateFixture(&groundBox, 0.0f);

  // CREATE THE BALL

  // create/define body
  b2BodyDef ballDef;
  ballDef.type = b2_dynamicBody;
  ballDef.position.Set(levels[level].start.x, levels[level].start.y);
  ball = world->CreateBody(&ballDef);
  b2Vec2 position = ball->GetPosition();
  // attach a shape
  b2CircleShape dynamicBox;
  dynamicBox.m_radius = .075f;
  // generate a fixture
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = .3f;
  // apply fixture to body
  ball->CreateFixture(&fixtureDef);
  
  // CREATE THE FINISH  
  
  float array[12] = { levels[level].end.x - .025, levels[level].end.y - .025, .0f,
                     levels[level].end.x + .025, levels[level].end.y - .025, .0f,
                     levels[level].end.x + .025, levels[level].end.y + .025, .0f,
                     levels[level].end.x - .025, levels[level].end.y + .025, .0f };
  box2d_polygon(array, 4);

  // CREATE THE POLYGONS
  
  for (unsigned int i = 0; i < levels[level].num_polygons; i++) {
    float *array2 = new float[levels[level].polygons[i].num_verts * 3];  
    for (unsigned int j = 0; j < levels[level].polygons[i].num_verts * 3; j += 3) {
      array2[j]   = levels[level].polygons[i].verts[j/3].x;
      array2[j+1] = levels[level].polygons[i].verts[j/3].y;
      array2[j+2] = .0f;
    }
    box2d_polygon(array2, levels[level].polygons[i].num_verts);
    delete[] array2;
  }

}


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
 * Function calculates and returns the distance between the points (x1,y1) and
 * (x2, y2).
 */

float distance_two_points(float x1, float y1, float x2, float y2) {
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}


/*
 * Function that uses openGL to draw a polygon to the screen. Pass an array of
 * floats containing 3-element vectors. Also pass the amount of such vectors
 * contained in ARRAY as per AMT. Pass a color with CR, CG, CB.
 */

void draw_polygon(float *x, float *y, float *z, int amt, 
                  float cr, float cg, float cb) {
  glColor3f(cr, cg, cb);
  glBegin(GL_POLYGON);
  for (int i = 0; i < amt; i++) {
    glVertex3f(x[i], y[i], z[i]);
  }
  glEnd();

}


void internal_list_polys(void) {
  polydraw *handle = first;
  while (handle != NULL) {
    printf("Polygon X (%p) data :: amt = %d :: next = %p :: verts = \n", handle, handle->amt_vertices, handle->next);
    for (int j = 0; j < 8; j++) {
      printf("%f %f %f \n", handle->x[j], handle->y[j], handle->z[j]);
    }
    handle = handle->next;
    printf("\n\n\n");
  }
}

/*
 * Called when we should redraw the scene (i.e. every frame).
 * It will show the current framerate in the window title.
 */

void draw(void) {

  glClear(GL_COLOR_BUFFER_BIT);

  //internal_list_polys();

  // SET ELAPSED TIME AND KEEP TRACK OF FRAME LOGIC
  int time = glutGet(GLUT_ELAPSED_TIME);
  int frametime = time - last_time;
  frame_count++;

  // CAUSE WORLD TO STEP AND GET BALL POSITION
  world->Step(TIMESTEP, 6, 2);
  b2Vec2 p = ball->GetPosition();

  // LOAD NEXT LEVEL IF WON
  if (distance_two_points(p.x, p.y, levels[lvl].end.x, levels[lvl].end.y) < 0.125f) {
    load_world(lvl + 1);
  }
    
  // DRAW BALL  
  draw_circle(p.x, p.y, .075f, 1.f, .0f, .0f);

  // DRAW POLYGONS
  polydraw *handle = first;
  while (handle != NULL) {
    draw_polygon(handle->x, handle->y, handle->z, handle->amt_vertices, .0f, 1.f, .0f);
    handle = handle->next;
  }

  // RENDER FRAME
  glutSwapBuffers();

  // SHOW FRAMERATE
  if (frametime >= 1000) {
    char window_title[128];
    snprintf(window_title, 128,
      "Box2D: %f fps, level %d/%d",
      frame_count / (frametime / 1000.f), lvl, num_levels);
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
 *
 */

int find_smallest_index(float *array, int amt) {
  int smallest = 0;
  for (int i = 1; i < amt; i++)
    if (array[i] < array[smallest])
      smallest = i;
  return smallest;
}

int find_largest_index(float *array, int amt) {
  int largest = 0;
  for (int i = 1; i < amt; i++)
    if (array[i] > array[largest])
      largest = i;
  return largest;
}

/*
 * Called when the user clicked (or released) a mouse buttons inside the window.
 */
void mouse_clicked(int button, int state, int x, int y) {

  // fix twisted coordinates
  float x_fixed = (float)x / reso_x * 8.f;
  float y_fixed = (float)abs(y - reso_y) / reso_y * 6.f;

  if (state) {
    // record mouse_clicks
    x_draw[mouse_clicks] = x_fixed;
    y_draw[mouse_clicks] = y_fixed;
    z_draw[mouse_clicks] = 0;

    // add mouse clicks
    mouse_clicks ++;

    // if sufficient mouse clicks, add polygon
    if (mouse_clicks == PLAYER_VERT) {

      int swap;
      float array[PLAYER_VERT * 3];

      // rearrange counterclockwise
      // first smallest x, then smallest y, then largest x, then largest y
      swap = find_smallest_index(x_draw, PLAYER_VERT);
      array[0] = x_draw[swap];
      array[1] = y_draw[swap];
      array[2] = z_draw[swap];
      swap = find_smallest_index(y_draw, PLAYER_VERT);
      array[3] = x_draw[swap];
      array[4] = y_draw[swap];
      array[5] = z_draw[swap];
      swap = find_largest_index(x_draw, PLAYER_VERT);
      array[6] = x_draw[swap];
      array[7] = y_draw[swap];
      array[8] = z_draw[swap];
      swap = find_largest_index(y_draw, PLAYER_VERT);
      array[9] = x_draw[swap];
      array[10] = y_draw[swap];
      array[11] = z_draw[swap];

      box2d_polygon(array, PLAYER_VERT);
      mouse_clicks = 0; 
    }
  }

}

/*
 * Called when the mouse is moved to a certain given position.
 */
void mouse_moved(int x, int y)
{

}


int main(int argc, char **argv)
{

// To disable music - comment this out
  pid_t x = fork();
  char * str = "kill -s 9 ";  

  if (x == 0) {
    execlp("mpg123", "mpg123", "-q", "sticker.mp3", 0);
  }               
  else {
    // To disable music - untill this

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

// To disable music - and
    sprintf(str, "%s%d", str, x);
    system(str);
    printf("All");
    exit(0);
// To disable music - untill this
  }
}
