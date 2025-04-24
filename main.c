#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include <string.h>
#define MODULE_NAME "nipvp"
PSP_MODULE_INFO(MODULE_NAME, 0x1007, 1, 0);

#define EMULATOR_DEVCTL__IS_EMULATOR 0x03

#define REF(x) *((int*)(x))
#define REF_BYTE(x) *((char*)(x))
#define REF_HALF_WORD(x) *((short*)(x))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define J(addr) (0x8000000 | ((int)(addr) >> 2))

#define INIT_USER_MODULE_ADDR() int user_text_addr; get_from_reg(user_text_addr, at);
#define USER_ADDR(addr) ({INIT_USER_MODULE_ADDR(); (int)addr - (int)&__executable_start + (int)user_text_addr;})
#define J_USER(target) J(({INIT_USER_MODULE_ADDR(); ((int (*)(int, int, int))USER_ADDR(&make_trampoline))(1, user_text_addr, USER_ADDR(target));}))
#define USER_ALIAS_OF(name) (*((typeof(name)*)USER_ADDR(&name)))

#define REGION_ADDR 0x08901b6b

#define GAME_MODE_ADDR_JPN 0x8c05800
#define GAME_MODE_ADDR_USA 0x8c069c0

#define PLAYER_BASE_ADDR_JPN 0x8bD898C
#define PLAYER_BASE_ADDR_USA 0x8bD9B54

#define MISSION_CODE_ADDR_JPN 0x8c05884
#define MISSION_CODE_ADDR_USA 0x8c06A44

#define PLAYER_INIT_HOOK_ADDR_JPN 0x0895bb28
#define PLAYER_INIT_HOOK_ADDR_USA 0x0895c458

#define PLAYER_1_SET_POS_HOOK_ADDR_JPN 0x08901470
#define PLAYER_1_SET_POS_HOOK_ADDR_USA 0x08901DA0

#define PLAYER_2_SET_POS_HOOK_ADDR_JPN 0x088FFA30
#define PLAYER_2_SET_POS_HOOK_ADDR_USA 0x08900360

#define LOAD_COORDINATE_HOOK_ADDR_JPN 0x08930a34
#define LOAD_COORDINATE_HOOK_ADDR_USA 0x08931364

#define SET_GAME_MODE_HOOK_ADDR_JPN 0x0883e770
#define SET_GAME_MODE_HOOK_ADDR_USA 0x0883f0a0

#define LOAD_TAG_MODE_HOOK_ADDR_JPN 0x089fa448
#define LOAD_TAG_MODE_HOOK_ADDR_USA 0x089fadc8

#define SET_ATTACK_VALUE_HOOK_ADDR_JPN 0x088963b8
#define SET_ATTACK_VALUE_HOOK_ADDR_USA 0x08896CE8

// #define GAME_MODE_ADDR       (REF_BYTE(REGION_ADDR) == 0x0 ? GAME_MODE_ADDR_JPN       : GAME_MODE_ADDR_USA)
// #define PLAYER_BASE_ADDR     (REF_BYTE(REGION_ADDR) == 0x0 ? PLAYER_BASE_ADDR_JPN     : PLAYER_BASE_ADDR_USA)
// #define MISSION_CODE_ADDR    (REF_BYTE(REGION_ADDR) == 0x0 ? MISSION_CODE_ADDR_JPN    : MISSION_CODE_ADDR_USA)
// #define PLAYER_INIT_HOOK_ADDR       (REF_BYTE(REGION_ADDR) == 0x0 ? PLAYER_INIT_HOOK_ADDR_JPN       : PLAYER_INIT_HOOK_ADDR_USA)
// #define PLAYER_1_SET_POS_HOOK_ADDR  (REF_BYTE(REGION_ADDR) == 0x0 ? PLAYER_1_SET_POS_HOOK_ADDR_JPN  : PLAYER_1_SET_POS_HOOK_ADDR_USA)
// #define PLAYER_2_SET_POS_HOOK_ADDR  (REF_BYTE(REGION_ADDR) == 0x0 ? PLAYER_2_SET_POS_HOOK_ADDR_JPN  : PLAYER_2_SET_POS_HOOK_ADDR_USA)
// #define LOAD_COORDINATE_HOOK_ADDR   (REF_BYTE(REGION_ADDR) == 0x0 ? LOAD_COORDINATE_HOOK_ADDR_JPN   : LOAD_COORDINATE_HOOK_ADDR_USA)
#define SET_GAME_MODE_HOOK_ADDR     (REF_BYTE(REGION_ADDR) == 0x0 ? SET_GAME_MODE_HOOK_ADDR_JPN     : SET_GAME_MODE_HOOK_ADDR_USA)
#define LOAD_TAG_MODE_HOOK_ADDR     (REF_BYTE(REGION_ADDR) == 0x0 ? LOAD_TAG_MODE_HOOK_ADDR_JPN     : LOAD_TAG_MODE_HOOK_ADDR_USA)

//#define IS_TAG_MODE (REF_BYTE(GAME_MODE_ADDR) == 0x02)
static int IS_TAG_MODE = 0;

extern char __executable_start;
extern char end;

typedef struct {
    int addr;
    int inst;
    int ori_inst;
} Patch;

#define FUNC_HOOK_NUM 5

#define MAX_INST_PATCHES 23
#define MAX_PATCHES      (FUNC_HOOK_NUM + MAX_INST_PATCHES + 1)

static Patch instruction_patch_set1[] = {
    // Remove enemies
    {0x08901238, 0x1000006B},
    // 2P Health Bar
    {0x08888A7C, 0x00000000},
    {0x0888D3AC, 0x00000000},
    {0x0888D544, 0x00000000},
    // Better Ending
    {0x08938E00, 0x00000000},
    // No more cache!
    {0x08939054, 0x00000000},
    {0x08938E10, 0x34040034},
    // No reviving
    {0x08938e78, 0x34040008},
    // Skip Opening
    {0x088FD920, 0x00000000},
    {0x089019f4, 0x10000004},
    {0x08901A1C, 0x34020000},
    {0x08939a4c, 0x00000000},
    // Remove Circles
    {0x08880808, 0x1000001A},
    // Ad-Hoc port
    {0x08814ecc, 0x3405030a},
    {0x08814b90, 0x3406030a},
    {0x08814ccc, 0x3416030a},
    // Ad-Hoc matching port
    {0x088117e8, 0x34060002},
    {0x088116f4, 0x34060002},
    // 1P Balance
    {0x08892864, 0x34050180},
    {0x08892860, 0x34040080},
    // 2P Balance
    {0x08892A04, 0x34050180},
    {0x08892A00, 0x34040080},
    // DEF Balance
    {0x08896258, 0x34020150},
    {}, // end
};

static Patch instruction_patch_set2[] = {
    // Remove enemies
    {0x08901B68,0x1000006B},
    // 2P Health Bar
    {0x088893AC, 0x00000000},
    {0x0888DCDC, 0x00000000},
    {0x0888DE74, 0x00000000},
    // Better Ending
    {0x08939730, 0x00000000},
    // No more cache!
    {0x08939984, 0x00000000},
    {0x08939740, 0x34040034},
    // No reviving
    {0x089397a8, 0x34040008},
    // Skip Opening
    {0x088FE250, 0x00000000},
    {0x08902324, 0x10000004},
    {0x0890234C, 0x34020000},
    {0x0893a37c, 0x00000000},
    // Remove Circles
    {0x08881138, 0x1000001A},
    // Ad-Hoc port
    {0x08814ef0, 0x3405030a},
    {0x08814bb4, 0x3406030a},
    {0x08814cf0, 0x3416030a},
    // Ad-Hoc matching port
    {0x0881180c, 0x34060002},
    {0x08811718, 0x34060002},
    // 1P Balance
    {0x08893194, 0x34050180},
    {0x08893190, 0x34040080},
    // 2P Balance
    {0x08893334, 0x34050180},
    {0x08893330, 0x34040080},
    // DEF Balance
    {0x08896B88, 0x34020150},
    {}, // end
};

static struct {
    Patch function_hook_patches[FUNC_HOOK_NUM];
    Patch instruction_patches[MAX_INST_PATCHES + 1];
} _patches;

void init_patch_buffer(void) {
    static int inited = 0;
    if (inited) return;
    inited = 1;
    Patch *src = (REF_BYTE(REGION_ADDR)==0
                  ? instruction_patch_set1
                  : instruction_patch_set2);

    Patch *dst = _patches.instruction_patches;
    int i;
    for (i = 0; i < MAX_INST_PATCHES && src[i].addr; i++) {
        dst[i].addr     = src[i].addr;
        dst[i].inst     = src[i].inst;
        dst[i].ori_inst = 0;
    }
    // sentinel
    dst[i].addr     = 0;
    dst[i].inst     = 0;
    dst[i].ori_inst = 0;
}

#define patches ((Patch*)USER_ADDR(&_patches))

void player_info_hook() {
    int a;
    if (REF_BYTE(REGION_ADDR) == 0x0)
        a = REF(PLAYER_BASE_ADDR_JPN);
    else
        a = REF(PLAYER_BASE_ADDR_USA);
    a = REF(a + 4);
    int b = REF(a);
    int c = REF(b);
    a = REF(c + 0x90);
    a = REF(a);

    // PVP
    REF_BYTE(c + 0xa4) = 2;
    REF_BYTE(a + 0x8) = 2;
    REF_BYTE(a + 0x538) = 2;
}

typedef struct {
    int idx;
    float x;
    float z;
    float y;
    float unknown1;
    int orientation;
} Coordinate;

#define get_from_reg(var, reg) asm( \
    ".set noat\n" \
    "move %0,$"#reg \
    : "=r" (var) \
);

#define save_to_reg(reg, var) asm( \
    "move $"#reg",%0" \
    : \
    : "r" (var) \
);

#define save_to_stack(reg) asm( \
    "addiu $sp,$sp,-4\n" \
    "sw $"#reg",0x0($sp)" \
);

#define restore_from_stack(reg) asm( \
    "lw $"#reg",0x0($sp)\n" \
    "addiu $sp,$sp,4" \
);

#define return_to(addr) asm( \
    "move $ra,%0\n" \
    : \
    : "r" (addr) \
    : "v0" \
);

void set_attack_value_hook() {

    int value;
    int multiplier;
    
    asm("lh %0, 0x6($s1)" : "=r"(value));
    asm("lh %0, 0x34($s1)" : "=r"(multiplier));

    if (value > 65) {
        value -= 20;
    }
    if (value > 8 && value < 20) {
        value = 6;
    }
    if (multiplier < 580){
        multiplier = 580;
    }
    if (multiplier > 670){
        if (value < 20) {
            multiplier = 580;
        }
        else{
            if (value < 65){
            multiplier += 50;
            }
        }
    }

    asm("move $s5, %0" :: "r"(value));
    asm("move $a0, %0" :: "r"(multiplier));
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(SET_ATTACK_VALUE_HOOK_ADDR_JPN + 4)
    else
        return_to(SET_ATTACK_VALUE_HOOK_ADDR_USA + 4)
}

int _p1_pos_idx;
int _p2_pos_idx;
#define p1_pos_idx USER_ALIAS_OF(_p1_pos_idx)
#define p2_pos_idx USER_ALIAS_OF(_p2_pos_idx)

void set_p1_pos_hook() {
    save_to_stack(a0);
    save_to_stack(a2);
    asm(
        "lhu %0,0x8($s6)\n"
        : "=r" (p1_pos_idx)
    );
    p2_pos_idx = p1_pos_idx + 1;
    int mission;
    if (REF_BYTE(REGION_ADDR) == 0x0)
        mission = REF(MISSION_CODE_ADDR_JPN);
    else
        mission = REF(MISSION_CODE_ADDR_USA);
    if (mission == 0x9d || mission == 0x9e || mission == 0xa8) {
        p2_pos_idx = p1_pos_idx - 1;
    }
    save_to_reg(a1, p1_pos_idx);
    restore_from_stack(a2);
    restore_from_stack(a0);
    //return_to(PLAYER_1_SET_POS_HOOK_ADDR + 8);
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(PLAYER_1_SET_POS_HOOK_ADDR_JPN + 8)
    else
        return_to(PLAYER_1_SET_POS_HOOK_ADDR_USA + 8)
}

void set_p2_pos_hook() {
    save_to_stack(a0);
    save_to_reg(a1, p2_pos_idx);
    restore_from_stack(a0);
    //return_to(PLAYER_2_SET_POS_HOOK_ADDR + 8);
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(PLAYER_2_SET_POS_HOOK_ADDR_JPN + 8)
    else
        return_to(PLAYER_2_SET_POS_HOOK_ADDR_USA + 8)
}

static float _offset = 100.;
#define offset USER_ALIAS_OF(_offset)

void load_coordinate_hook() {
    save_to_stack(a0);
    Coordinate *coordinate_list = 0;
    get_from_reg(coordinate_list, v0);
    if (!coordinate_list || coordinate_list[0].idx != p1_pos_idx) {  // not a coordinate list
        goto exit;
    }

    Coordinate *p1_coordinate = coordinate_list;
    Coordinate *p2_coordinate = coordinate_list - p1_pos_idx + p2_pos_idx;

    int orientation = p1_coordinate->orientation * 180 / 2048;
    if (orientation < 0) {
        orientation += 360;
    }
    if (orientation < 45) {
        p2_coordinate->x = p1_coordinate->x;
        p2_coordinate->y = p1_coordinate->y + offset;
    } else if (orientation < 135) {
        p2_coordinate->x = p1_coordinate->x + offset;
        p2_coordinate->y = p1_coordinate->y;
    } else if (orientation < 225) {
        p2_coordinate->x = p1_coordinate->x;
        p2_coordinate->y = p1_coordinate->y - offset;
    } else if (orientation < 315) {
        p2_coordinate->x = p1_coordinate->x - offset;
        p2_coordinate->y = p1_coordinate->y;
    }
    p2_coordinate->orientation = p1_coordinate->orientation + 2048;

    exit:
    restore_from_stack(a0);
    asm("lui $a1,0x3f80");  // original instruction
    //return_to(LOAD_COORDINATE_HOOK_ADDR + 8);
    if (REF_BYTE(REGION_ADDR) == 0x0)
        return_to(LOAD_COORDINATE_HOOK_ADDR_JPN + 8)
    else
        return_to(LOAD_COORDINATE_HOOK_ADDR_USA + 8)
}

int _trampolines [FUNC_HOOK_NUM * 3] = {};
int _trampolines_used_count = 0;
#define trampolines USER_ALIAS_OF(_trampolines)
#define trampolines_used_count USER_ALIAS_OF(_trampolines_used_count)

int make_trampoline(int user_mode, int user_text_addr, int target) {
    int tramp_addr;
    if (user_mode) {
        tramp_addr = (int)(trampolines + trampolines_used_count++ * 3);
    } else {
        SceUID block_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_High, 12, NULL);
        tramp_addr = (int)sceKernelGetBlockHeadAddr(block_id);
    }
    REF(tramp_addr) = 0x3c010000 | (user_text_addr >> 16); // lui at,addr_h
    REF(tramp_addr + 4) = J(target);
    REF(tramp_addr + 8) = 0x34210000 | (user_text_addr & 0x0000ffff); // ori at,at,addr_l
    return tramp_addr;
}


inline static void patch_instruction(int addr, int inst) {
    REF(addr) = inst;
    asm(
        "cache 0x1a,0x0(%0)\n"
        "cache 8,0x0(%0)"
        :
        : "r" (addr)
    );
}

static int _is_patched = 0;
#define is_patched USER_ALIAS_OF(_is_patched)

void _start(int ignore_mode) {
    IS_TAG_MODE = REF_BYTE(REF_BYTE(REGION_ADDR) == 0x0 ? GAME_MODE_ADDR_JPN : GAME_MODE_ADDR_USA) == 0x02;
    if (!is_patched && (IS_TAG_MODE || ignore_mode)) {
        is_patched = 1;
        int hook_i = 0;
        
        if (REF_BYTE(REGION_ADDR) == 0x0) {
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_JPN, J_USER(&player_info_hook)};
            patches[hook_i++] = (Patch) {LOAD_COORDINATE_HOOK_ADDR_JPN, J_USER(&load_coordinate_hook)};
            patches[hook_i++] = (Patch) {PLAYER_1_SET_POS_HOOK_ADDR_JPN, J_USER(&set_p1_pos_hook)};
            patches[hook_i++] = (Patch) {PLAYER_2_SET_POS_HOOK_ADDR_JPN, J_USER(&set_p2_pos_hook)};
            patches[hook_i++] = (Patch) {SET_ATTACK_VALUE_HOOK_ADDR_JPN, J_USER(&set_attack_value_hook)};
        }
        else{
            patches[hook_i++] = (Patch) {PLAYER_INIT_HOOK_ADDR_USA, J_USER(&player_info_hook)};
            patches[hook_i++] = (Patch) {LOAD_COORDINATE_HOOK_ADDR_USA, J_USER(&load_coordinate_hook)};
            patches[hook_i++] = (Patch) {PLAYER_1_SET_POS_HOOK_ADDR_USA, J_USER(&set_p1_pos_hook)};
            patches[hook_i++] = (Patch) {PLAYER_2_SET_POS_HOOK_ADDR_USA, J_USER(&set_p2_pos_hook)};
            patches[hook_i++] = (Patch) {SET_ATTACK_VALUE_HOOK_ADDR_USA, J_USER(&set_attack_value_hook)};
        }
        
        for (Patch *patch = patches; patch->addr; patch++) {
            patch->ori_inst = REF(patch->addr);
            patch_instruction(patch->addr, patch->inst);
        }
    } else if (is_patched && !IS_TAG_MODE && !ignore_mode) {
        is_patched = 0;

        for (Patch *patch = patches; patch->addr; patch++) {
            patch_instruction(patch->addr, patch->ori_inst);
        }
    }
}


void mode_changed_hook() {
    ((void (*)(int))(USER_ADDR(_start)))(0);
}

void tag_mode_entered_hook() {
    ((void (*)(int))(USER_ADDR(_start)))(1);
}

void init(int user_text_addr) {
    patch_instruction(SET_GAME_MODE_HOOK_ADDR, J(make_trampoline(0, user_text_addr, (char*)&mode_changed_hook - &__executable_start + user_text_addr)));
    patch_instruction(LOAD_TAG_MODE_HOOK_ADDR, J(make_trampoline(0, user_text_addr, (char*)&tag_mode_entered_hook - &__executable_start + user_text_addr)));
}

void load_module_to_user_space() {
    int elf_size = &end - &__executable_start;
    SceUID block_id = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "", PSP_SMEM_High, elf_size, NULL);
    int user_text_addr = (u32)sceKernelGetBlockHeadAddr(block_id);
    memcpy((void*)user_text_addr, &__executable_start, elf_size);
    //init_addr();
    //init_addr();
    init(user_text_addr);
}

static STMOD_HANDLER previous;
int OnModuleStart(SceModule2 *mod) {
	if (strcmp(mod->modname, "Model") == 0) {
        init_patch_buffer();
	    load_module_to_user_space();
	}
	if (!previous)
		return 0;
	return previous(mod);
}

int module_start(SceSize args, void* argp) {
    int is_emulator = 0;
    sceIoDevctl("kemulator:", EMULATOR_DEVCTL__IS_EMULATOR, NULL, 0, &is_emulator, 4);

    if (is_emulator) {
        init_patch_buffer();
        load_module_to_user_space();
    } else {
        previous = sctrlHENSetStartModuleHandler(OnModuleStart);
    }

    return 0;
}