.nds
.thumb

.include "armips/include/battlescriptcmd.s"
.include "armips/include/abilities.s"
.include "armips/include/itemnums.s"
.include "armips/include/monnums.s"
.include "armips/include/movenums.s"

.create "build/move/battle_sub_seq/1_359", 0

// Cackle

a001_359:
    ifmonstat IF_NOTEQUAL, BATTLER_ADDL_EFFECT, MON_DATA_STAT_STAGE_ATTACK, 0, StatusEffect
    ifmonstat IF_NOTEQUAL, BATTLER_ADDL_EFFECT, MON_DATA_STAT_STAGE_DEFENSE, 0, StatusEffect
    goto NoStatusEffect
StatusEffect:
    gotosubscript 76
	ifmonstat IF_NOTEQUAL, BATTLER_DEFENDER, MON_DATA_STATUS_1, 0, Statused
	ifmonstat IF_EQUAL, BATTLER_ADDL_EFFECT, MON_DATA_STAT_STAGE_SPEED, 12, StatusEffectNoSpeed
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, SPEED_UP
    gotosubscript 12
	changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x200000
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x4001
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS2, 0x80
	changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x2
StatusEffectNoSpeed:
	changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x200000
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x4001
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS2, 0x80
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, ATTACK_DOWN
    gotosubscript 12
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, DEFENSE_DOWN
    gotosubscript 12
    changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x2
    changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x80
    endscript
Statused:
	ifmonstat IF_EQUAL, BATTLER_ADDL_EFFECT, MON_DATA_STAT_STAGE_SPEED, 12, StatusEffectNoSpeed
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x200000
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x4001
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS2, 0x80
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, SPEED_UP_2
    gotosubscript 12
	changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x200000
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS1, 0x4001
    changevar VAR_OP_SETMASK, VAR_SERVER_STATUS2, 0x80
	changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x2
StatusedNoSpeed:
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, ATTACK_DOWN_2
    gotosubscript 12
    changevar VAR_OP_SET, VAR_ADD_EFFECT_ATTRIBUTE, DEFENSE_DOWN_2
    gotosubscript 12
    changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x2
    changevar VAR_OP_CLEARMASK, VAR_SERVER_STATUS2, 0x80
    endscript
NoStatusEffect:
    changevar VAR_OP_SETMASK, VAR_MOVE_STATUS, 0x40
    endscript
.close