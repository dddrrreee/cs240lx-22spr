// general IR constants
#define SKIP_EXP 600
#define NEC_REMOTE 0
#define GEN_REMOTE 1

// nec (higher quality) based IR constants
#define NEC_ZERO_EXP 600
#define NEC_ONE_EXP 1600
#define NEC_FIRST_HEADER 9000
#define NEC_SECOND_HEADER 4500
#define NEC_NUM_READINGS 67

// nec button codes
#define NEC_PWR_BTN 0x01de237e2
#define NEC_VOL_UP_BTN 0x1be437e2
#define NEC_VOL_DOWN_BTN 0x19e637e2
#define NEC_O_BTN 0x17e837e2
#define NEC_X_BTN 0x15ea37e2

// generic remote (ah59-02733B) IR constants
#define GEN_ZERO_EXP 400
#define GEN_ONE_EXP 1400
#define GEN_HEADER 4500
#define GEN_NUM_READINGS 77

// generic button codes
#define GEN_PWR_BTN 0xf7000f30
#define GEN_SRC_BTN 0xa7500f30
#define GEN_VOL_UP_BTN 0x87700f30
#define GEN_VOL_DOWN_BTN 0x77800f30
#define GEN_MUTE_BTN 0xe7100f30
#define GEN_REPLAY_BTN 0x68900f30
#define GEN_REWIND_BTN 0x48b00f30
#define GEN_PLAY_PAUSE_BTN 0x58a00f30
#define GEN_FORWARD_BTN 0x38c00f30