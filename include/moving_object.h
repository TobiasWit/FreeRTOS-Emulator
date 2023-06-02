
#include "gfx_draw.h"


extern TaskHandle_t MovingObjectsDisplay;
extern TaskHandle_t CheckInputTaskStateOne;


/**
 * @brief Object to represent a moving circle
*/
typedef struct circle_moving {
    signed short x;     /** X coordinate of circles center*/
    signed short y;     /** Y coordinate of circles center*/
    signed short radius;    /** Radius of the circle*/
    unsigned int colour;    /** Colour of the circle*/
} circle_moving_t;

/**
 * @brief Object to represent a moving rectangle
*/
typedef struct rect_moving {
    signed short x;         /** X coordinate of top left point*/ 
    signed short y;         /** Y coordinate of top left point*/
    signed short w;         /** Width of rectangle*/
    signed short h;         /** Height of rectangle*/
    unsigned int colour;    /** Colour of rectangle*/
} rect_moving_t;

/**
 * @brief Object to represent a moving text
*/
typedef struct text_moving {
    signed short x;         /** X coordinate of top left point*/
    signed short y;         /** Y coordinate of top left point*/
    char str[50];           /** String of the text*/
    int w;                  /** Width of the text*/
    int h;                  /** Height of the text*/
    unsigned int colour;    /** Colour of the text*/
    float speed;            /** Speed of the moving text*/
    signed short direction; /** Direction of the moving text (=0 moving left, !=0 moving right)*/
} text_moving_t;

/**
 * @brief Updates the position of a moving circle
 * 
 * Circle moves in a circle around the center of the screen
 * 
 * New_angle += rotational_speed  *  milliseconds_passed / milliseconds_in_a_second
 * 
 * rotational speed is set by the object representing the circular path 
 * 
 * New_x_coord = center_of_circular_path  +  radius_of_circular_path * cos(new_angle)
 * New_y_coord = center_of_circular_path  +  radius_of_circular_path * sin(new_angle)
 * 
 * @param circle_moving Reference to the circle object whose position is to be updated
 * @param milli_seconds Milliseconds passed since balls position was last updated
*/
void updateCirclePosition(circle_moving_t *circle_moving, unsigned milli_seconds);

/**
 * @brief Updates the position of a moving rectangle
 * 
 * Rectangle moves in a circle around the center of the screen
 * 
 * New_angle += rotational_speed  *  milliseconds_passed / milliseconds_in_a_second
 * 
 * rotational speed is set by the object representing the circular path 
 * 
 * New_x_coord = center_of_circular_path  -  radius_of_circular_path * cos(new_angle)
 *              - width_recangle / 2
 * New_y_coord = center_of_circular_path  -  radius_of_circular_path * sin(new_angle)
 *              - height_rectangle / 2
 * 
 * @param rect_moving Reference to the rectangle object whose position is to be updated
 * @param milli_seconds Milliseconds passed since balls position was last updated
*/
void updateRectPosition(rect_moving_t *rect_moving, unsigned milli_seconds);

/**
 * @brief Prints a string on the screen
*/
void writeStaticText(void);

/**
 * @brief Updates the position of the moving text
 * 
 * Text moves left to right
 * 
 * @param text_moving Reference to the text object whose position is to be updated
 * @param milli_seconds Milliseconds passed since balls position was last updated
*/
void updateMovingTextPosition(text_moving_t *text_moving, unsigned milli_seconds);

/**
 * @brief Prints a moving string to the screen
 * @param text_moving Reference to the text object which will be moved
*/
void writeMovingText(text_moving_t *text_moving, TickType_t xLastWakeTime,
            TickType_t prevWakeTime);

/**
 * @brief Prints the coordinates of the cursor
*/
void writeMouseCoord();

/**
 * @brief Moves the screen into the direction of the cursor
 * 
 * New_screen_middle = (mouse_coordinates - screen_middle) / 1.7
 * devided by 1.7 just so the screen will move slower than the cursor
*/
void moveScreenInMouseDirection();

void vMovingObjectsDisplay(void *pvParameters);

int xCreateMovingObjectsDisplayTasks(void);

void vDeleteMovingObjectsDisplayTasks(void);
