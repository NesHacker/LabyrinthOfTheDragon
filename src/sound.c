#pragma bank 30

#include "core.h"
#include "sound.h"

/**
 * Denotes the end of a sound register array.
 */
#define SOUND_END 0

/**
 * Sweep definition helper.
 * @param t Sweep time.
 * @param d Sweep direction.
 * @param s Sweep shift.
 */
#define sweep(t, d, s) \
  (uint8_t)((((t) & 0x3) << 4) | (((d) & 1) << 3) | ((s) & 0x3))

/**
 * Envelope helper.
 * @param i Initial volume for the envelope.
 * @param d Direction, `1` grow louder, `0` grow quieter.
 * @param l Length of each step. Smaller values go faster.
 */
#define envelope(i, d, l) \
  (uint8_t)((((i) & 0xF) << 4) | (((d) & 1) << 3) | ((l) & 0x03))

/**
 * Noise frequency.
 * @param shift Shift clock frequency.
 * @param steps Polynomial counter steps (0: 15, 1: 7).
 * @param ratio Dividing ratio frequency.
 */
#define noise_freq(shift, steps, ratio) \
  (uint8_t)((((shift) & 0xF) << 4) | ((steps & 0x1) << 3) | (ratio & 0x3))

/**
 * Sound register data structure.
 */
typedef struct SoundRegister {
  /**
   * Memory mapped I/O address for the sound register.
   */
  uint8_t *address;
  /**
   * Pointer to the data array for the register.
   */
  uint8_t *data;
  /**
   * Whether or not the register is currently enabled.
   */
  bool enabled;
  /**
   * Delay timer for the register.
   */
  Timer timer;
} SoundRegister;

/**
 * Disabled the given register.
 * @param r The register to disable.
 */
inline void register_disable(SoundRegister *r) {
  r->enabled = false;
}

/**
 * Reads the next data entry for the register.
 * @param r Register to read.
 */
inline void register_next(SoundRegister *r) {
  if (!r->enabled)
    return;

  const uint8_t frames = *r->data++;
  if (!frames) {
    r->enabled = false;
    return;
  }
  init_timer(r->timer, frames);

  const uint8_t value = *r->data++;
  *r->address = value;
}

/**
 * Updates the register.
 * @param r Register to update.
 */
inline void register_update(SoundRegister *r) {
  if (!r->enabled)
    return;

  if (!update_timer(r->timer))
    return;

  register_next(r);
}

volatile SoundRegister nr10 = { (void *)0xFF10 };
volatile SoundRegister nr11 = { (void *)0xFF11 };
volatile SoundRegister nr12 = { (void *)0xFF12 };
volatile SoundRegister nr13 = { (void *)0xFF13 };
volatile SoundRegister nr14 = { (void *)0xFF14 };

volatile SoundRegister nr21 = { (void *)0xFF16 };
volatile SoundRegister nr22 = { (void *)0xFF17 };
volatile SoundRegister nr23 = { (void *)0xFF18 };
volatile SoundRegister nr24 = { (void *)0xFF19 };

volatile SoundRegister nr41 = { (void *)0xFF20 };
volatile SoundRegister nr42 = { (void *)0xFF21 };
volatile SoundRegister nr43 = { (void *)0xFF22 };
volatile SoundRegister nr44 = { (void *)0xFF23 };


/**
 * Initializes a sound register to begin playing the given data.
 * @param r Register to initialize.
 * @param data Time & value data for the register.
 */
void register_init(SoundRegister *r, uint8_t *data) {
  r->data = data;
  r->enabled = true;
  register_next(r);
}

/**
 * Global sound update timer.
 */
Timer sound_update_timer;

void update_sound_isr(void) NONBANKED {
  if (!update_timer(sound_update_timer))
    return;
  reset_timer(sound_update_timer);

  uint8_t _prev_bank = CURRENT_BANK;

  SWITCH_ROM(BANK_30);

  register_update(&nr10);
  register_update(&nr11);
  register_update(&nr12);
  register_update(&nr13);
  register_update(&nr14);

  register_update(&nr21);
  register_update(&nr22);
  register_update(&nr23);
  register_update(&nr24);

  register_update(&nr41);
  register_update(&nr42);
  register_update(&nr43);
  register_update(&nr44);

  SWITCH_ROM(_prev_bank);
}

void sound_init(void) NONBANKED {
  // Note: SO1 (left) and SO2 (right)
  // Enable sound
  NR52_REG = 0b10000000;
  // SO1 & SO2 on at maximum volume
  NR50_REG = 0b01110111;
  // Send channels 1, 2 & 4 to both SO1 & SO2
  NR51_REG = 0b11111111;

  init_timer(sound_update_timer, 4);
  add_TIM(update_sound_isr);
  TAC_REG = 0b00000110;
  set_interrupts(TIM_IFLAG | VBL_IFLAG);
}

void play_sound(void (*sound)(void)) NONBANKED {
  uint8_t _prev_bank = CURRENT_BANK;
  SWITCH_ROM(BANK_30);
  sound();
  SWITCH_ROM(_prev_bank);
}

#define SFX_STAIRS_DURATION 28

const uint8_t sfx_stairs_nr42[] = {
  SFX_STAIRS_DURATION, (6 << 4) | 1,
  SFX_STAIRS_DURATION, (5 << 4) | 1,
  SFX_STAIRS_DURATION, (4 << 4) | 1,
  SFX_STAIRS_DURATION, (3 << 4) | 1,
  SOUND_END
};

const uint8_t sfx_stairs_nr44[] = {
  SFX_STAIRS_DURATION, 0xC0,
  SFX_STAIRS_DURATION, 0xC0,
  SFX_STAIRS_DURATION, 0xC0,
  SFX_STAIRS_DURATION, 0xC0,
  SOUND_END
};

void sfx_stairs(void) {
  NR41_REG = 0x00;
  NR43_REG = (2 << 4)| 3;
  register_init(&nr42, sfx_stairs_nr42);
  register_init(&nr44, sfx_stairs_nr44);
}

void sfx_error(void) {
  NR11_REG = 0;
  NR12_REG = 0xA1;
  NR13_REG = 0x2C;
  NR14_REG = 0xC0;
}

void sfx_wall_hit(void) {
  NR10_REG = 0b00111011;
  NR11_REG = 0b10000000;
  NR12_REG = 0x71;
  NR13_REG = 0x62;
  NR14_REG = 0xC5;
}

#define SFX_MENU_MOVE_DURATION 2

const uint8_t sfx_menu_move_nr13[] = {
  SFX_MENU_MOVE_DURATION, 0x82,
  SFX_MENU_MOVE_DURATION, 0x9C,
  SFX_MENU_MOVE_DURATION, 0xAC,
  SFX_MENU_MOVE_DURATION, 0xC1,
  SOUND_END
};

const uint8_t sfx_menu_move_nr14[] = {
  SFX_MENU_MOVE_DURATION, 0xC7,
  SFX_MENU_MOVE_DURATION, 0xC7,
  SFX_MENU_MOVE_DURATION, 0xC7,
  SFX_MENU_MOVE_DURATION, 0xC7,
  SOUND_END
};

void sfx_menu_move(void) {
  NR10_REG = 0;
  NR11_REG = 0b01000000 | 50;
  NR12_REG = envelope(8, 0, 1);
  register_init(&nr13, sfx_menu_move_nr13);
  register_init(&nr14, sfx_menu_move_nr14);
}

#define SFX_NEXT_ROUND_DURATION 13

const uint8_t sfx_next_round_nr13[] = {
  SFX_NEXT_ROUND_DURATION, 0xA2,
  SFX_NEXT_ROUND_DURATION, 0x82,
  SOUND_END
};

const uint8_t sfx_next_round_nr14[] = {
  SFX_NEXT_ROUND_DURATION, 0x87,
  SFX_NEXT_ROUND_DURATION, 0x87,
  SOUND_END
};

void sfx_next_round(void) {
  NR10_REG = 0;
  NR11_REG = 0b01000000 | 0;
  NR12_REG = envelope(7, 0, 6);
  register_init(&nr13, sfx_next_round_nr13);
  register_init(&nr14, sfx_next_round_nr14);
}

#define SFX_TRANSPORTER 4

const uint8_t sfx_transporter_nr13[] = {
  SFX_TRANSPORTER, 0x6B,
  SFX_TRANSPORTER, 0x39,
  SFX_TRANSPORTER, 0x6B,
  SFX_TRANSPORTER, 0x73,
  SFX_TRANSPORTER, 0x6B,
  SFX_TRANSPORTER, 0x39,
  SFX_TRANSPORTER, 0x6B,
  SFX_TRANSPORTER, 0x73,
  SOUND_END
};

const uint8_t sfx_transporter_nr14[] = {
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SFX_TRANSPORTER, 0xC7,
  SOUND_END
};

void sfx_no_no_square(void) {
  NR10_REG = 0;
  NR11_REG = 0b01000000 | 0;
  NR12_REG = envelope(7, 0, 6);
  register_init(&nr13, sfx_transporter_nr13);
  register_init(&nr14, sfx_transporter_nr14);
}

const uint8_t sfx_melee_attack_nr42[] = {
  20, envelope(1, 1, 2),
  10, envelope(10, 0, 1),
  SOUND_END
};

const uint8_t sfx_melee_attack_nr43[] = {
  20, noise_freq(3, 0, 1),
  10, noise_freq(5, 0, 1),
  SOUND_END
};

const uint8_t sfx_melee_attack_nr44[] = {
  20, 0x80,
  10, 0x80,
  SOUND_END
};

const uint8_t sfx_melee_attack_nr10[] = {
  40, 0,
  30, sweep(1, 0, 7),
  SOUND_END
};

const uint8_t sfx_melee_attack_nr11[] = {
  40, 0,
  30, 0,
  SOUND_END
};

const uint8_t sfx_melee_attack_nr12[] = {
  40, 0,
  30, envelope(15, 0, 7),
  SOUND_END
};

const uint8_t sfx_melee_attack_nr13[] = {
  40, 0,
  30, 0xFF,
  SOUND_END
};

const uint8_t sfx_melee_attack_nr14[] = {
  40, 0x02,
  30, 0x82,
  SOUND_END
};

const void sfx_melee_attack(void) {
  NR41_REG = 63;
  register_init(&nr42, sfx_melee_attack_nr42);
  register_init(&nr43, sfx_melee_attack_nr43);
  register_init(&nr44, sfx_melee_attack_nr44);

  register_init(&nr10, sfx_melee_attack_nr10);
  register_init(&nr11, sfx_melee_attack_nr11);
  register_init(&nr12, sfx_melee_attack_nr12);
  register_init(&nr13, sfx_melee_attack_nr13);
  register_init(&nr14, sfx_melee_attack_nr14);
}

void sfx_monster_death(void) {
  NR41_REG = 0;
  NR42_REG = envelope(12, 0, 7);
  NR43_REG = noise_freq(8, 0, 1);
  NR44_REG = 0x80;
}

const uint8_t nr13_level_up[] = {
  14, 0x15,
  14, 0xE4,
  14, 0x63,
  14, 0x0B,
  14, 0x72,
  14, 0xB1,
  14, 0x05,
  SOUND_END
};

const uint8_t nr14_level_up[] = {
  14, 0x84,
  14, 0x84,
  14, 0x85,
  14, 0x86,
  14, 0x86,
  14, 0x86,
  14, 0x87,
  SOUND_END
};

const uint8_t nr23_level_up[] = {
  14, 0xE4,
  14, 0x63,
  14, 0x0B,
  14, 0x72,
  14, 0xB1,
  14, 0x05,
  14, 0x39,
  SOUND_END
};

const uint8_t nr24_level_up[] = {
  14, 0x84,
  14, 0x85,
  14, 0x86,
  14, 0x86,
  14, 0x86,
  14, 0x87,
  14, 0x87,
  SOUND_END
};

void sfx_level_up(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(14, 0, 5);
  register_init(&nr13, nr13_level_up);
  register_init(&nr14, nr14_level_up);

  NR21_REG = 0;
  NR22_REG = envelope(8, 0, 7);
  register_init(&nr23, nr23_level_up);
  register_init(&nr24, nr24_level_up);
}

const uint8_t nr41_light_fire[] = {
  10, 16,
  30, 0,
  SOUND_END
};

const uint8_t nr42_light_fire[] = {
  10, envelope(0, 1, 1),
  30, envelope(10, 0, 1),
  SOUND_END
};

const uint8_t nr43_light_fire[] = {
  1, noise_freq(9, 1, 0),
  SOUND_END
};

const uint8_t nr44_light_fire[] = {
  10, 0xC0,
  30, 0x80,
  SOUND_END
};

void sfx_light_fire(void) {
  register_init(&nr41, nr41_light_fire);
  register_init(&nr42, nr42_light_fire);
  register_init(&nr43, nr43_light_fire);
  register_init(&nr44, nr44_light_fire);
}

const uint8_t nr13_chest_open[] = {
  8, 0xD6,
  8, 0xCE,
  8, 0xE0,
  SOUND_END
};

const uint8_t nr14_chest_open[] = {
  8, 0x87,
  8, 0x87,
  8, 0x87,
  SOUND_END
};

void sfx_open_chest(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(14, 0, 1);
  register_init(&nr13, nr13_chest_open);
  register_init(&nr14, nr14_chest_open);
}

#define DOOR_UNLOCK_DELAY 20

const uint8_t nr41_door_unlock[] = {
  1, 0x63,
  SOUND_END,
};

const uint8_t nr42_door_unlock[] = {
  DOOR_UNLOCK_DELAY, envelope(8, 0, 1),
  DOOR_UNLOCK_DELAY, envelope(10, 0, 1),
  SOUND_END
};

const uint8_t nr43_door_unlock[] = {
  DOOR_UNLOCK_DELAY, noise_freq(8, 1, 2),
  DOOR_UNLOCK_DELAY, noise_freq(1, 0, 0),
  SOUND_END
};

const uint8_t nr44_door_unlock[] = {
  DOOR_UNLOCK_DELAY, 0xC0,
  DOOR_UNLOCK_DELAY, 0x80,
  SOUND_END
};

void sfx_door_unlock(void) {
  NR41_REG = 0;
  register_init(&nr42, nr42_door_unlock);
  register_init(&nr43, nr43_door_unlock);
  register_init(&nr44, nr44_door_unlock);
}

void sfx_big_door_open(void) {
  NR41_REG = 0;
  NR42_REG = envelope(11, 0, 2);
  NR43_REG = noise_freq(9, 1, 1);
  NR44_REG = 0x80;
}

#define BATTLE_START_DELAY 15
#define BATTLE_START_DELAY2 18
#define BATTLE_START_VOLUME 5

const uint8_t nr42_battle_start[] = {
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY2, envelope(BATTLE_START_VOLUME, 0, 3),
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY, envelope(BATTLE_START_VOLUME, 0, 0),
  BATTLE_START_DELAY2, envelope(BATTLE_START_VOLUME, 0, 3),
  SOUND_END
};

const uint8_t nr43_battle_start[] = {
  BATTLE_START_DELAY, noise_freq(4, 0, 0),
  BATTLE_START_DELAY, noise_freq(6, 0, 1),
  BATTLE_START_DELAY, noise_freq(6, 0, 2),
  BATTLE_START_DELAY2, noise_freq(8, 1, 2),
  BATTLE_START_DELAY, noise_freq(4, 0, 0),
  BATTLE_START_DELAY, noise_freq(6, 0, 1),
  BATTLE_START_DELAY, noise_freq(6, 0, 2),
  BATTLE_START_DELAY2, noise_freq(8, 1, 2),
  SOUND_END,
};

const uint8_t nr44_battle_start[] = {
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY2, 0x80,
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY, 0x80,
  BATTLE_START_DELAY2, 0x80,
  SOUND_END
};

void sfx_start_battle(void) {
  NR41_REG = 0;
  register_init(&nr42, nr42_battle_start);
  register_init(&nr43, nr43_battle_start);
  register_init(&nr44, nr44_battle_start);
}

#define MONSTER_ATTACK_DELAY 18

const uint8_t nr10_monster_attack1[] = {
  MONSTER_ATTACK_DELAY, sweep(7, 0, 5),
  MONSTER_ATTACK_DELAY, sweep(7, 0, 5),
  SOUND_END
};

const uint8_t nr12_monster_attack1[] = {
  MONSTER_ATTACK_DELAY, envelope(14, 0, 0),
  MONSTER_ATTACK_DELAY, envelope(14, 0, 2),
  SOUND_END
};

const uint8_t nr14_monster_attack[] = {
  MONSTER_ATTACK_DELAY, 0x80,
  MONSTER_ATTACK_DELAY, 0x80,
  SOUND_END
};

void sfx_monster_attack1(void) {
  register_init(&nr10, nr10_monster_attack1);
  NR11_REG = 0;
  register_init(&nr12, nr12_monster_attack1);
  NR13_REG = 0x9B;
  register_init(&nr14, nr14_monster_attack);
}

void sfx_monster_critical(void) {
  NR41_REG = 0;
  NR42_REG = envelope(13, 0, 3);
  NR43_REG = noise_freq(8, 1, 2);
  NR44_REG = 0x80;
}

#define MONSTER_ATTACK2_DURATION 4

const uint8_t nr13_monster_attack2[] = {
  MONSTER_ATTACK2_DURATION, 0x15,
  MONSTER_ATTACK2_DURATION, 0xB5,
  MONSTER_ATTACK2_DURATION, 0x63,
  MONSTER_ATTACK2_DURATION, 0x0B,
  MONSTER_ATTACK2_DURATION, 0x72,
  MONSTER_ATTACK2_DURATION, 0x9D,
  MONSTER_ATTACK2_DURATION, 0x05,
  SOUND_END
};

const uint8_t nr14_monster_attack2[] = {
  MONSTER_ATTACK2_DURATION, 0x84,
  MONSTER_ATTACK2_DURATION, 0x84,
  MONSTER_ATTACK2_DURATION, 0x85,
  MONSTER_ATTACK2_DURATION, 0x86,
  MONSTER_ATTACK2_DURATION, 0x86,
  MONSTER_ATTACK2_DURATION, 0x86,
  MONSTER_ATTACK2_DURATION, 0x87,
  SOUND_END
};


void sfx_monster_attack2(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(9, 0, 1);
  register_init(&nr13, nr13_monster_attack2);
  register_init(&nr14, nr14_monster_attack2);
}

#define BATTLE_SUCCESS_DELAY 15


const uint8_t env_battle_success[] = {
  BATTLE_SUCCESS_DELAY, envelope(11, 0, 1),
  BATTLE_SUCCESS_DELAY, envelope(12, 0, 3),
  SOUND_END
};

const uint8_t trigger_battle_success[] = {
  BATTLE_SUCCESS_DELAY, 0x87,
  BATTLE_SUCCESS_DELAY, 0x87,
  SOUND_END
};

void sfx_battle_success(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR13_REG = 0x05;
  register_init(&nr12, env_battle_success);
  register_init(&nr14, trigger_battle_success);

  NR21_REG = 0b11000000;
  NR23_REG = 0x39;
  register_init(&nr22, env_battle_success);
  register_init(&nr24, trigger_battle_success);
}

#define BATTLE_DEATH_DURATION1 16
#define BATTLE_DEATH_DURATION2 37

const uint8_t env_battle_death[] = {
  BATTLE_DEATH_DURATION1, envelope(10, 0, 1),
  BATTLE_DEATH_DURATION1, envelope(9, 0, 1),
  BATTLE_DEATH_DURATION1, envelope(9, 0, 1),
  BATTLE_DEATH_DURATION1, envelope(8, 0, 1),
  BATTLE_DEATH_DURATION1, envelope(8, 0, 1),
  BATTLE_DEATH_DURATION1, envelope(7, 0, 1),
  BATTLE_DEATH_DURATION2, envelope(7, 0, 3),
  BATTLE_DEATH_DURATION1, envelope(12, 0, 3),
  SOUND_END,
};

const uint8_t freq_battle_death[] = {
  BATTLE_DEATH_DURATION1, 0x6B,
  BATTLE_DEATH_DURATION1, 0x39,
  BATTLE_DEATH_DURATION1, 0x05,
  BATTLE_DEATH_DURATION1, 0xD6,
  BATTLE_DEATH_DURATION1, 0x72,
  BATTLE_DEATH_DURATION1, 0x0B,
  BATTLE_DEATH_DURATION2, 0xAC,
  BATTLE_DEATH_DURATION1, 0x58,
  SOUND_END,
};

const uint8_t trigger_battle_death[] = {
  BATTLE_DEATH_DURATION1, 0x87,
  BATTLE_DEATH_DURATION1, 0x87,
  BATTLE_DEATH_DURATION1, 0x87,
  BATTLE_DEATH_DURATION1, 0x86,
  BATTLE_DEATH_DURATION1, 0x86,
  BATTLE_DEATH_DURATION1, 0x86,
  BATTLE_DEATH_DURATION2, 0x85,
  BATTLE_DEATH_DURATION1, 0x83,
  SOUND_END
};

void sfx_battle_death(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  register_init(&nr12, env_battle_death);
  register_init(&nr13, freq_battle_death);
  register_init(&nr14, trigger_battle_death);
}

const uint8_t freq_monster_fail[] = {
  10, 0x9C,
  10, 0xC9,
  10, 0x9C,
  SOUND_END
};

const uint8_t trigger_monster_fail[] = {
  10, 0x81,
  10, 0x81,
  10, 0x81,
  SOUND_END
};

void sfx_monster_fail(void) {
  NR11_REG = 0;
  NR12_REG = envelope(13, 0, 1);
  register_init(&nr13, freq_monster_fail);
  register_init(&nr14, trigger_monster_fail);
}

const uint8_t env_action_surge[] = {
  4, envelope(0, 1, 1),
  4, envelope(10, 0, 0),
  4, envelope(10, 0, 0),
  10, envelope(8, 0, 1),
  4, envelope(0, 1, 1),
  4, envelope(10, 0, 0),
  4, envelope(10, 0, 0),
  4, envelope(8, 0, 1),
  SOUND_END
};

const uint8_t freq_action_surge[] = {
  4, noise_freq(1, 0, 0),
  4, noise_freq(2, 0, 1),
  4, noise_freq(3, 0, 2),
  10, noise_freq(4, 0, 3),
  4, noise_freq(1, 0, 0),
  4, noise_freq(2, 0, 1),
  4, noise_freq(3, 0, 2),
  4, noise_freq(4, 0, 3),
  SOUND_END
};

const uint8_t trigger_action_surge[] = {
  4, 0x80,
  4, 0x80,
  4, 0x80,
  10, 0x80,
  4, 0x80,
  4, 0x80,
  4, 0x80,
  4, 0x80,
  SOUND_END
};

void sfx_action_surge(void) {
  NR41_REG = 0;
  register_init(&nr42, env_action_surge);
  register_init(&nr43, freq_action_surge);
  register_init(&nr44, trigger_action_surge);
}

const uint8_t freq_miss[] = {
  12, 0xDA,
  10, 0x15,
  SOUND_END
};

const uint8_t trigger_miss[] = {
  12, 0x83,
  10, 0x84,
  SOUND_END
};

void sfx_miss(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(12, 0, 1);
  register_init(&nr13, freq_miss);
  register_init(&nr14, trigger_miss);
}

static const uint8_t freq_heal[] = {
  15, 0x82,
  15, 0x9C,
  15, 0xAC,
  15, 0xBD,
  SOUND_END
};

static const uint8_t trigger_heal[] = {
  15, 0x87,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  SOUND_END
};

static const uint8_t env_heal2[] = {
  30, envelope(0, 0, 1),
  15, envelope(8, 0, 1),
  15, envelope(7, 0, 1),
  15, envelope(6, 0, 1),
  15, envelope(5, 0, 1),
  SOUND_END,
};

static const uint8_t freq_heal2[] = {
  30, 0,
  15, 0x82,
  15, 0x9C,
  15, 0xAC,
  15, 0xBD,
  SOUND_END
};

static const uint8_t trigger_heal2[] = {
  30, 0x00,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  SOUND_END
};

void sfx_heal(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(10, 0, 1);
  register_init(&nr13, freq_heal);
  register_init(&nr14, trigger_heal);

  NR21_REG = 0;
  register_init(&nr22, env_heal2);
  register_init(&nr23, freq_heal2);
  register_init(&nr24, trigger_heal2);
}

static const uint8_t freq_big_powerup[] = {
  8, 0x15,
  8, 0x83,
  8, 0xE4,
  8, 0x11,
  8, 0x63,
  SOUND_END
};

static const uint8_t trigger_big_powerup[] = {
  8, 0x84,
  8, 0x84,
  8, 0x84,
  8, 0x85,
  8, 0x85,
  SOUND_END
};

static const uint8_t env_big_powerup2[] = {
  10, envelope(0, 0, 1),
  40, envelope(7, 0, 1),
  1, envelope(2, 0, 1),
  SOUND_END
};

static const uint8_t freq_big_powerup2[] = {
  10, 0,
  8, 0x0B,
  8, 0x41,
  8, 0x72,
  8, 0x88,
  8, 0xB1,
  8, 0x41,
  8, 0x72,
  8, 0x88,
  8, 0xB1,
  SOUND_END
};

static const uint8_t trigger_big_powerup2[] = {
  10, 0,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  8, 0x86,
  SOUND_END
};

void sfx_big_powerup(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(10, 0, 1);
  register_init(&nr13, freq_big_powerup);
  register_init(&nr14, trigger_big_powerup);
  NR21_REG = 0b10000000;
  register_init(&nr22, env_big_powerup2);
  register_init(&nr23, freq_big_powerup2);
  register_init(&nr24, trigger_big_powerup2);
}

void sfx_mid_powerup(void) {
  NR10_REG = sweep(7, 0, 7);
  NR11_REG = 0;
  NR12_REG = envelope(13, 0, 2);
  NR13_REG = 0x68;
  NR14_REG = 0x80;
}

void sfx_poison_spray(void) {
  NR41_REG = 0;
  NR42_REG = envelope(12, 0, 2);
  NR43_REG = noise_freq(2, 0, 1);
  NR44_REG = 0x80;
}

const uint8_t monk_strike_envelope[] = {
  40, envelope(0, 0, 1),
  1, envelope(12, 0, 1),
  SOUND_END
};

const uint8_t monk_strike_trigger[] = {
  40, 0,
  1, 0x80,
  SOUND_END
};

void sfx_monk_strike(void) {
  // Punch 1
  NR10_REG = sweep(1, 1, 3);
  NR11_REG = 0;
  NR12_REG = envelope(12, 0, 1);
  NR13_REG = 0x00;
  NR14_REG = 0x87;

  // Punch 2
  NR41_REG = 0;
  register_init(&nr42, monk_strike_envelope);
  NR43_REG = noise_freq(4, 0, 3);
  register_init(&nr44, monk_strike_trigger);
}

static const uint8_t env_evade[] = {
  SFX_STAIRS_DURATION, envelope(9, 0, 1),
  SFX_STAIRS_DURATION, envelope(8, 0, 1),
  SOUND_END
};

static const uint8_t trigger_evade[] = {
  SFX_STAIRS_DURATION, 0xC0,
  SFX_STAIRS_DURATION, 0xC0,
  SOUND_END
};

void sfx_evade(void) {
  NR41_REG = 0x00;
  NR43_REG = (2 << 4)| 3;
  register_init(&nr42, env_evade);
  register_init(&nr44, trigger_evade);
}

static const uint8_t freq_magic_missile[] = {
  15, 0x6B,
  15, 0x7B,
  15, 0x82,
  15, 0xAC,
  SOUND_END
};

static const uint8_t trigger_magic_missile[] = {
  15, 0x87,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  SOUND_END
};

static const uint8_t env_magic_missile2[] = {
  30, envelope(0, 0, 1),
  15, envelope(8, 0, 1),
  15, envelope(7, 0, 1),
  15, envelope(6, 0, 1),
  15, envelope(5, 0, 1),
  SOUND_END,
};

static const uint8_t freq_magic_missile2[] = {
  30, 0,
  15, 0x6B,
  15, 0x7B,
  15, 0x82,
  15, 0xAC,
  SOUND_END
};

static const uint8_t trigger_magic_missile2[] = {
  30, 0x00,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  15, 0x87,
  SOUND_END
};

void sfx_magic_missile(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(10, 0, 1);
  register_init(&nr13, freq_magic_missile);
  register_init(&nr14, trigger_magic_missile);

  NR21_REG = 0;
  register_init(&nr22, env_magic_missile2);
  register_init(&nr23, freq_magic_missile2);
  register_init(&nr24, trigger_magic_missile2);
}

static const uint8_t freq_falling[] = {
  2, 0xAC,
  4, 0x82,
  8, 0x7B,
  16, 0x6B,
  32, 0x37,
  SOUND_END
};

static const uint8_t trigger_falling[] = {
  2, 0x87,
  4, 0x87,
  8, 0x87,
  16, 0x87,
  32, 0x87,
  SOUND_END
};

void sfx_falling(void) {
  NR10_REG = 0;
  NR11_REG = 0;
  NR12_REG = envelope(10, 0, 2);
  register_init(&nr13, freq_falling);
  register_init(&nr14, trigger_falling);
}

void sfx_neshacker_presents(void) {
  sfx_magic_missile();
}

const uint8_t noise_title_fire[] = {
  4, noise_freq(8, 0, 1),
  4, noise_freq(8, 0, 1),
  4, noise_freq(8, 0, 1),
  4, noise_freq(8, 0, 1),
  4, noise_freq(8, 0, 1),
  4, noise_freq(8, 0, 1),
  4, noise_freq(7, 0, 1),
  4, noise_freq(6, 0, 1),
  4, noise_freq(5, 0, 1),
  SOUND_END,
};

const uint8_t trigger_title_fire[] = {
  10, 0x80,
  10, 0x80,
  10, 0x80,
  10, 0x80,
  10, 0x80,
  10, 0x80,
  4, 0x80,
  4, 0x80,
  4, 0x80,
  SOUND_END,
};

void sfx_title_fire(void) {
  NR41_REG = 0;
  NR42_REG = envelope(12, 0, 7);
  register_init(&nr43, noise_title_fire);
  register_init(&nr44, trigger_title_fire);
}

void sfx_hero_selected(void) {
  sfx_battle_success();
}

void sfx_test(void) {
  sfx_falling();

  // NR10_REG = sweep(7, 0, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(10, 0, 1);
  // NR13_REG = 0x2C;
  // NR14_REG = 0x80;

  // NR10_REG = sweep(7, 0, 5);
  // NR11_REG = 0;
  // NR12_REG = envelope(14, 0, 2);
  // NR13_REG = 0x9B;
  // NR14_REG = 0x80;

  // NR10_REG = sweep(7, 0, 5);
  // NR12_REG = envelope(14, 0, 2);


  // Punch 1
  // NR10_REG = sweep(1, 1, 3);
  // NR11_REG = 0;
  // NR12_REG = envelope(10, 0, 1);
  // NR13_REG = 0x00;
  // NR14_REG = 0x87;

  // Punch 2
  // NR41_REG = 0;
  // NR42_REG = envelope(12, 0, 1);
  // NR43_REG = noise_freq(4, 0, 3);
  // NR44_REG = 0x80;

  // Boingg 2
  // NR10_REG = sweep(7, 0, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(14, 0, 2);
  // NR13_REG = 0x9B;
  // NR14_REG = 0x80;

  // Boingg
  // NR10_REG = sweep(7, 0, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(13, 0, 1);
  // NR13_REG = 0x68;
  // NR14_REG = 0x82;

  // "Hard Fall"
  // NR10_REG = sweep(7, 1, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(15, 0, 7);
  // NR13_REG = 0x68;
  // NR14_REG = 0x85;

  // "Brrrrunnn" Sound
  // NR10_REG = sweep(5, 1, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(15, 0, 7);
  // NR13_REG = 0xFF;
  // NR14_REG = 0x84;

  // "Zip Up" Sound
  // NR10_REG = sweep(2, 0, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(15, 0, 7);
  // NR13_REG = 0xFF;
  // NR14_REG = 0x80;

  // "Rip" Sound
  // NR10_REG = sweep(1, 0, 7);
  // NR11_REG = 0;
  // NR12_REG = envelope(15, 0, 7);
  // NR13_REG = 0xFF;
  // NR14_REG = 0x82;
}
