/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2013 Project Cerberus <http://www.erabattle.ru/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is not free software; you can not redistribute it and/or modify it.
 *
 * This program is distributed only by <http://www.erabattle.ru/>!
 */

/* ScriptData
SDName: Boss_Magmadar
SD%Complete: 75
SDComment: Conflag on ground nyi
SDCategory: Molten Core
EndScriptData */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "molten_core.h"

enum Texts
{
    EMOTE_FRENZY        = 0
};

enum Spells
{
    SPELL_FRENZY        = 19451,
    SPELL_MAGMA_SPIT    = 19449,
    SPELL_PANIC         = 19408,
    SPELL_LAVA_BOMB     = 19428,
};

enum Events
{
    EVENT_FRENZY        = 1,
    EVENT_PANIC         = 2,
    EVENT_LAVA_BOMB     = 3,
};

class boss_magmadar : public CreatureScript
{
    public:
        boss_magmadar() : CreatureScript("boss_magmadar") { }

        struct boss_magmadarAI : public BossAI
        {
            boss_magmadarAI(Creature* creature) : BossAI(creature, BOSS_MAGMADAR)
            {
            }

            void Reset() OVERRIDE
            {
                BossAI::Reset();
                DoCast(me, SPELL_MAGMA_SPIT, true);
            }

            void EnterCombat(Unit* victim) OVERRIDE
            {
                BossAI::EnterCombat(victim);
                events.ScheduleEvent(EVENT_FRENZY, 30000);
                events.ScheduleEvent(EVENT_PANIC, 20000);
                events.ScheduleEvent(EVENT_LAVA_BOMB, 12000);
            }

            void UpdateAI(uint32 diff) OVERRIDE
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FRENZY:
                            Talk(EMOTE_FRENZY);
                            DoCast(me, SPELL_FRENZY);
                            events.ScheduleEvent(EVENT_FRENZY, 15000);
                            break;
                        case EVENT_PANIC:
                            DoCastVictim(SPELL_PANIC);
                            events.ScheduleEvent(EVENT_PANIC, 35000);
                            break;
                        case EVENT_LAVA_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_LAVA_BOMB))
                                DoCast(target, SPELL_LAVA_BOMB);
                            events.ScheduleEvent(EVENT_LAVA_BOMB, 12000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const OVERRIDE
        {
            return new boss_magmadarAI(creature);
        }
};

void AddSC_boss_magmadar()
{
    new boss_magmadar();
}
