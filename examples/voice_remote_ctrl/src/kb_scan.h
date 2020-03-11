#ifndef _KB_SCAN_H
#define _KB_SCAN_H

/**********************************
 * A simple keyboard scan module
 **********************************/

/**
 * \berief  Init keyboard module
 */
void kb_init(void);

/**
 * \berief  Scan keys
 *
 *  Once states of keys are changed, below function provided by user application
 *  is called:
 *  void kb_state_changed(const uint32_t old_state, const uint32_t new_state)
 */
void kb_update(void);

#define KEEP_PRESSED_LIMIT 30

#endif
