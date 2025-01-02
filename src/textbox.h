#ifndef _TEXTBOX_H
#define _TEXTBOX_H

#include "text_writer.h"

/**
 * State enumeration for the animated pop-up textbox.
 */
typedef enum TextBoxState {
  TEXT_BOX_CLOSED,
  TEXT_BOX_OPENING,
  TEXT_BOX_OPEN,
  TEXT_BOX_CLOSING,
} TextBoxState;

/**
 * Animated pop-up text box.
 */
typedef struct TextBox {
  /**
   * Initializes the text box.
   */
  const void (*init)(void);
  /**
   * Opens the textbox and starts printing the given text.
   */
  const void (*open)(const char *text);
  /**
   * Called to render updates for the textbox.
   */
  const void (*update)(void);
  /**
   * Current state of the textbox.
   */
  TextBoxState state;
  /**
   * Current y position for the window (which contains the text box).
   */
  uint8_t y;
  /**
   * Text to be written to the text box.
   */
  const char *text;
} TextBox;

/**
 * Animated pop-up text box. Uses the `TextWriter` under the hood to animate
 * text for interaction on the world map or in cutscenes.
 */
extern TextBox textbox;

/**
 * Palette used by the text box.
 */
extern const palette_color_t textbox_palette[4];

#endif
