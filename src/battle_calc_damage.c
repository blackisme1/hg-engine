#include "../include/battle.h"
#include "../include/pokemon.h"
#include "../include/types.h"
#include "../include/constants/ability.h"
#include "../include/constants/hold_item_effects.h"
#include "../include/constants/item.h"
#include "../include/constants/move_effects.h"
#include "../include/constants/moves.h"
#include "../include/constants/species.h"

static const u8 HeldItemPowerUpTable[][2]={
	{HOLD_EFFECT_BOOST_BUG, TYPE_BUG},
	{HOLD_EFFECT_BOOST_STEEL, TYPE_STEEL},
	{HOLD_EFFECT_BOOST_GROUND, TYPE_GROUND},
	{HOLD_EFFECT_BOOST_ROCK, TYPE_ROCK},
	{HOLD_EFFECT_BOOST_GRASS, TYPE_GRASS},
	{HOLD_EFFECT_BOOST_DARK, TYPE_DARK},
	{HOLD_EFFECT_BOOST_FIGHTING, TYPE_FIGHTING},
	{HOLD_EFFECT_BOOST_ELECTRIC, TYPE_ELECTRIC},
	{HOLD_EFFECT_BOOST_WATER, TYPE_WATER},
	{HOLD_EFFECT_BOOST_FLYING, TYPE_FLYING},
	{HOLD_EFFECT_BOOST_POISON, TYPE_POISON},
	{HOLD_EFFECT_BOOST_ICE, TYPE_ICE},
	{HOLD_EFFECT_BOOST_GHOST, TYPE_GHOST},
	{HOLD_EFFECT_BOOST_PSYCHIC, TYPE_PSYCHIC},
	{HOLD_EFFECT_BOOST_FIRE, TYPE_FIRE},
	{HOLD_EFFECT_BOOST_DRAGON, TYPE_DRAGON},
	{HOLD_EFFECT_BOOST_NORMAL, TYPE_NORMAL},
	{HOLD_EFFECT_PLATE_BOOST_FIRE, TYPE_FIRE},
	{HOLD_EFFECT_PLATE_BOOST_WATER, TYPE_WATER},
	{HOLD_EFFECT_PLATE_BOOST_ELECTRIC, TYPE_ELECTRIC},
	{HOLD_EFFECT_PLATE_BOOST_GRASS, TYPE_GRASS},
	{HOLD_EFFECT_PLATE_BOOST_ICE, TYPE_ICE},
	{HOLD_EFFECT_PLATE_BOOST_FIGHTING, TYPE_FIGHTING},
	{HOLD_EFFECT_PLATE_BOOST_POISON, TYPE_POISON},
	{HOLD_EFFECT_PLATE_BOOST_GROUND, TYPE_GROUND},
	{HOLD_EFFECT_PLATE_BOOST_FLYING, TYPE_FLYING},
	{HOLD_EFFECT_PLATE_BOOST_PSYCHIC, TYPE_PSYCHIC},
	{HOLD_EFFECT_PLATE_BOOST_BUG, TYPE_BUG},
	{HOLD_EFFECT_PLATE_BOOST_ROCK, TYPE_ROCK},
	{HOLD_EFFECT_PLATE_BOOST_GHOST, TYPE_GHOST},
	{HOLD_EFFECT_PLATE_BOOST_DRAGON, TYPE_DRAGON},
	{HOLD_EFFECT_PLATE_BOOST_DARK, TYPE_DARK},
	{HOLD_EFFECT_PLATE_BOOST_STEEL, TYPE_STEEL},
};

static const u16 IronFistMovesTable[] = {
	MOVE_ICE_PUNCH,
    MOVE_FIRE_PUNCH,
    MOVE_THUNDER_PUNCH,
    MOVE_MACH_PUNCH,
    MOVE_FOCUS_PUNCH,
    MOVE_DIZZY_PUNCH,
    MOVE_DYNAMIC_PUNCH,
    MOVE_HAMMER_ARM,
    MOVE_MEGA_PUNCH,
    MOVE_COMET_PUNCH,
    MOVE_METEOR_MASH,
    MOVE_SHADOW_PUNCH,
    MOVE_DRAIN_PUNCH,
    MOVE_BULLET_PUNCH,
    MOVE_SKY_UPPERCUT,
};

const u8 StatBoostModifiers[][2] = {
         // numerator, denominator
        {          10,          40 },
        {          10,          35 },
        {          10,          30 },
        {          10,          25 },
        {          10,          20 },
        {          10,          15 },
        {          10,          10 },
        {          15,          10 },
        {          20,          10 },
        {          25,          10 },
        {          30,          10 },
        {          35,          10 },
        {          40,          10 },
};

int CalcBaseDamage(void *bw, struct BattleStruct *sp, int moveno, u32 side_cond, 
                   u32 field_cond, u16 pow, u8 type, u8 attacker, u8 defender, u8 critical)
{
    u32 i;
    s32 damage = 0;
    s32 damage2 = 0;
    u8 movetype;
    u8 movesplit;
    u16 attack;
    u16 defense;
    u16 sp_attack;
    u16 sp_defense;
    s8 atkstate;
    s8 defstate;
    s8 spatkstate;
    s8 spdefstate;
    u8 level;
    u16 movepower;
    u16 item;
    u32 battle_type;

    struct sDamageCalc AttackingMon;
    struct sDamageCalc DefendingMon;

    attack = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_ATK, NULL);
    defense = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_DEF, NULL);
    sp_attack = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_SPATK, NULL);
    sp_defense = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_SPDEF, NULL);

    atkstate = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_STATE_ATK, NULL) - 6;
    defstate = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_STATE_DEF, NULL) - 6;
    spatkstate = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_STATE_SPATK, NULL) - 6;
    spdefstate = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_STATE_SPDEF, NULL) - 6;

    level = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_LEVEL, NULL);

    AttackingMon.species = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_SPECIES, NULL);
    DefendingMon.species = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_SPECIES, NULL);
    AttackingMon.hp = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_HP, NULL);
    DefendingMon.hp = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_HP, NULL);
    AttackingMon.maxhp = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_MAX_HP, NULL);
    DefendingMon.maxhp = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_MAX_HP, NULL);
    AttackingMon.condition = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_MAX_CONDITION, NULL);
    DefendingMon.condition = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_MAX_CONDITION, NULL);
    AttackingMon.ability = GetBattlerAbility(sp, attacker);
    DefendingMon.ability = GetBattlerAbility(sp, defender);
    AttackingMon.sex = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_SEX, NULL);
    DefendingMon.sex = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_SEX, NULL);
    AttackingMon.type1 = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_TYPE1, NULL);
    DefendingMon.type1 = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_TYPE1, NULL);
    AttackingMon.type2 = BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_TYPE2, NULL);
    DefendingMon.type2 = BattlePokemonParamGet(sp, defender, BATTLE_MON_DATA_TYPE2, NULL);
    
    item = GetBattleMonItem(sp, attacker);
    AttackingMon.item_held_effect = BattleItemDataGet(sp, item, 1);
    AttackingMon.item_power = BattleItemDataGet(sp, item, 2);

    item = GetBattleMonItem(sp, defender);
    DefendingMon.item_held_effect = BattleItemDataGet(sp, item, 1);
    DefendingMon.item_power = BattleItemDataGet(sp, item, 2);

    battle_type = BattleTypeGet(bw);

    if (pow == 0)
        movepower = sp->old_moveTbl[moveno].power;
    else
        movepower = pow;

    // get the type
    if (AttackingMon.ability == ABILITY_NORMALIZE)
        movetype = TYPE_NORMAL;
    else if (type == 0)
        movetype = sp->old_moveTbl[moveno].type;
    else
        movetype = type & 0x3f;

    movepower = movepower * sp->damage_value / 10;

    // handle charge
    if ((sp->battlemon[attacker].effect_of_moves & MOVE_EFFECT_FLAG_CHARGE) && (movetype == TYPE_ELECTRIC))
        movepower *= 2;

    // handle helping hand
    if (sp->oneTurnFlag[attacker].helping_hand_flag)
        movepower = movepower * 15 / 10;

    // handle technician
    if ((AttackingMon.ability == ABILITY_TECHNICIAN) && (moveno != MOVE_STRUGGLE) && (movepower <= 60))
        movepower = movepower * 15 / 10;

    movesplit = sp->old_moveTbl[moveno].split;

    // handle huge power + pure power
    if ((AttackingMon.ability == ABILITY_HUGE_POWER) || (AttackingMon.ability == ABILITY_PURE_POWER))
        attack = attack * 2;

    // handle slow start
    if ((AttackingMon.ability == ABILITY_SLOW_START) 
     && ((BattleWorkMonDataGet(bw, sp, 3, 0) - BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_SLOW_START_COUNTER, NULL)) < 5))
        attack /= 2;

    // type boosting held items
    for (i = 0; i < NELEMS(HeldItemPowerUpTable); i++)
    {
        if ((AttackingMon.item_held_effect == HeldItemPowerUpTable[i][0]) && (movetype == HeldItemPowerUpTable[i][1]))
        {
            movepower = movepower * (100 + AttackingMon.item_power) / 100;
            break;
        }
    }
    // handle choice band
    if (AttackingMon.item_held_effect == HOLD_EFFECT_CHOICE_BAND)
        attack = attack * 150 / 100;

    // handle choice specs
    if (AttackingMon.item_held_effect == HOLD_EFFECT_CHOICE_SPECS)
        sp_attack = sp_attack * 150 / 100;

    // handle soul dew
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_SOUL_DEW) &&
        ((battle_type & BATTLE_TYPE_BATTLE_TOWER) == 0) &&
        ((AttackingMon.species == SPECIES_LATIOS) || (AttackingMon.species == SPECIES_LATIAS)))
        sp_attack = sp_attack * 150 / 100;

    if ((DefendingMon.item_held_effect == HOLD_EFFECT_SOUL_DEW) &&
        ((battle_type & BATTLE_TYPE_BATTLE_TOWER) == 0) &&
        ((DefendingMon.species == SPECIES_LATIOS) || (DefendingMon.species == SPECIES_LATIAS)))
        sp_defense = sp_defense * 150 / 100;

    // handle deep sea tooth
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_DEEP_SEA_TOOTH) && (AttackingMon.species == SPECIES_CLAMPERL))
        sp_attack *= 2;

    // handle deep sea scale
    if ((DefendingMon.item_held_effect == HOLD_EFFECT_DEEP_SEA_SCALE) && (DefendingMon.species == SPECIES_CLAMPERL))
        sp_defense *= 2;

    // handle light ball
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_LIGHT_BALL) && (AttackingMon.species == SPECIES_PIKACHU))
        movepower *= 2;

    // handle metal powder
    if ((DefendingMon.item_held_effect == HOLD_EFFECT_METAL_POWDER) && (DefendingMon.species == SPECIES_DITTO))
        defense *= 2;

    // handle thick club
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_THICK_CLUB)
     && ((AttackingMon.species == SPECIES_CUBONE)
      || (AttackingMon.species == SPECIES_MAROWAK)))
        attack *= 2;

    // handle adamant/lustrous/griseous orb
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_ADAMANT_ORB) &&
        ((movetype == TYPE_DRAGON) || (movetype == TYPE_STEEL)) &&
        (AttackingMon.species == SPECIES_DIALGA))
    {
        movepower = movepower * (100 + AttackingMon.item_power) / 100;
    }

    if ((AttackingMon.item_held_effect == HOLD_EFFECT_LUSTROUS_ORB) &&
        ((movetype == TYPE_DRAGON) || (movetype == TYPE_WATER)) &&
        (AttackingMon.species == SPECIES_PALKIA))
    {
        movepower = movepower * (100 + AttackingMon.item_power) / 100;
    }

    if ((AttackingMon.item_held_effect == HOLD_EFFECT_GRISEOUS_ORB) &&
        ((movetype == TYPE_DRAGON) || (movetype == TYPE_GHOST)) &&
        ((BattlePokemonParamGet(sp, attacker, BATTLE_MON_DATA_STATUS2, NULL) & STATUS2_FLAG_TRANSFORMED) == 0) &&
        (AttackingMon.species == SPECIES_GIRATINA))
    {
        movepower = movepower * (100 + AttackingMon.item_power) / 100;
    }

    // handle items that boost physical/special moves
    if ((AttackingMon.item_held_effect == HOLD_EFFECT_BOOST_PHYSICAL) && (movesplit == SPLIT_PHYSICAL))
    {
        movepower = movepower * (100 + AttackingMon.item_power) / 100;
    }

    if ((AttackingMon.item_held_effect == HOLD_EFFECT_BOOST_SPECIAL) && (movesplit == SPLIT_SPECIAL))
    {
        movepower = movepower * (100 + AttackingMon.item_power) / 100;
    }
    
    // handle thick fat
    if ((CheckDefenceAbility(sp, attacker, defender, ABILITY_THICK_FAT) == TRUE) &&
        ((movetype == TYPE_FIRE) || (movetype == TYPE_ICE)))
    {
        movepower /= 2;
    }

    // handle hustle
    if (AttackingMon.ability == ABILITY_HUSTLE)
    {
        attack = attack * 150 / 100;
    }

    // handle guts
    if ((AttackingMon.ability == ABILITY_GUTS) && (AttackingMon.condition))
    {
        attack = attack * 150 / 100;
    }

    // handle marvel scale
    if ((CheckDefenceAbility(sp, attacker, defender, ABILITY_MARVEL_SCALE) == TRUE) && (AttackingMon.condition))
    {
        defense = defense * 150 / 100;
    }

    // handle plus/minus
    if ((AttackingMon.ability == ABILITY_PLUS) &&
        (CheckSideAbility(bw, sp, CHECK_PLAYER_SIDE_ALIVE, attacker, ABILITY_MINUS)))
    {
        sp_attack = sp_attack * 150 / 100;
    }

    if ((AttackingMon.ability == ABILITY_MINUS) &&
        (CheckSideAbility(bw, sp, CHECK_PLAYER_SIDE_ALIVE, attacker, ABILITY_PLUS)))
    {
        sp_attack = sp_attack * 150 / 100;
    }

    // handle mud/water sport
    if ((movetype == TYPE_ELECTRIC) && (CheckFieldMoveEffect(bw, sp, MOVE_EFFECT_FLAG_MUD_SPORT)))
    {
        movepower /= 2;
    }

    if ((movetype == TYPE_FIRE) && (CheckFieldMoveEffect(bw, sp, MOVE_EFFECT_FLAG_WATER_SPORT)))
    {
        movepower /= 2;
    }

    // handle "in a pinch" type boosters
    if ((movetype == TYPE_GRASS) && (AttackingMon.ability == ABILITY_OVERGROW) && (AttackingMon.hp <= AttackingMon.maxhp * 10 / 30))
    {
        movepower = movepower * 150 / 100;
    }

    if ((movetype == TYPE_FIRE) && (AttackingMon.ability == ABILITY_BLAZE) && (AttackingMon.hp <= AttackingMon.maxhp * 10 / 30))
    {
        movepower = movepower * 150 / 100;
    }

    if ((movetype == TYPE_WATER) && (AttackingMon.ability == ABILITY_TORRENT) && (AttackingMon.hp <= AttackingMon.maxhp * 10 / 30))
    {
        movepower = movepower * 150 / 100;
    }

    if ((movetype == TYPE_BUG) && (AttackingMon.ability == ABILITY_SWARM) && (AttackingMon.hp <= AttackingMon.maxhp * 10 / 30))
    {
        movepower = movepower * 150 / 100;
    }

    // handle heatproof/dry skin
    if ((movetype == TYPE_FIRE) && (CheckDefenceAbility(sp, attacker, defender, ABILITY_HEATPROOF) == TRUE))
    {
        movepower /= 2;
    }

	if ((movetype == TYPE_FIRE) && (CheckDefenceAbility(sp, attacker, defender, ABILITY_DRY_SKIN) == TRUE))
    {
        movepower = movepower * 125 / 100;
    }
    
    // handle simple
    if (AttackingMon.ability == ABILITY_SIMPLE)
    {
        atkstate *= 2;
        if (atkstate < -6)
        {
            atkstate = -6;
        }
        if (atkstate > 6)
        {
            atkstate = 6;
        }
        spatkstate *= 2;
        if (spatkstate < -6)
        {
            spatkstate = -6;
        }
        if (spatkstate > 6)
        {
            spatkstate = 6;
        }
    }

    if (CheckDefenceAbility(sp, attacker, defender, ABILITY_SIMPLE) == TRUE)
    {
        defstate *= 2;
        if (defstate < -6)
        {
            defstate = -6;
        }
        if (defstate > 6)
        {
            defstate = 6;
        }
        spdefstate *= 2;
        if (spdefstate < -6)
        {
            spdefstate = -6;
        }
        if (spdefstate > 6)
        {
            spdefstate = 6;
        }
    }

    // handle unaware
    if (CheckDefenceAbility(sp, attacker, defender, ABILITY_UNAWARE) == TRUE)
    {
        atkstate = 0;
        spatkstate = 0;
    }

    if (AttackingMon.ability == ABILITY_UNAWARE)
    {
        defstate = 0;
        spdefstate = 0;
    }

    // adjust states to access from the array
    atkstate += 6;
    defstate += 6;
    spatkstate += 6;
    spdefstate += 6;

    // handle rivalry
    if ((AttackingMon.ability == ABILITY_RIVALRY) &&
        (AttackingMon.sex == DefendingMon.sex) && (AttackingMon.sex != POKEMON_GENDER_UNKNOWN) && (DefendingMon.sex != POKEMON_GENDER_UNKNOWN))
    {
        movepower = movepower * 125 / 100;
    }

    if ((AttackingMon.ability == ABILITY_RIVALRY) &&
        (AttackingMon.sex != DefendingMon.sex) && (AttackingMon.sex != POKEMON_GENDER_UNKNOWN) && (DefendingMon.sex != POKEMON_GENDER_UNKNOWN))
    {
        movepower = movepower * 75 / 100;
    }

    // handle iron fist
    for (i = 0; i < NELEMS(IronFistMovesTable); i++)
    {
        if ((IronFistMovesTable[i] == moveno) && (AttackingMon.ability == ABILITY_IRON_FIST))
        {
            movepower = movepower * 12 / 10;
            break;
        }
    }

    // handle weather boosts
    if ((CheckSideAbility(bw, sp, CHECK_ALL_BATTLER_ALIVE, 0, ABILITY_CLOUD_NINE) == 0) &&
        (CheckSideAbility(bw, sp, CHECK_ALL_BATTLER_ALIVE, 0, ABILITY_AIR_LOCK) == 0))
    {
        if ((field_cond & WEATHER_SUNNY_ANY) && (AttackingMon.ability == ABILITY_SOLAR_POWER))
        {
            sp_attack = sp_attack * 15 / 10;
        }
        if ((field_cond & WEATHER_SANDSTORM_ANY) &&
            ((DefendingMon.type1 == TYPE_ROCK) || (DefendingMon.type2 == TYPE_ROCK)))
        {
            sp_defense = sp_defense * 15 / 10;
        }
        if ((field_cond & WEATHER_SUNNY_ANY) &&
            (CheckSideAbility(bw, sp, CHECK_PLAYER_SIDE_ALIVE, attacker, ABILITY_FLOWER_GIFT)))
        {
            attack = attack * 15 / 10;
        }
        if ((field_cond & WEATHER_SUNNY_ANY) &&
            (GetBattlerAbility(sp, attacker) != ABILITY_MOLD_BREAKER) &&
            (CheckSideAbility(bw, sp, CHECK_PLAYER_SIDE_ALIVE, defender, ABILITY_FLOWER_GIFT)))
        {
            sp_defense = sp_defense * 15 / 10;
        }
    }

    // halve the defense if using selfdestruct/explosion
    if (sp->old_moveTbl[moveno].effect == MOVE_EFFECT_HALVE_DEFENSE)
		defense = defense / 2;

    // handle physical moves
    if (movesplit == SPLIT_PHYSICAL)
    {
        if (critical > 1)
        {
            if (atkstate > 6)
            {
                damage = attack * StatBoostModifiers[atkstate][0];
                damage /= StatBoostModifiers[atkstate][1];
            }
            else
            {
                damage = attack;
            }
        }
        else
        {
            damage = attack * StatBoostModifiers[atkstate][0];
            damage /= StatBoostModifiers[atkstate][1];
        }

        damage *= movepower;
        damage *= (level * 2 / 5 + 2);

        if (critical > 1)
        {
            if (defstate < 6)
            {
                damage2 = defense * StatBoostModifiers[defstate][0];
                damage2 /= StatBoostModifiers[defstate][1];
            }
            else
            {
                damage2 = defense;
            }
        }
        else
        {
            damage2 = defense * StatBoostModifiers[defstate][0];
            damage2 /= StatBoostModifiers[defstate][1];
        }

        damage /= damage2;
        damage /= 50;

        // burns halve physical damage
        if ((AttackingMon.condition & STATUS_FLAG_BURNED) && (AttackingMon.ability != ABILITY_GUTS))
        {
            damage /= 2;
        }
        
        // handle reflect
        if (((side_cond & SIDE_STATUS_REFLECT) != 0) && 
            (critical == 1) && 
            (sp->old_moveTbl[moveno].effect != MOVE_EFFECT_REMOVE_SCREENS))
        {
            if ((battle_type & BATTLE_TYPE_DOUBLE) && (CheckNumMonsHit(bw, sp, 1, defender) == 2))
            {
                damage = damage * 2 / 3;
            }
            else
            {
                damage /= 2;
            }
        }
    }
    else// if (movesplit == SPLIT_SPECIAL) // same as above, handle special moves
    {
        if (critical > 1)
        {
            if (spatkstate > 6)
            {
                damage = sp_attack * StatBoostModifiers[spatkstate][0];
                damage /= StatBoostModifiers[spatkstate][1];
            }
            else
            {
                damage = sp_attack;
            }
        }
        else
        {
            damage = sp_attack * StatBoostModifiers[spatkstate][0];
            damage /= StatBoostModifiers[spatkstate][1];
        }

        damage *= movepower;
        damage *= (level * 2 / 5 + 2);

        if (critical > 1)
        {
            if (spdefstate < 6)
            {
                damage2 = sp_defense * StatBoostModifiers[spdefstate][0];
                damage2 /= StatBoostModifiers[spdefstate][1];
            }
            else
            {
                damage2 = sp_defense;
            }
        }
        else
        {
            damage2 = sp_defense * StatBoostModifiers[spdefstate][0];
            damage2 /= StatBoostModifiers[spdefstate][1];
        }

        damage /= damage2;
        damage /= 50;

        // handle light screen
        if (((side_cond & SIDE_STATUS_LIGHT_SCREEN) != 0) && 
            (critical == 1) && 
            (sp->old_moveTbl[moveno].effect != MOVE_EFFECT_REMOVE_SCREENS))
        {
            if ((battle_type & BATTLE_TYPE_DOUBLE) && (CheckNumMonsHit(bw, sp, 1, defender) == 2))
            {
                damage = damage * 2 / 3;
            }
            else
            {
                damage /= 2;
            }
        }
    }

    if ((battle_type & BATTLE_TYPE_DOUBLE) &&
        (sp->old_moveTbl[moveno].target == 0x4) &&
        (CheckNumMonsHit(bw, sp, 1, defender) == 2))
    {
        damage = damage * 3 / 4;
    }

    if ((battle_type & BATTLE_TYPE_DOUBLE) &&
        (sp->old_moveTbl[moveno].target == 0x8) &&
        (CheckNumMonsHit(bw, sp, 1, defender) >= 2))
    {
        damage = damage * 3 / 4;
    }

    // handle weather inate type boosts
    if ((CheckSideAbility(bw, sp, CHECK_ALL_BATTLER_ALIVE, 0, ABILITY_CLOUD_NINE) == 0) &&
        (CheckSideAbility(bw, sp, CHECK_ALL_BATTLER_ALIVE, 0, ABILITY_AIR_LOCK) == 0))
    {
        if (field_cond & WEATHER_RAIN_ANY) // handle rain boosts
        {
            switch (movetype)
            {
            case TYPE_FIRE:
                damage /= 2;
                break;
            case TYPE_WATER:
                damage = damage * 15 / 10;
                break;
            }
        }

        if ((field_cond & (FIELD_STATUS_FOG | WEATHER_HAIL_ANY | WEATHER_SANDSTORM_ANY | WEATHER_RAIN_ANY)) && (moveno == MOVE_SOLAR_BEAM))
        {
            damage /= 2;
        }
        if (field_cond & WEATHER_SUNNY_ANY)
        {
            switch (movetype)
            {
            case TYPE_FIRE:
                damage = damage * 15 / 10;
                break;
            case TYPE_WATER:
                damage /= 2;
                break;
            }
        }
    }

    if ((BattlePokemonParamGet(sp, attacker, BATTLE_MON_FLASH_FIRE_ACTIVATED, NULL)) && (movetype == TYPE_FIRE))
    {
        damage = damage * 15 / 10;
    }
    
    return damage + 2;
}

// GetBattleMonItem needs to be rewritten AND hooked from.  interesting
u16	GetBattleMonItem(struct BattleStruct *sp, int client_no)
{
	if ((GetBattlerAbility(sp, client_no) == ABILITY_KLUTZ))
    {
		return 0;
	}
	if (sp->battlemon[client_no].moveeffect.embargo_count)
    {
		return 0;
	}
    // handle unnerve:  if an opposing pokemon has unnerve and the item it is holding is a berry, then no item will be read.
    if (((GetBattlerAbility(sp, BATTLER_OPPONENT(client_no)) == ABILITY_UNNERVE && sp->battlemon[BATTLER_OPPONENT(client_no)].hp != 0) 
      || (GetBattlerAbility(sp, BATTLER_ACROSS(client_no)) == ABILITY_UNNERVE && sp->battlemon[BATTLER_ACROSS(client_no)].hp != 0))
     && (IS_ITEM_BERRY(sp->battlemon[client_no].item)))
    {
        return 0;
    }
	
	return sp->battlemon[client_no].item;
}