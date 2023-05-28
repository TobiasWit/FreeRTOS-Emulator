
/**
 * @brief Prints how often the buttons A, B, C and D were pressed
*/
void writePressedButtonsCount(void);

/**
 * @brief Checks if the left mouse or the buttons A, B, C and D were pressed
*/
void CheckButtonInput(void);

int checkButtonPress(unsigned char keycode, TickType_t *last_pressed, TickType_t *debounce_delay);